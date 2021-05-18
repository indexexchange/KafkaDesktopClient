#ifndef ABOUT_H_
#define ABOUT_H_

#include <string>
#include "ui/noname.h"


class CustomMessageWindow : public CustomMessageFrame
{
public:
	CustomMessageWindow(wxWindow* parent);
	void AddHeader(std::string);
	void AddParagraph(std::string);
	void NewLine();
	~CustomMessageWindow() { }
private:
	virtual void OnOk(wxCommandEvent& event);
};

#endif // ABOUT_H_