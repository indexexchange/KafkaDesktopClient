//#define _SIM_PKTS_
#include "app.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <regex>

#include <wx/dialog.h>
#include <nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <xlnt/xlnt.hpp>

#include "filtersui.h"
#include "logger.h"
#include "settings.h"
#include "util.h"

#include "custommsg.h"
#include "inspector.h"
#include "statistics.h"
#include "trafficmatch.h"
#include "util.h"

using json = nlohmann::json;

// Initialize GUI
App::App(wxWindow* parent) : UIFrame(parent)
{
    inspector_panel_ = new Inspector(auinotebook);
    auinotebook->AddPage(inspector_panel_, wxT("Inspector"), false, wxNullBitmap);

    auto stats_panel = new Statistics(auinotebook);
    auinotebook->AddPage(stats_panel, wxT("Statistics"), false, wxNullBitmap);

    traffic_match_panel_ = new TrafficMatch(auinotebook);
    auinotebook->AddPage(traffic_match_panel_, wxT("TrafficMatch"), false, wxNullBitmap);

    // Need to start on separate thread otherwise it blocks when it can't connect
    // Check for updates
    std::thread t([this]() {
        try {
            // RAII cleanup
            
            curlpp::Cleanup myCleanup;

            std::ostringstream os;
            os << curlpp::options::Url("http://update_url/versions.json");

            auto j = nlohmann::json::parse(os.str());

            auto msg = j[VERSION];

            if (!msg.empty()) {
                std::string message = msg.dump();
                logger_info << message;
                // new version detected, callback on main thread
                wxTheApp->GetTopWindow()->GetEventHandler()->CallAfter([this, message]() {
                    auto update = new CustomMessageWindow(this);
                    update->AddHeader("New version available");
                    update->AddParagraph(message);
                    update->Show();
                });
            }
        }
        catch (curlpp::RuntimeError& e)
        {
            logger_debug << e.what();
        }

        catch (curlpp::LogicError& e)
        {
            logger_debug << e.what();
        }
        catch (...) {
        }
    });
    t.detach();
}

void App::ShowFiltersMenu()
{
    auto ui_filters = new FiltersUI(this, [this](std::string topic, std::string filter) {
        inspector_panel_->SetTopic(topic);
        inspector_panel_->SetFilter(filter);
    });
    ui_filters->Show();
}

void App::OnSelectMenuFiltersView(wxCommandEvent& event)
{
    // Init Filters ui frame
    // Pass callback to be executed if user clicks apply
    ShowFiltersMenu();
}

void App::OnSelectFiltersSave(wxCommandEvent& event)
{
    std::string topic = inspector_panel_->GetTopic();
    std::string filter = inspector_panel_->GetFilter();
    settings::filters::Add(topic, filter);

    ShowFiltersMenu();
}


void App::OnAbout(wxCommandEvent& event)
{
    auto about = new CustomMessageWindow(this);

    about->AddHeader("Description");

    std::stringstream ss;
    ss << R"##(The Kafka Desktop Client is a cross-platform desktop application that consumes, analyzes, and inspects Kafka messages. Using the flexible set of tools, you can quickly and easily browse and filter messages, gather statistics, and scan and match traffic from different topics.\n\n)##";
    about->AddParagraph(ss.str());
    about->NewLine();

    about->AddHeader("License");
    about->AddParagraph("Internal use at Index Exchange.");

    about->Show();
}

void App::OnClose(wxCloseEvent& event)
{
    // Do not call Close() from here, it will cause a stack overflow. Just skip and it will be caused down the stack.
    event.Skip();
}


