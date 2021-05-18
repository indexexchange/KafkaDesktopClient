#ifndef UI_H_
#define UI_H_

// this needs to be defined on windows otherwise it will clash with rdkafka lib
#ifdef _WIN32
#define _SSIZE_T  // on MAC does not compile with this on
#endif

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

// WxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "ui/noname.h"

#include "inspector.h"
#include "trafficmatch.h"

const std::string VERSION = "KafkaDesktopClient 2.3";
const std::string BUILD_TIME = __TIME__;
const std::string BUILD_DATE = __DATE__; 

// Main UI class
class App : public UIFrame
{   
public:
	App(wxWindow* parent);
	~App() {}

private:
    Inspector* inspector_panel_;
    TrafficMatch* traffic_match_panel_;
    void ShowFiltersMenu();

private:

    // Event handlers
    virtual void OnAbout(wxCommandEvent& event);
    virtual void OnClose(wxCloseEvent& event);
    virtual void OnSelectMenuExport(wxCommandEvent& event);
    virtual void OnSelectMenuImport(wxCommandEvent& event);
    virtual void OnSelectMenuQuit(wxCommandEvent& event);
    virtual void OnSelectFiltersSave(wxCommandEvent& event);
    virtual void OnSelectMenuFiltersView(wxCommandEvent& event);

    // Closing & Cleanup
    void OnClose();

};

#endif  // UI_H_