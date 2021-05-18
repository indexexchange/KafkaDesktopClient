#include "trafficmatch.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <memory>

#include <wx/stc/stc.h>

#include "guitools.h"
#include "jq/filter.h"
#include "kafka.h"
#include "packetviewui.h"
#include "scheduler.h"
#include "util.h"
#include "guitools.h"

const std::string TrafficMatch::file1_ = "temp_topic1_cache.txt";
const std::string TrafficMatch::file2_ = "temp_topic2_cache.txt";
const std::string TrafficMatch::scratch_ = "temp_scratchfile.txt";

TrafficMatch::TrafficMatch(wxWindow* window) : TrafficMatchPanel(window)
{
    // Apply graphics to button
    guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
    guitools::apply_graphic(gui_button_filter, guitools::PNG::FILTER);
    guitools::apply_graphic(gui_button_match, guitools::PNG::MATCH);

    // Decorate text view
    guitools::decorate_styledtextctrl(gui_text_view);
    // Connect folding function when user clicks margin
    gui_text_view->Connect(wxEVT_STC_MARGINCLICK, wxStyledTextEventHandler(TrafficMatch::OnMarginClick), NULL, this);

    // initialize scheduler for running things on main thread
	scheduler_ = std::make_shared<Scheduler>(150);

    is_streaming_ = std::make_shared<bool>(false);

    // add columns to inspector data view (it's empty when constructed. could also call this in context constructor.)
    gui_list_view->AppendTextColumn("ID");
    gui_list_view->AppendTextColumn("MESSAGE");

    // Add help text
    gui_text_view->SetText(help);
}

std::vector<std::shared_ptr<std::string>> TrafficMatch::GetDisplayResults()
{
    std::vector<std::shared_ptr<std::string>> results;
    int size = gui_list_view->GetItemCount();

    for (int i = 0; i < size; ++i) {
        auto item = std::make_shared<std::string>(gui_list_view->GetTextValue(i, 1));
        results.push_back(item);
    }
    return results;
}

TrafficMatch::~TrafficMatch()
{
}

