#ifndef FILTERS_UI_H_
#define FILTERS_UI_H_

#include <functional>
#include <iostream>

#include "ui/noname.h"

// GUI window dealing with filters
class FiltersUI : public FilterFrame
{
public:
	FiltersUI(wxWindow* parent);
	FiltersUI(wxWindow* parent, std::function<void(std::string, std::string)>);
	~FiltersUI() { }

	void OnClose(wxCloseEvent& evt);
	void OnTextFiltersSearch(wxCommandEvent& event);
	void OnClickFiltersApply(wxCommandEvent& event);
	void OnClickFiltersClose(wxCommandEvent& event);
	virtual void OnClickFiltersAdd(wxCommandEvent& event);
	virtual void OnClickFiltersDelete(wxCommandEvent& event);
	void OnSelectFiltersCell(wxGridEvent& event);
	void OnClickFiltersImport(wxCommandEvent& event);
	void OnClickFiltersExport(wxCommandEvent& event);

private:

	//
	void SaveGridData();
	void SaveGridDataTo(std::string);

	// Grid selection
	int row_ = -1;
	int col_ = -1;

	// callback to handle user selection
	std::function<void(std::string, std::string)> selection_handler_;
};

#endif // FILTERS_UI_H_