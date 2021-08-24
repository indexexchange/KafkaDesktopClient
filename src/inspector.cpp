#include "inspector.h"

#include <regex>

#include <wx/app.h>
#include <wx/stc/stc.h>

#include "guitools.h"
#include "jq/filter.h"
#include "kafka.h"
#include "logger.h"
#include "settings.h"
#include "util.h"

Inspector::Inspector(wxWindow* window) : InspectorPanel(window)
{

    // Decorate gui_text_view (colors, folding)
    guitools::decorate_styledtextctrl(gui_text_view);

    // copy theme from list view because text view does not change with the OS theme...
    guitools::theme_styledtextctrl(gui_text_view, gui_list_view->GetBackgroundColour(), gui_list_view->GetForegroundColour());

    // Connect text view margin click to folding text function
    gui_text_view->Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(Inspector::OnMarginClick), NULL, this);

    //
    scheduler_ = std::make_shared<Scheduler>(150);
    // 
    is_streaming_ = std::make_shared<bool>(false);

    // add columns to inspector data view (it's empty when constructed. could also call this in context constructor.)
    gui_list_view->AppendTextColumn("ID");
    gui_list_view->AppendTextColumn("MESSAGE");
    
    SyncCache();

    gui_text_view->SetText(help);

    SetGUIState(State::Idle);
}

void Inspector::SetFilter(std::string value)
{
    gui_filter->SetLabelText(value);
}

std::string Inspector::GetFilter()
{
    return gui_filter->GetValue().ToStdString();
}

void Inspector::SetTopic(std::string value)
{
    gui_topic->SetLabelText(value);
}

std::string Inspector::GetTopic()
{
    return gui_topic->GetValue().ToStdString();
}

std::vector<std::shared_ptr<std::string>> Inspector::GetDisplayResults()
{
    std::vector<std::shared_ptr<std::string>> results;
    int size = gui_list_view->GetItemCount();

    for (int i = 0; i < size; ++i) {
        auto item = std::make_shared<std::string>(gui_list_view->GetTextValue(i, 1));
        results.push_back(item);
    }
    return results;
}

void Inspector::AddToBuffer(std::shared_ptr<std::string> request)
{
    buffer_.push(request);
}

void Inspector::AddToBuffer(std::string request)
{
    auto r = std::make_shared<std::string>(request);
    buffer_.push(r);
}

void Inspector::ForceRefresh(void)
{
    // This is a workaround b/c I don't know a better way to force a refresh on inspector panel
    // ie. buffer has data -> show it in gui
    wxCommandEvent e;
    OnFilter(e);
}

Inspector::~Inspector()
{
    // Save settings file
    settings::cache::Save();
}

void Inspector::OnRun(wxCommandEvent& event)
{

    SetGUIState(State::Connecting);

    // Gather user input
    int max = gui_buffer_size->GetValue();
    std::string topic = gui_topic->GetValue().ToStdString();
    std::string brokers = gui_brokers->GetValue().ToStdString();
    std::string filter_query = gui_filter->GetValue().ToStdString();

    filter_query_ = filter_query;

    auto filter = jq::FilterPtr(filter_query_);

    try {
        filter->Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        SetGUIState(State::Idle);
        return;
    }

    SyncCache();
    buffer_.clear();

    logger_debug << "consuming from topic: " << topic << ", brokers: " << brokers << ", max packets: " << max;
    ShowMessage("searching " + std::to_string(max) + " messages from " + topic + " with filter  " + filter_query + ", please wait...");

    // Start streaming thread for consuming packets
    auto t = std::thread([this, max, topic, brokers, filter]() {

        *is_streaming_ = true;
        int count = 0;
        long long total = 0;

        // Consume is a blocking call and needs to be run on a separate thread.
        // we pass o to consume method because we want it to keep consuming until we stop the stream with our own logic
        kafka::consume(brokers, { topic }, 0, is_streaming_, [this, filter, &count, &total, max](std::shared_ptr<std::string> packet, std::shared_ptr<std::string>) {

            if (total == 0) {
                // Update GUI: we're now connected to brokers and actively consuming data
                scheduler_->CallLaterOnMainThread([this]() {
                    SetGUIState(State::Running);
                });
            }

            // On each received packet:
            total++;
            auto result = filter->Apply(packet);

            if (jq::has_data(result)) {
                buffer_.push(packet);

                // Callback to update GUI, needs to be run on main GUI thread.
                scheduler_->CallLaterOnMainThread([this, count, result]() {
                    auto id = std::to_string(count);

                    //dataViewList
                    wxVector<wxVariant> data;
                    data.push_back(wxVariant(id));
                    wxString wstr = wxString::FromUTF8(result->c_str());
                    data.push_back(wxVariant(wstr));
                    gui_list_view->AppendItem(data);
                });
                ++count;
                
                if (max != 0 && max == count) {
                    *is_streaming_ = false;
                }
            }

            if (total % 20) {
                scheduler_->CallLaterOnMainThread([this, count, total]() {
                    ShowMessage("consumed: " + std::to_string(total) + ", found: " + std::to_string(count));
                });
            }
        });

        *is_streaming_ = false;

        scheduler_->CallLaterOnMainThread([this, max, topic]() {
            SetGUIState(State::Idle);
            UpdateStats(); 
            ShowMessage("stream closed, consumed " + std::to_string(buffer_.size()) + " messages from " + topic);
        });

    });
    t.detach();
}

