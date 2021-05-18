#include "custommsg.h"

#include <sstream>

#include "app.h"

#include "ui/logo4_64.xpm"
#include "ui/logo4.xpm"

CustomMessageWindow::CustomMessageWindow(wxWindow* parent) : CustomMessageFrame(parent)
{
	wxIcon mainicon;
	wxBitmap bitmap(logo4_png);
	mainicon.CopyFromBitmap(bitmap);
	SetIcon(mainicon);

	gui_logo->SetBitmap(logo4_64_png);

	// Set version and build 
	gui_version->SetLabelText(VERSION);
	std::stringstream ss;
	ss << "Build time: ";
	ss << BUILD_DATE;
	ss << " - ";
	ss << BUILD_TIME;
	gui_build->SetLabelText(ss.str());
}

void CustomMessageWindow::AddHeader(std::string title)
{
	gui_text->BeginBold();
	gui_text->WriteText(title);
	gui_text->EndBold();
	gui_text->Newline();
}

void CustomMessageWindow::AddParagraph(std::string paragraph)
{
	gui_text->WriteText(paragraph);
	gui_text->Newline();
}

void CustomMessageWindow::NewLine()
{
	gui_text->Newline();
}

void CustomMessageWindow::OnOk(wxCommandEvent& event)
{
	Close(true);
}