void TrafficMatch::OnRun(wxCommandEvent& event)
{

    if (*is_streaming_) {
        // user cancelled a statistics run
        gui_button_run->SetLabelText("Run");
        guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
        *is_streaming_ = false;
        return;
    }
    
    // Clear all memory and cached files
    Clear();

    // Start a brand new run
    // Update flags
    *is_streaming_ = true;

    // Update buttons (Apply and Run)
    gui_button_run->SetLabelText("Stop");
    guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);

    // Gather user input
    int max_matches = gui_buffer_size->GetValue();
    std::string brokers = gui_brokers->GetValue();

    std::string topic1 = gui_topic1->GetValue();
    std::string topic2 = gui_topic2->GetValue();
    
    std::string filter1_query = gui_filter1->GetValue();
    std::string filter2_query = gui_filter2->GetValue();

    std::string prefilter1_query = gui_prefilter1->GetValue();
    std::string prefilter2_query = gui_prefilter2->GetValue();

    std::stringstream message;
    if (gui_choice->GetStringSelection() == "in") {
        message << "Extracting jq '" << filter1_query << "' from " << topic1 << " and matching the result as a string in " << topic2 << ", please wait...";
    }
    else {
        message << "Matching requests from " << topic1 << " with " << topic2 << ", please wait...";
    }
    ShowMessage(message.str());
    

    // Start streaming thread for consuming packets
    auto t = std::thread([this, max_matches, brokers, topic1, topic2, filter1_query, filter2_query, prefilter1_query, prefilter2_query]() {

        unsigned long total_count = 0;
        unsigned long match_count = 0;

        std::string topic1_name = topic1;
        std::string topic2_name = topic2;
        unsigned long topic1_count = 0;
        unsigned long topic1_prefilter = 0;
        unsigned long topic2_count = 0;
        unsigned long topic2_prefilter = 0;

        std::shared_ptr<jq::FilterAPI> f1 = jq::FilterPtr(filter1_query);
        std::shared_ptr<jq::FilterAPI> f2 = jq::FilterPtr(filter2_query);

        std::shared_ptr<jq::FilterAPI> prefilter1;
        if (!prefilter1_query.empty()) {
            prefilter1 = jq::FilterPtr(prefilter1_query);
        }
        std::shared_ptr<jq::FilterAPI> prefilter2;
        if (!prefilter2_query.empty()) {
            prefilter2 = jq::FilterPtr(prefilter2_query);
        }

        try {
            f1->Compile();
            f2->Compile();

            if (prefilter1) {
                prefilter1->Compile();
            }

            if (prefilter2) {
                prefilter2->Compile();
            }
        }
        catch (JQException& e) {
            ShowErrorDialog(e.what());
            return;
        }

        // Save all packets from topic1 in a scratchfile and only matching packets from topic2 in topic2 file
        // After stream is finished, filter only matching packets into topic1 file
        std::ofstream scratch(scratch_);
        std::ofstream file2(file2_);

        bool comparator_equal = gui_choice->GetStringSelection() == "==";
        bool comparator_in = gui_choice->GetStringSelection() == "in";

        //Consume is a blocking calland needs to be run on a separate thread.
        kafka::consume(brokers, { topic1, topic2 }, 0, is_streaming_, [&, prefilter1, prefilter2, topic1, topic2](std::shared_ptr<std::string> message, std::shared_ptr<std::string> topic) {
            
            // Execute on each packet
            total_count++;

            // process packet
            try {
                if (*topic == topic1) {
                    // topic 1 packet
                    topic1_count++;

                    if (prefilter1) {
                        message = prefilter1->Apply(message);

                        if (!jq::has_data(message)) {
                            return;
                        }
                        topic1_prefilter++;
                    }

                    auto key1 = f1->Apply(message);
                    if (jq::has_data(key1)) {
                        keys1_.insert(*key1);
                        scratch << *message << std::endl;
                    }
                }
                else {  
                    
                    //topic 2 packet
                    topic2_count++;

                    if (prefilter2) {
                        message = prefilter2->Apply(message);

                        if (!jq::has_data(message)) {
                            return;
                        }
                        topic2_prefilter++;
                    }

                    if (comparator_equal) {

                        auto key2 = f2->Apply(message);

                        if (jq::has_data(key2) && (keys1_.find(*key2) != keys1_.end())) {
                            // this is a match: save the key and the packet
                            keys2_.insert(*key2);
                            file2 << *message << std::endl;

                            if (max_matches != 0 && max_matches == keys2_.size()) {
                                // we've reached the max requested by the user. Stop the stream.
                                *is_streaming_ = false;
                            }
                        }
                    }
                    else if (comparator_in) {
                        for (auto& key : keys1_) {
                            if (util::is_substring(*message, key)) {
                                keys2_.insert(key);
                                file2 << *message << std::endl;
                                break;
                            }
                        }
                    }
                }

                
                if (total_count % 250 == 0) {
                    // Update GUI
                    // This will be run on a different thread therefore 
                    // we have to pass copy of data (not references) b/c they might go out of scope
                    auto keys2_size = keys2_.size();
                    auto g_console = gui_console;

                    scheduler_->CallLaterOnMainThread([topic1, topic1_count, prefilter1, topic1_prefilter, 
                        topic2, topic2_count, prefilter2, topic2_prefilter,
                        keys2_size, g_console]() {

                        std::filesystem::path p{ scratch_ };
                        auto size = std::filesystem::file_size(p);
                        size = size >> 20;

                        std::stringstream ss;
                        ss << topic1 << ": " << topic1_count << ", ";
                        if (prefilter1) {
                            ss << "with prefilter1: " << topic1_prefilter << ", ";
                        }
                        ss << topic2 << ": " << topic2_count << ", ";
                        if (prefilter2) {
                            ss << "with prefilter2: " << topic2_prefilter << ", ";
                        }
                        
                        ss << "cache: " << size << " MB, ";
                        ss << "matches found: " << std::to_string(keys2_size);
                        
                        auto str = ss.str();
                        g_console->SetLabelText(str);
                    });
                }
                
            }
            catch (...) {
                //logger_err << "ERROR: could not parse or filter packet. skipping...";
            }

        });
        *is_streaming_ = false;
        scratch.close();
        file2.close();


        scheduler_->CallLaterOnMainThread([this, total_count, match_count]() {
            ShowMessage("stream closed, processing matched traffic, please wait...");
        });
        
        // Process files: filter matching packets (keys) from scratchfile into topic1_cache file.
        std::ofstream file1(file1_);
        util::for_each_line_in_file(scratch_, [&](std::string line) {
            
            auto line_ptr = std::make_shared<std::string>(line);
            auto key = f1->Apply(line_ptr);

            if (jq::has_data(key)) {
                if (keys2_.find(*key) != keys2_.end()) {
                    // this key exists in file2 so it's a match. save it.
                    file1 << line << std::endl;
                }
            }
        });

        file1.close();

        // Delete contents of scratch file to save space
        util::clear_file_contents(scratch_);

        //One last update because we will miss the last packets(from last refresh rate until stream stopped)
        scheduler_->CallLaterOnMainThread([this, total_count, match_count]() {
            ShowMessage("loading...");
            LoadTopicSelection();

            // Update buttons
            gui_button_run->Enable();
            gui_button_run->SetLabelText("Run");
            guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
            ShowMessage("stream closed, matched " + std::to_string(keys2_.size()) + " requests.");
        });

    }); // std::thread
    t.detach();

}

