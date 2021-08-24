#include "statistics.h"

#include <memory>
#include <thread>

#include "jq/filter.h"
#include "kafka.h"
#include "logger.h"
#include "scheduler.h"
#include "guitools.h"
#include "settings.h"

#define AVERAGE "AVERAGE"
#define COUNT "COUNT"
#define SUM "SUM"

#define COLUMN_FILTER   0
#define COLUMN_OP       1
#define COLUMN_RESULT   2
#define COLUMN_PCTTOTAL 3

/*
TODO:
    - if we leave a row blank in the grid, it doesn't display results at the right place
*/

Statistics::Statistics(wxWindow* window) : StatisticsPanel(window)
{
    // Scheduler for callign events on main thread
    scheduler_ = std::make_shared<Scheduler>(151);

    // Kafka flg 
	is_streaming_ = std::make_shared<bool>(false);

    // Set operation column
    wxString choices[3] = { AVERAGE,COUNT,SUM }; // Make table strings
    for (auto row = 0; row < gui_grid->GetNumberRows(); ++row) {
        gui_grid->SetCellEditor(row, 1, new wxGridCellChoiceEditor(3, choices, false)); // Add this to Cell
        gui_grid->EnableCellEditControl(true);
    }
    
    // Set default Values
    // Row1 defaults
    gui_grid->SetCellValue(0, COLUMN_FILTER, "select(.error == 123)");
    gui_grid->SetCellValue(0, COLUMN_OP, COUNT); 

    // Row1 defaults
    gui_grid->SetCellValue(1, COLUMN_FILTER, ".data.item.price");
    gui_grid->SetCellValue(1, COLUMN_OP, AVERAGE); // Set default value
    
    // Row1 defaults
    gui_grid->SetCellValue(2, COLUMN_FILTER, ".data.profit");
    gui_grid->SetCellValue(2, COLUMN_OP, SUM); // Set default value

    gui_grid->SetGridCursor(0, 1);
    gui_grid->ShowCellEditControl();

    SetGUIState(State::Idle);
}

Statistics::~Statistics()
{
}

void Statistics::OnRun(wxCommandEvent& event)
{
    SetGUIState(State::Connecting);    

    // Gather user input
    int max = gui_buffer_size->GetValue();
    std::string topic = gui_topic->GetValue().ToStdString();
    std::string brokers = gui_brokers->GetValue().ToStdString();
    std::string pf = gui_filter->GetValue().ToStdString();

    auto prefilter = jq::FilterPtr(pf);
    try {
        prefilter->Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        SetGUIState(State::Idle);
        return;
    }

    // Collect user provided filters from GUI and store them in table_
    GetGUIValues();

    // Create JQ filters
    size_t total_rows = table_.size();
    std::vector<std::shared_ptr<jq::FilterAPI>> filters(total_rows);

    for (size_t row = 0; row < total_rows; ++row) {
        auto filter_query = GetTableRowFilter(row);
        if (!filter_query.empty()) {
            auto f = jq::FilterPtr(filter_query);
            try {
                f->Compile();
            }
            catch (JQException& e){
                ShowErrorDialog(e.what());
                SetGUIState(State::Idle);
                return;
            }
            filters[row] = f;
        }
    }

    ConsoleShowMessage("please wait... consuming messages from " + topic);

    // Update streaming flag and start streaming
    *is_streaming_ = true;

    // Start streaming thread for consuming packets
    auto t = std::thread([this, max, topic, brokers, filters, prefilter]() {

        size_t total_count = 0;
        size_t prefilter_count = 0;
        std::vector<int> hit_count(filters.size());
        auto last_refresh = std::chrono::high_resolution_clock::now();

        //Consume is a blocking call and needs to be run on a separate thread.
        kafka::consume(brokers, { topic }, 0, is_streaming_, [this, &prefilter_count, &total_count, &hit_count, filters, prefilter, max, &last_refresh](std::shared_ptr<std::string> packet, std::shared_ptr<std::string>) {

            if (prefilter_count == 0) {
                // We're now connected to brokers, update gui that we're now consuming data
                scheduler_->CallLaterOnMainThread([this]() {
                    SetGUIState(State::Running);
                });
            }

            // On each received packet this function executes
            total_count++;

            // Update UI only at certain refresh rate
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refresh).count();
            if (duration > 100) {
                RefreshGUI(total_count, prefilter_count);
                last_refresh = std::chrono::high_resolution_clock::now();
            }

            try {

                auto prefilter_result = prefilter->Apply(packet);
                if (jq::empty(prefilter_result)) {
                    return;
                }

                // At this point, we have passed prefilter
                // Count request towards total
                prefilter_count++;

                // Apply all row filters
                for (size_t row = 0; row < filters.size(); ++row) {
                    auto result = filters[row]->Apply(packet);

                    if (jq::has_data(result)) {
                        hit_count[row]++;
                    }
                    else {
                        continue;
                    }

                    // Set percent hits - common to all operations
                    SetTableRowPercent(row, (double)hit_count[row] / (double)prefilter_count * 100.0);

                    // Process each row operation
                    std::string op = gui_grid->GetCellValue(row,1).ToStdString();

                    if (op == COUNT) {
                        SetTableRowResult(row, hit_count[row]);
                    }
                    else if (op == AVERAGE) {
                        double data = std::stod(*result);
                        auto value = GetTableRowResult(row);

                        // Compute the Cumulative Moving Average (CMA)
                        // new value = old value + (new data point - old value) / total data points;
                        value = value + (data - value) / hit_count[row];
                        SetTableRowResult(row, value);
                    }
                    else if (op == SUM) {
                        double data = std::stod(*result);
                        double value = GetTableRowResult(row);
                        value += data;
                        SetTableRowResult(row, value);
                    }
                    else {
                        // should never reach this point
                    }
                }
            }
            catch (...) {
                logger_err << "ERROR: could not parse or filter packet. skipping...";
            }

            if (max == prefilter_count) {
                *is_streaming_ = false;
            }

        });
        *is_streaming_ = false;

        //One last update because we will miss the last packets(from last refresh rate until stream stopped)
        RefreshGUI(total_count, prefilter_count);

        // On finish - re-enable all buttons
        scheduler_->CallLaterOnMainThread([this, prefilter_count, topic]() {
            // Update buttons
            SetGUIState(State::Idle);
            ConsoleShowMessage("stream closed, consumed " + std::to_string(prefilter_count) + " messages from " + topic);
        });

    });
    t.detach();
}