void Inspector::OnStop(wxCommandEvent& event)
{
    SetGUIState(State::Idle);
    *is_streaming_ = false;
}

void Inspector::OnFilter(wxCommandEvent& event)
{
    ShowMessage("filtering...");

    auto start = std::chrono::high_resolution_clock::now();

    filter_query_ = gui_filter->GetValue();


    // If user clears the filter then simply display all requests from buffer
    if (filter_query_.empty()) {
        if (gui_list_view->GetItemCount() > 0) {
            gui_list_view->DeleteAllItems();
        }

        for (size_t i = 0; i < buffer_.size(); ++i) {
            wxVector<wxVariant> data;
            data.push_back(wxVariant(std::to_string(i)));            
            wxString wstr = wxString::FromUTF8(buffer_[i]->c_str());
            data.push_back(wstr);
            gui_list_view->AppendItem(data);
        }
        UpdateStats();
        return;
    }

    // At this point we have a non-empty filter. Apply it and show results.
    logger_info << "Applying \"" << filter_type_ << "\", \"" << filter_query_ << "\", buffer size: " << buffer_.size();

    auto filter = jq::FilterAPI(filter_query_);

    try {
        filter.Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        return;
    }

    // Clear list
    if (gui_list_view->GetItemCount() > 0) {
        gui_list_view->DeleteAllItems();
    }

    // Sync current values with cache
    SyncCache();

    for (size_t i = 0; i < buffer_.size(); ++i) {

        auto result = filter.Apply(buffer_[i]);

        if (jq::has_data(result)) {
            wxVector<wxVariant> data;
            data.push_back(wxVariant(std::to_string(i)));
            // data.push_back(*result);
            wxString wstr = wxString::FromUTF8(result->c_str());
            data.push_back(wstr);
            gui_list_view->AppendItem(data);
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    ShowMessage("filtered in " + std::to_string(duration.count()) + "ms");
    UpdateStats();
}

void Inspector::OnKeyUpFilter(wxKeyEvent& event)
{
    auto key = event.GetKeyCode();
    if (key == WXK_RETURN) {
        wxCommandEvent e;
        OnFilter(e);
    }
}

void Inspector::OnSelect(wxDataViewEvent& event)
{
    auto row = gui_list_view->GetSelectedRow();

    // on macOS deleting items from listview is sending an OnSelect event with invalid row. verify.
    if (row == -1)
        return;

    wxVariant idx;
    gui_list_view->GetValue(idx, row, 0);
    auto data = buffer_[idx.GetLong()];

    logger_trace << "Selected row: " << row << ", data: \n" << data;

    auto filter = jq::FilterAPI(filter_query_);

    try {
        filter.Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        return;
    }

    try {
        auto result = filter.Apply(data);
        std::string str = *jq::pretty_json(result);
        gui_text_view->SetText(str);

        if(gui_text_view->IsEmpty() && !str.empty()){
            // Special case for linux, text does not get displayed at all (blank). This is a workaround.
            wxString wstr = wxString::FromUTF8(str.c_str());
            gui_text_view->SetValue(wstr);
        }

        // highlight search word if any
        wxCommandEvent evt;
        OnSearch(evt);
        return;
    }
    catch (...) {
        logger_err << "failed to parse a non-json string";
    }

    // At this point, we were not able to display the selected message because it's not in json format.
    // Display as-is
    gui_text_view->SetText(*data);

}

void Inspector::OnSearchPartial(wxCommandEvent& event)
{
    std::string word = gui_search->GetValue().ToStdString();
    guitools::search_partial_styledtextctrl(gui_text_view, word);
    
}

void Inspector::OnSearch(wxCommandEvent& event)
{
    // search for all matches
    std::string word = gui_search->GetValue().ToStdString();
    guitools::search_styledtextctrl(gui_text_view, word);
}

void Inspector::OnSearchCancel(wxCommandEvent& event)
{
    gui_search->Clear();
    gui_text_view->IndicatorClearRange(0, gui_text_view->GetTextLength());
}

void Inspector::SetGUIState(State s)
{
    if (s == state_) {
        return;
    }
    state_ = s;

    switch (state_)
    {
    case Inspector::State::Idle:
        logger_info << "IDLE";
        
        // Set button logo
        guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
        guitools::apply_graphic(gui_button_filter, guitools::PNG::FILTER);
        gui_button_run->SetLabelText("Run");
        gui_button_run->Enable();
        gui_button_filter->Enable();
        gui_filter->Enable();

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Inspector::OnRun), NULL, this);

        break;
    case Inspector::State::Connecting:
        logger_info << "CONNECTING";
        // update logo
        guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);
        gui_button_run->SetLabelText("Stop");
        gui_button_filter->Disable();
        gui_filter->Disable();

        // Clear display 
        gui_list_view->DeleteAllItems();
        gui_text_view->SetText("");

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Inspector::OnStop), NULL, this);

        break;
    case Inspector::State::Running:
        logger_info << "RUNNING";
        // update button logo
        guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);
        gui_button_run->SetLabelText("Stop");
        gui_button_run->Enable();
        gui_button_filter->Disable();
        gui_filter->Disable();

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Inspector::OnStop), NULL, this);
        break;
    default:
        break;
    }
    // gui_button_connect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Database::OnDisconnect), NULL, this);
}

