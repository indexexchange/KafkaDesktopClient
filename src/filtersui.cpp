#include <wx/wx.h>

#include "filtersui.h"
#include "logger.h"
#include "settings.h"
#include "util.h"

const int kName = 0;
const int kDescription = 1;
const int kTopic = 2;
const int kFilter = 3;

FiltersUI::FiltersUI(wxWindow* parent) : FilterFrame(parent)
{
    // fill up the grid
    auto grid_data = settings::filters::get_filters();
    gui_filters_grid->AppendRows(grid_data.size(), true);

    auto total_rows = grid_data.size();
    for (size_t i = 0; i < total_rows; ++i) {
        for (size_t j = 0; j < grid_data[i].size(); ++j) {
            gui_filters_grid->SetCellValue(i, j, grid_data[i][j]);
        }
    }
}

FiltersUI::FiltersUI(wxWindow* parent, std::function<void(std::string, std::string)> handler) : FiltersUI(parent)
{
    selection_handler_ = handler;
}

void FiltersUI::OnClose(wxCloseEvent& event)
{
    SaveGridData();
    event.Skip();
}

void FiltersUI::OnTextFiltersSearch(wxCommandEvent& event)
{
    std::string query = gui_filters_search->GetValue();

    // treat only queries larger than 3
    if (query.size() < 3) {
        for (int i = 0; i < gui_filters_grid->GetNumberRows(); ++i) {
            gui_filters_grid->ShowRow(i);
        }
        return;
    }

    for (int i = 0; i < gui_filters_grid->GetNumberRows(); ++i) {
        bool found = false;
        
        for (int j = 0; j < gui_filters_grid->GetNumberCols(); ++j) {
            std::string cell_value = gui_filters_grid->GetCellValue(i, j);

            if (util::is_substring(cell_value, query)) {
                found = true;
                continue;
            }
            if (found)
                continue;
        }

        if (!found) {
            gui_filters_grid->HideRow(i);
        }
        else {
            gui_filters_grid->ShowRow(i);
        }
        
    }

}

void FiltersUI::OnClickFiltersApply(wxCommandEvent& event)
{
    if (selection_handler_ != nullptr && row_ != -1) {

        std::string topic = gui_filters_grid->GetCellValue(row_, kTopic);
        std::string filter = gui_filters_grid->GetCellValue(row_, kFilter);

        selection_handler_(topic, filter);

        Close(true);
    }
}

void FiltersUI::OnClickFiltersClose(wxCommandEvent& event)
{
    SaveGridData();
    Close(true);
}

void FiltersUI::OnClickFiltersAdd(wxCommandEvent& event)
{
    gui_filters_grid->AppendRows(1, true);
}

void FiltersUI::OnClickFiltersDelete(wxCommandEvent& event)
{
    gui_filters_grid->DeleteRows(row_);
    row_ = -1;
    col_ = -1;
}

void FiltersUI::OnSelectFiltersCell(wxGridEvent& event)
{
    row_ = event.GetRow();
    col_ = event.GetCol();
}

void FiltersUI::OnClickFiltersImport(wxCommandEvent& event)
{
    wxFileDialog* LoadDialog = new wxFileDialog(
        this, _("Import Filters From _?"), wxEmptyString, wxEmptyString,
        _("Json file (*.j)|*.j"),wxFD_OPEN, wxDefaultPosition);

    if (LoadDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
    {
        auto file = LoadDialog->GetPath();

        logger_info << "Importing filters from: " << file;

        try {
            auto data = settings::filters::get_filters_from_file(file);
            auto new_items_count = data.size();
            auto old_items_count = gui_filters_grid->GetNumberRows();
            
            // allocate new rows
            gui_filters_grid->AppendRows(data.size(), true);

            // Insert new items in table, starting from the last item in the table
            for (size_t i = 0; i < new_items_count; ++i) {
                for (size_t j = 0; j < data[i].size(); ++j) {
                    gui_filters_grid->SetCellValue(i + old_items_count, j, data[i][j]);
                }
            }
        }
        catch (...) {
            logger_err << "Unable to import file.";
        }
    }

    // Clean up after ourselves
    LoadDialog->Destroy();

}

void FiltersUI::OnClickFiltersExport(wxCommandEvent& event)
{
    // save all modifications first
    SaveGridData();

    wxFileDialog* SaveDialog = new wxFileDialog(
        this, _("Export Filters To _?"), wxEmptyString, wxEmptyString,
        _("Json file (*.j)|*.j"),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

    if (SaveDialog->ShowModal() == wxID_OK) // If the user clicked "OK"
    {
        auto file = SaveDialog->GetPath();
        
        logger_info << "Exporting filters to file: " << file;

        try {
            SaveGridDataTo(file);
        }
        catch(...){
            logger_info << "Unable to export file.";
        }
    }

    // Clean up after ourselves
    SaveDialog->Destroy();
}

void FiltersUI::SaveGridData()
{
    std::vector<std::vector<std::string>> grid_data;

    for (int i = 0; i < gui_filters_grid->GetNumberRows(); ++i) {
        std::vector<std::string> row;

        for (int j = 0; j < gui_filters_grid->GetNumberCols(); ++j) {
            row.push_back(gui_filters_grid->GetCellValue(i, j));
        }
        grid_data.push_back(row);
    }

    // Save settings file
    settings::filters::Save(grid_data);
}

void FiltersUI::SaveGridDataTo(std::string path)
{
    std::vector<std::vector<std::string>> grid_data;

    for (int i = 0; i < gui_filters_grid->GetNumberRows(); ++i) {
        std::vector<std::string> row;

        for (int j = 0; j < gui_filters_grid->GetNumberCols(); ++j) {
            row.push_back(gui_filters_grid->GetCellValue(i, j));
        }
        grid_data.push_back(row);
    }

    // Save settings file
    settings::filters::Save(grid_data, path);
}