void TrafficMatch::OnFilter(wxCommandEvent& event)
{
    if (gui_list_view->GetItemCount() > 0) {
        gui_list_view->DeleteAllItems();
    }

    filter_ = gui_filter->GetValue();
    logger_info << "Applying \"" <<  filter_ << "\", buffer size: " << buffer_.size();

    auto filter = jq::FilterAPI(filter_);

    try {
        filter.Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        return;
    }

    for (size_t i = 0; i < buffer_.size(); ++i) {

        try {
            auto result = filter.Apply(buffer_[i]);

            if (jq::has_data(result)) {
                wxVector<wxVariant> data;
                data.push_back(wxVariant(std::to_string(i)));
                data.push_back(*result);
                gui_list_view->AppendItem(data);
            }
        }
        catch (...) {
            logger_err << "could not parse or filter this request";
            continue;
        }
    }
    ConsoleShowStats();
}

void TrafficMatch::OnBrowse(wxCommandEvent& event)
{
    LoadTopicSelection();
    
    // When switching topics, clear the current filter.
    gui_filter->Clear();
    filter_.clear();

    // Update console message
    std::stringstream ss;
    ss << "Showing " << buffer_.size() << " ";
    if (gui_browse->GetSelection() == 0) {
        ss << gui_topic1->GetValue();
    }
    else {
        ss << gui_topic2->GetValue();
    }
    ss << " messages";
    ShowMessage(ss.str());
}

void TrafficMatch::OnCompareChoice(wxCommandEvent&)
{
    auto comparator = gui_choice->GetStringSelection();

    if (comparator == "==") {
        gui_filter2->Enable();
    }
    else if (comparator == "in") {
        gui_filter2->Disable();
    }
}

void TrafficMatch::OnKeyUpFilter(wxKeyEvent& event)
{
    auto key = event.GetKeyCode();
    if (key == WXK_RETURN) {
        wxCommandEvent e;
        OnFilter(e);
    }
}

void TrafficMatch::OnKeyUpTopic1(wxKeyEvent&)
{
    gui_browse->SetString(0, gui_topic1->GetValue());
}