void Inspector::SyncCache()
{
    bool is_filter_empty = gui_filter->GetValue().empty();
    // update cache with new request
    settings::cache::Update(
        gui_buffer_size->GetValue(),
        gui_topic->GetValue().ToStdString(),
        gui_filter->GetValue().ToStdString()
    );
    

    // update filter and topic drop downs with latest data
    auto filters = settings::cache::get_filters();

    gui_filter->Clear();
    
    if (is_filter_empty) {
        gui_filter->AppendText("");
    }
    else {
        gui_filter->AppendText(*filters.begin());
    }

    for (auto& f : filters) {
        gui_filter->Append(f);
    }

    auto topics = settings::cache::get_topics();

    gui_topic->Clear();
    // set current selection to most recent
    gui_topic->AppendText(*topics.begin());
    for (auto& t : topics) {
        gui_topic->Append(t);
    }

    if (is_filter_empty) {
        gui_filter->SetLabelText("");
    }
}

void Inspector::UpdateStats()
{
    std::stringstream ss;

    auto showing = gui_list_view->GetItemCount();
    auto total = buffer_.size();
    auto pct = 0.0f;
    if (total != 0) {
        pct = float(showing) / (float(total)) * 100.0f;
    }
    ss << "Showing: " << showing << "/" << total << " (" << pct << "%)";

    logger_debug << "Showing: " << showing << "/" << total;
    ShowMessage(ss.str());
}

void Inspector::ShowMessage(std::string msg)
{
    gui_console->SetLabelText(msg);
}

void Inspector::ShowErrorDialog(std::string msg)
{
    std::stringstream ss;
    ss << "There's a problem with that filter: ";
    ss << msg << "\n\n";
    wxString error(ss.str());
    wxMessageDialog dial(NULL,
        error, wxT("JQ Filter"), wxOK | wxICON_ERROR);
    dial.ShowModal();
}

void Inspector::OnMarginClick(wxStyledTextEvent& event)
{
    guitools::fold_styledtextctrl(event, gui_text_view);
}

const std::string Inspector::help = R"#(To begin, click Run

Run with no filter -> will collect 100 requests
Run with provided filter -> will collect 100 requests that pass provided filter.

JQ Help:

# Field access
.company
.team.name

# Array access
.team.members[0]
.team.members[0].name   # from json object, get first index in array, show name

# Piping intermediary result to next filter
.team | .members[0] | .name

# Regex(<expr no quotes>), return matched strings, otherwise empty
# Can be applied to any object (not just string)
regex(page.*www.yahoo.com)

# Contains(<string no quotes>), return bool if string is found otherwise false
# Can be applied to any object (not just string)
contains(msn.com)    

# Select
select(.errno != null)      # is not null
select(.errno == null)      # is null
select(.country == "US") | .company

select(.team.members[1])         # members[1] not null, same as select(.team.members[1] != null)
select(.team.members[1] | contains(Fan))

# Other examples
select(regex(page.*www.yahoo.com)) | .company
select(.country == "US" and .team.members[0].name == "Bob")

# Comparison
select(.age > 90)    
select(.age != 25)    # equivalent to select(.age == 25 | not) 
select(.number >= 500 and .age > 90)
)#";