void Statistics::OnStop(wxCommandEvent& event)
{
    SetGUIState(State::Idle);
    *is_streaming_ = false;
}

void Statistics::OnDropdown(wxCommandEvent& event)
{
    SyncCache();
}

void Statistics::SetGUIState(State s)
{
    if (s == state_) {
        return;
    }
        
    state_ = s;

    switch (state_)
    {
    case Statistics::State::Idle:
        // Apply graphics to buttons
        guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
        gui_button_run->SetLabelText("Run");

        gui_button_run->Enable();
        gui_filter->Enable();

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Statistics::OnRun), NULL, this);

        break;
    case Statistics::State::Connecting:
        // Update buttons (Apply and Run)
        gui_button_run->SetLabelText("Stop");
        guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);
        gui_filter->Disable();

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Statistics::OnStop), NULL, this);

        break;
    case Statistics::State::Running:
        // Update buttons (Apply and Run)
        gui_button_run->SetLabelText("Stop");
        guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);
        gui_button_run->Enable();

        gui_button_run->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Statistics::OnStop), NULL, this);
        break;
    default:
        break;
    }
}

// Update GUI grid with result and percent total column.
// No need to update filter column
void Statistics::RefreshGUI(unsigned long total, unsigned long prefilterMatches)
{
    scheduler_->CallLaterOnMainThread([this, total, prefilterMatches]() {

        std::stringstream ss;
        ss << "Total traffic: " << total << ", ";
        ss << "prefilter hits: " << prefilterMatches;
        gui_console->SetLabelText(ss.str());

        for (auto row = 0; row < table_.size(); ++row) {
            auto result = std::get<COLUMN_RESULT>(table_[row]);
            auto percent = std::get<COLUMN_PCTTOTAL>(table_[row]);
            auto operation = std::get<COLUMN_OP>(table_[row]);

            if (operation == COUNT) {
                gui_grid->SetCellValue(row, COLUMN_RESULT, std::to_string((unsigned long)result));
            }
            else {
                gui_grid->SetCellValue(row, COLUMN_RESULT, std::to_string(result));
            }
            gui_grid->SetCellValue(row, COLUMN_PCTTOTAL, std::to_string(percent));
        }
    });
}

// Get GUI table values and populate internal table
void Statistics::GetGUIValues()
{
    table_.clear();

    for (auto row=0; row < gui_grid->GetNumberRows(); ++row) {
        std::string filter = gui_grid->GetCellValue(row, COLUMN_FILTER).ToStdString();
        std::string op = gui_grid->GetCellValue(row, COLUMN_OP).ToStdString();

        if (!filter.empty()) {
            auto tup = std::make_tuple(filter, op, 0.0, 0.0);
            table_.push_back(tup);
        }
    }
}

void Statistics::ConsoleShowMessage(std::string msg)
{
    logger_info << msg;
}

void Statistics::ShowErrorDialog(std::string msg)
{
    std::stringstream ss;
    ss << "There's a problem with that filter: ";
    ss << msg << "\n\n";
    wxString error(ss.str());
    wxMessageDialog dial(NULL,
        error, wxT("JQ Filter"), wxOK | wxICON_ERROR);
    dial.ShowModal();
}

void Statistics::SyncCache()
{
    // update filter and topic drop downs with latest data
    auto filters = settings::cache::get_filters();

    std::string current_value = gui_filter->GetValue().ToStdString();
    gui_filter->Clear();

    for (auto& f : filters) {
        gui_filter->Append(f);
    }

    gui_filter->SetValue(current_value);
}

inline void Statistics::SetTableRowResult(int row, double value)
{
    double& ref = std::get<COLUMN_RESULT>(table_[row]);
    ref = value;
}

inline void Statistics::SetTableRowPercent(int row, double value)
{
    double& ref = std::get<COLUMN_PCTTOTAL>(table_[row]);
    ref = value;
}

inline double Statistics::GetTableRowResult(int row)
{
    return std::get<COLUMN_RESULT>(table_[row]);
}

inline std::string Statistics::GetTableRowFilter(int row)
{
    return std::get<COLUMN_FILTER>(table_[row]);
}

inline std::string Statistics::GetTableRowOperation(int row)
{
    auto result = std::get<COLUMN_OP>(table_[row]);
    return result;
}
