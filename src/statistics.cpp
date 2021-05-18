#include "statistics.h"

#include <memory>
#include <thread>

#include "jq/filter.h"
#include "kafka.h"
#include "logger.h"
#include "scheduler.h"
#include "guitools.h"

Statistics::Statistics(wxWindow* window) : StatisticsPanel(window)
{
    // Apply graphics to buttons
    guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);

    // Scheduler for callign events on main thread
    scheduler_ = std::make_shared<Scheduler>(151);

    // Kafka flg 
	is_streaming_ = std::make_shared<bool>(false);
}

Statistics::~Statistics()
{
}

void Statistics::OnRun(wxCommandEvent& event)
{
    if (*is_streaming_) {
        // user cancelled a statistics run
        gui_button_run->SetLabelText("Run");
        guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
        *is_streaming_ = false;
        return;
    }

    // Start a brand new run
    // Update flags
    *is_streaming_ = true;

    // Update buttons (Apply and Run)
    gui_button_run->SetLabelText("Stop");
    guitools::apply_graphic(gui_button_run, guitools::PNG::STOP);

    // Gather user input
    int max = gui_buffer_size->GetValue();
    std::string topic = gui_topic->GetValue();
    std::string brokers = gui_brokers->GetValue();
    std::string pf = gui_filter->GetValue();

    auto prefilter = jq::FilterPtr(pf);
    try {
        prefilter->Compile();
    }
    catch (JQException& e) {
        ShowErrorDialog(e.what());
        return;
    }

    // Collect user provided filters
    size_t total_rows = gui_grid->GetNumberRows();
    std::vector<std::shared_ptr<jq::FilterAPI>> filters;

    for (size_t i = 0; i < total_rows; ++i) {
        auto filter_query = gui_grid->GetCellValue(i, 0);
        if (!filter_query.empty()) {
            auto f = jq::FilterPtr(filter_query);
            try {
                f->Compile();
            }
            catch (JQException& e){
                ShowErrorDialog(e.what());
                return;
            }
            filters.push_back(f);
            gui_grid->SetCellValue(i, 1, "0");
            gui_grid->SetCellValue(i, 2, "0");
        }
    }

    ConsoleShowMessage("please wait... consuming messages from " + topic);

    // Start streaming thread for consuming packets
    auto t = std::thread([this, max, topic, brokers, filters, prefilter]() {

        size_t total_count = 0;
        std::vector<int> hit_count(filters.size());
        auto last_refresh = std::chrono::high_resolution_clock::now();

        //Consume is a blocking calland needs to be run on a separate thread.
        kafka::consume(brokers, { topic }, 0, is_streaming_, [this, &total_count, &hit_count, filters, prefilter, max, &last_refresh](std::shared_ptr<std::string> packet, std::shared_ptr<std::string>) {

            // On each received packet
            
            
            // process packet
            try {

                auto prefilter_result = prefilter->Apply(packet);
                if (jq::empty(prefilter_result)) {
                    return;
                }

                // Count request only if it passes pre-filter
                total_count++;

                for (size_t i = 0; i < filters.size(); ++i) {
                    auto result = filters[i]->Apply(packet);

                    if (jq::has_data(result)) {
                        hit_count[i]++;
                    }
                }

                auto now = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_refresh).count();
                // Update UI only at certain intervals
                if (duration > 75) {
                    scheduler_->CallLaterOnMainThread([this, total_count, hit_count]() {
                        for (size_t i = 0; i < hit_count.size(); ++i) {
                            gui_grid->SetCellValue(i, 1, std::to_string(hit_count[i]));
                            gui_grid->SetCellValue(i, 2, std::to_string(float(hit_count[i]) / float(total_count) * 100.0));
                            gui_console->SetLabelText("Consumed: " + std::to_string(total_count));
                        }
                    });

                    last_refresh = std::chrono::high_resolution_clock::now();
                }
            }
            catch (...) {
                logger_err << "ERROR: could not parse or filter packet. skipping...";
            }

            if (max == total_count) {
                *is_streaming_ = false;
            }

        });
        *is_streaming_ = false;

        //One last update because we will miss the last packets(from last refresh rate until stream stopped)
        //    todo: function is repeated, needs refactoring
        scheduler_->CallLaterOnMainThread([this, total_count, hit_count]() {
            for (size_t i = 0; i < hit_count.size(); ++i) {
                gui_grid->SetCellValue(i, 1, std::to_string(hit_count[i]));
                gui_grid->SetCellValue(i, 2, std::to_string(float(hit_count[i]) / float(total_count) * 100.0));
                gui_console->SetLabelText("Consumed: " + std::to_string(total_count));
            }
        });

        // On finish - re-enable all buttons
        scheduler_->CallLaterOnMainThread([this, total_count, topic]() {
            // Update buttons
            gui_button_run->Enable();
            gui_button_run->SetLabelText("Run");
            guitools::apply_graphic(gui_button_run, guitools::PNG::PLAY);
            ConsoleShowMessage("stream closed, consumed " + std::to_string(total_count) + " messages from " + topic);
        });

    });
    t.detach();
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