void TrafficMatch::OnkeyUpTopic2(wxKeyEvent&)
{
    gui_browse->SetString(1, gui_topic2->GetValue());
}

void TrafficMatch::OnMatch(wxCommandEvent& event)
{
    if (gui_choice->GetStringSelection() == "in" && gui_browse->GetSelection() == 1) {
        // if we're trying to match from topic2 into topic1 when the "in" comparator was used.
        // we cannot match because we can't isolate the key from topic 2 so we can filter messages from topic1. 
        // only works the other way around.
        ShowMessage("When using string comparison ('in') you cannot match from topic 2 into topic 1");
        return;
    }

    auto row = gui_list_view->GetSelectedRow();

    // on macOS deleting items from listview is sending an OnSelect event with invalid row. verify.
    if (row == -1)
        return;

    try {

         // Extarct the key from the selected message
        wxVariant idx;
        gui_list_view->GetValue(idx, row, 0);
        auto value = buffer_[idx.GetLong()];

        std::string selection_filter;   // matching filter for current browsing topic
        std::string other_filter;       // the other filter for the other bworsing topic
        std::string filename;

        if (gui_browse->GetSelection() == 0) {
            //first topic selected, search for matches in the second topic
            selection_filter = gui_filter1->GetValue();

            filename = file2_;
            other_filter = gui_filter2->GetValue();
        }
        else {
            //second topic selected, search for matches in the first topic
            selection_filter = gui_filter2->GetValue();

            filename = file1_;
            other_filter = gui_filter1->GetValue();
        }

        auto f1 = jq::FilterAPI(selection_filter);
        
        try
        {
            f1.Compile();
        }
        catch (JQException& e)
        {
            ShowErrorDialog(e.what());
            return;
        }
        auto key1 = f1.Apply(value);
        
        if (jq::empty(key1)) {
            return;
        }

        // Show console message to the user to make it clear what we're doing
        std::stringstream message;
        if (gui_choice->GetStringSelection() == "in") {
            // topic1 is selected and comparator is 'in'
            message << "matching any message from " << gui_topic2->GetValue() << " that contains string: " << key1;
        }
        else if (gui_browse->GetSelection() == 0) {
            // topic 1 is selected and comparator is ==
            message << "selection is: " << key1 << ", matching this with " << gui_topic2->GetValue() << " with filter '" << gui_filter2->GetValue() << "'";
        }
        else if (gui_browse->GetSelection() == 1){
            // topic 2 is selected and comparator is ==
            message << "selection is: " << key1 << ", matching this with " << gui_topic1->GetValue() << " with filter '" << gui_filter1->GetValue() << "'";
        }
        auto msg = message.str();
        //ShowMessage(msg);

        auto f2 = jq::FilterAPI(other_filter);
        try {
            f2.Compile();
        }
        catch(JQException& e) {
            ShowErrorDialog(e.what());
            return;
        }
        std::vector<std::string> buf;

        bool comparator_in = gui_choice->GetStringSelection() == "in";
        // Use the first key to match with keys from the other traffic topic
        util::for_each_line_in_file(filename, [&](std::string line) {

            if (comparator_in) {
                //match from topic1 into topic2 (only case available for "in")
                if (util::is_substring(line, *key1)) {
                    auto packet = std::make_shared<std::string>(line);
                    buf.push_back(*packet);
                }
            }
            else {
                auto line_ptr = std::make_shared<std::string>(line);
                auto key2 = f2.Apply(line_ptr);
                if (jq::has_data(key2)) {

                    if (*key1 == *key2) {
                        // it's a match
                        auto packet = std::make_shared<std::string>(line);
                        buf.push_back(*packet);
                    }
                }
            }
        });

        // buf now contains the matching packets from the opposite topic (the one that's not being browsed atm).
        auto packetview = new PacketView(this);
        packetview->Show();
        packetview->SetData({"ID","MESSAGE"}, buf);
        
        std::stringstream window_title;
        window_title << "Showing ";
        if (gui_browse->GetSelection() == 0) {
            window_title << gui_topic2->GetValue();
        }
        else {
            window_title << gui_topic1->GetValue();
        }
        window_title << " packets";

        //packetview->SetTitle(window_title.str());
        packetview->SetTitle(msg);
    }
    catch (...) {
        logger_err << "Exception caught. todo: print exception (probably json parsing)";
    }
}

