#include "packetviewui.h"

#include <string>
#include <vector>

#include "wx/wx.h"

#include "jq/filter.h"

using std::vector;
using std::string;

// NOTE: if thinking of generalizing this class, think about passing IDs along along with the data vector
PacketView::PacketView(wxWindow* parent) : PacketViewFrame(parent)
{
}

void PacketView::SetData(std::vector<std::string> cols, std::vector<std::string> rows)
{
    for (auto& c : cols) {
        gui_list_view->AppendTextColumn(c);
    }

    for (size_t i = 0; i < rows.size(); ++i) {
        wxVector<wxVariant> data;
        data.push_back(wxVariant(std::to_string(i)));
        data.push_back(rows[i]);
        gui_list_view->AppendItem(data);
    }
}

void PacketView::SetColumns(std::vector<std::string> columns)
{
    for (auto& col : columns) {
        gui_list_view->AppendTextColumn(col);
    }
}

void PacketView::AppendRow(std::vector<std::string> row)
{
    wxVector<wxVariant> data;
    for (size_t i = 0; i < row.size(); ++i) {
        data.push_back(row[i]);
        
    }
    gui_list_view->AppendItem(data);
}

void PacketView::SetTitle(std::string title)
{
    this->SetLabel(title);
}

void PacketView::Clear()
{
    if (gui_list_view->GetItemCount() > 0) {
        gui_list_view->DeleteAllItems();
    }
}

PacketView::~PacketView()
{
}

void PacketView::OnSelect(wxDataViewEvent&)
{
    auto row = gui_list_view->GetSelectedRow();

    // on macOS deleting items from listview is sending an OnSelect event with invalid row.
    if (row == -1)
        return;

    try {
        wxVariant value;
        gui_list_view->GetValue(value, row, gui_list_view->GetColumnCount()-1); //always display last column
        std::string valuestr = value.GetString();
        gui_text_view->SetText(*jq::pretty_json(valuestr));
    }
    catch (...) {
        //logger_err << "Problem when converting to json";
    }
}