void App::OnSelectMenuExport(wxCommandEvent& event)
{
    wxFileDialog
        saveFileDialog(this, _("Save XYZ file"), "", "requests",
            "Excel Workbook (*.xlsx)|*.xlsx|Plain text, pretty printed (*.txt)|*.txt|Plain text, raw (*.txt)|*.txt", 
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    std::string path = saveFileDialog.GetPath().ToStdString();
    logger_info << "Export to: " << path;

    std::vector<std::shared_ptr<std::string>> results;

    auto selected_tab = auinotebook->GetSelection();    // 0 = inspector, 1 = stats, 2 = traffic match

    // Get results from open panel 
    if (selected_tab == 0) {
        // Inspector
        results = inspector_panel_->GetDisplayResults();        
    }
    else if (selected_tab == 2) {
        // Traffic Match
        results = traffic_match_panel_->GetDisplayResults();
    }

    if (results.empty()) {
        return;
    }

    int index = saveFileDialog.GetFilterIndex();

    if (index == 0) {
        // Excel workbook selected
        // First pass: Create a set with sorted keys then dump into a vector
        std::set<std::string> keys_set;
        for (size_t i = 0; i < results.size(); ++i) {
            try {
                auto flat_json = json::parse(*results[i]).flatten();
                for (auto it = flat_json.begin(); it != flat_json.end(); ++it) {
                    keys_set.insert(it.key());
                }
            }
            catch (...){
                // parsing error
                continue;
            }
        }

        // Place all keys in a vector
        std::vector<std::string> keys_vec(keys_set.begin(), keys_set.end());

        // Second pass: populate values in xlsx
        xlnt::workbook wb;
        xlnt::worksheet ws = wb.active_sheet();

        // Write rows in forst column
        int column = 1;
        for (auto& key : keys_vec) {
            ws.cell(column, 1).value(key);
            ++column;
        }

        for (size_t i = 0; i < results.size(); ++i) {

            try {
                auto flat_json = json::parse(*results[i]).flatten();

                for (size_t j = 0; j < keys_vec.size(); ++j) {
                    if (!flat_json[keys_vec[j]].is_null()) {
                        // Write results to excel sheet
                        ws.cell(1 + j, 2 + i).value(flat_json[keys_vec[j]].dump());
                    }
                }
            }
            catch (...) {
                //parsing error
                ws.cell(1 , i + 2).value("error: can't parse json");
                continue;
            }
            
        }

        ws.freeze_panes("A2");
        ws.title("PROCESSED REQUESTS");

        // adding second sheet with raw requests
        xlnt::worksheet ws2 = wb.create_sheet();
        ws2.cell(1, 1).value("Raw Requests");
        for(size_t i = 0; i < results.size(); ++i) {
            ws2.cell(1, i+2).value(results[i]->c_str());
        }

        ws2.freeze_panes("A2");
        ws2.title("RAW REQUESTS");

        try {
            wb.save(path);
        }
        catch (...) {
            wxMessageDialog dial(NULL,
                wxT("Could not save to that location. The file is already open or you don't have permission to write to that location."), wxT("Info"), wxICON_ERROR);
            dial.ShowModal();
        }
    }
    else if (index == 1) {
        // text format - json pretty printed

        try {
            std::ofstream out(path);

            for (auto& result : results) {
                auto j = json::parse(result->c_str());
                out << j.dump(4) << std::endl;
            }
        }
        catch (...) {

        }
    }
    else if (index == 2) {
        // text format - json raw

        try {
            std::ofstream out(path);

            for (auto& result : results) {
                out << result->c_str() << std::endl;
            }
        }
        catch (...) {

        }
    }

    
    
}

void App::OnSelectMenuImport(wxCommandEvent& event)
{
    wxFileDialog
        OpenFileDialog(this, _("Import requests from txt file"), "", "requests",
            "Plain text, raw (*.*)|*.*",
            wxFD_OPEN);

    if (OpenFileDialog.ShowModal() == wxID_CANCEL)
        return;     // the user changed idea...

    std::string path = OpenFileDialog.GetPath().ToStdString();
    logger_info << "Import: " << path;

    auto selected_tab = auinotebook->GetSelection();    // 0 = inspector, 1 = stats, 2 = traffic match

    // Only Inspector supports importing for now (although it'll work with traffic match too)
    if (selected_tab != 0) {
        return;
    }

    // Load in buffer
    util::for_each_line_in_file(path, [this](std::string line) {

        auto line_ptr = std::make_shared<std::string>(line);
        if (util::is_valid_json(line_ptr)) {
            inspector_panel_->AddToBuffer(line_ptr);
        }
        else {
            // if the string is not valid json, try to convert it to a json array. 
            line_ptr = util::try_json_array(line_ptr);
            inspector_panel_->AddToBuffer(line_ptr);
        }
    });

    inspector_panel_->ForceRefresh();
}

void App::OnSelectMenuQuit(wxCommandEvent& event)
{
    // Stop timer otherwise it will crash the app next time it's invoked
    Close(true);
}