void TrafficMatch::OnSelect(wxDataViewEvent& event)
{
    auto row = gui_list_view->GetSelectedRow();

    // on macOS deleting items from listview is sending an OnSelect event with invalid row.
    if (row == -1)
        return;
    
    try {
        wxVariant idx;
        gui_list_view->GetValue(idx, row, 0);
        auto data = buffer_[idx.GetLong()];

        logger_trace << "Selected row: " << row << ", data: \n" << data;

        auto filter = jq::FilterAPI(filter_);
        try {
            filter.Compile();

        }
        catch (JQException& e) {
            ShowErrorDialog(e.what());
            return;
        }
        auto result = filter.Apply(data);

        if (jq::has_data(result)) {
            gui_text_view->SetText(*jq::pretty_json(result));
        }
    }
    catch (...) {
        logger_err << "Exception caught. todo: print exception (probably json parsing)";
    }
}

void TrafficMatch::Clear()
{

    if (gui_list_view->GetItemCount() != 0) {
        gui_list_view->DeleteAllItems();
    }

    gui_text_view->SetText("");

    keys1_.clear();
    keys2_.clear();

    util::clear_file_contents(file1_);
    util::clear_file_contents(file2_);
    util::clear_file_contents(scratch_);
}

void TrafficMatch::ShowMessage(std::string msg)
{
    gui_console->SetLabelText(msg);
}

void TrafficMatch::ShowErrorDialog(std::string msg)
{
    std::stringstream ss;
    ss << "There's a problem with that filter: ";
    ss << msg << "\n\n";
    wxString error(ss.str());
    wxMessageDialog dial(NULL,
        error, wxT("JQ Filter"), wxOK | wxICON_ERROR);
    dial.ShowModal();
}

void TrafficMatch::ConsoleShowStats()
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
    gui_console->SetLabel(ss.str());
}

void TrafficMatch::LoadTopicSelection()
{
    // Clear 
    buffer_.clear();
    gui_list_view->DeleteAllItems();

    // Get user selection for browsing
    int selection = gui_browse->GetSelection();

    // Verify which file to be selected
    std::string filename;
    if (selection == 0) {
        filename = file1_;
    }
    else {
        filename = file2_;
    }

    // Load file into buffer
    unsigned long count = 0;
    util::for_each_line_in_file(filename, [this, &count](std::string line) {

        auto packet = std::make_shared<std::string>(line);
        buffer_.push(packet);

        // update UI
        auto id = std::to_string(count);

        //dataViewList
        wxVector<wxVariant> data;
        data.push_back(wxVariant(id));
        data.push_back(wxVariant(*packet));
        gui_list_view->AppendItem(data);

        ++count;
    });
}

void TrafficMatch::OnMarginClick(wxStyledTextEvent& event)
{
    guitools::fold_styledtextctrl(event, gui_text_view);
}

void TrafficMatch::OnActivated(wxDataViewEvent& event)
{
    wxCommandEvent e;
    OnMatch(e);
}

const string TrafficMatch::help = R"#(To begin press Run
Default settings: 
- will consume messages from "topic1" and "topic2" 
- for every message, verify .id field and see if there is a match with the second topic. If a match, keep message in a buffer.

Topic Prefilter: will look for auctions only after running this filter.
Example: select(.country = "US") will only keep messages from US and discard other messages.

Select topic1/topic2: select which messages to show, either topic1 or topic2

Double click on any result (or select and click find match) to see the corresponding matched message from the opposite topic.
)#";