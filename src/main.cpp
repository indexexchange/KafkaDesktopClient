#include "app.h"
#include "logger.h"

#include <iostream>

#include "ui/logo4.xpm"

class MainApp : public wxApp
{
public:
	// test git
	MainApp() {}
	~MainApp() {}

public:
	bool OnInit();

private:
	App* ui_frame = nullptr;
};

wxIMPLEMENT_APP(MainApp);
//wxIMPLEMENT_APP_CONSOLE(MainApp);		// VS2019: if switching to console app, also change under Linker->System->Subsystem

/*
TODO:
	- add an auto updater
*/
bool MainApp::OnInit()
{
	//logger::SetLevelTrace();
	logger::SetLevelDebug();
	logger::WriteToFile("log.txt");

	ui_frame = new App(nullptr);
	ui_frame->SetTitle(VERSION);
	ui_frame->Show();
	wxIcon mainicon;

	wxBitmap bitmap(logo4_png);
	mainicon.CopyFromBitmap(bitmap);
		
	ui_frame->SetIcon(mainicon);
	return true;
}
