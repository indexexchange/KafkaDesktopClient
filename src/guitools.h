#ifndef GUI_TOOLS_H_
#define GUI_TOOLS_H_

#include <string>
#include <vector>

#include <wx/stc/stc.h>

#include "ui/play.xpm"
#include "ui/filter.xpm"
#include "ui/logo4.xpm"
#include "ui/stop.xpm"
#include "ui/match.xpm"

// Icons:
// https://icons8.com/icons/set/play
// https://wiki.wxwidgets.org/WxStyledTextCtrl


namespace guitools {
    enum
    {
        MARGIN_LINE_NUMBERS,
        MARGIN_FOLD
    };

    enum class PNG { PLAY, FILTER, LOGO4, STOP, MATCH};

    //Event callback when a margin is clicked, used here for code folding
    inline void fold_styledtextctrl(wxStyledTextEvent& event, wxStyledTextCtrl* text) {
        // move out this function
        if (event.GetMargin() == MARGIN_FOLD)
        {
            int lineClick = text->LineFromPosition(event.GetPosition());
            int levelClick = text->GetFoldLevel(lineClick);

            if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
            {
                text->ToggleFold(lineClick);
            }
        }
    }

	inline void decorate_styledtextctrl(wxStyledTextCtrl* ctrl) {

		ctrl->SetLexer(wxSTC_LEX_CPP);
        wxColour backgroundColor = wxColour(255, 255, 255);
        wxColour color = wxColour(100, 100, 100);
        ctrl->SetProperty(wxT("fold"), wxT("1"));
        ctrl->SetProperty(wxT("fold.comment"), wxT("1"));
        ctrl->SetProperty(wxT("fold.html"), wxT("1"));
        ctrl->SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);
        int marginNum = 1; // can be 0-5 I believe 
        ctrl->SetMarginType(marginNum, wxSTC_MARGIN_SYMBOL);
        ctrl->SetMarginWidth(marginNum, 16);
        ctrl->SetMarginSensitive(marginNum, true);
        ctrl->SetMarginMask(1, wxSTC_MASK_FOLDERS);
        //ctrl->SetFoldMarginColour(true, backgroundColor);
        //ctrl->SetFoldMarginHiColour(true, backgroundColor);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, backgroundColor, color);
        ctrl->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, backgroundColor, color);

        // highlight a word
        const int NUM = 8;
        ctrl->SetIndicatorCurrent(NUM);
        ctrl->IndicatorClearRange(0, ctrl->GetLength());
        ctrl->IndicatorSetStyle(NUM, wxSTC_INDIC_STRAIGHTBOX);
        ctrl->IndicatorSetUnder(NUM, true);
        ctrl->IndicatorSetForeground(NUM, wxColour(0, 255, 0));
        ctrl->IndicatorSetOutlineAlpha(NUM, 50);
        ctrl->IndicatorSetAlpha(NUM, 30);
	}

    inline void theme_styledtextctrl(wxStyledTextCtrl* ctrl, wxColour backgroundColor, wxColour foregroundColor) {
        // set theme
        ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, backgroundColor);
        ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, foregroundColor);
        ctrl->StyleClearAll();
    }

    inline void search_styledtextctrl(wxStyledTextCtrl* ctrl, std::string word) {

        if (word.size() < 3) {
            return;
        }

        std::vector<size_t> positions; // holds all the positions that sub occurs within str

        std::string text = ctrl->GetText().ToStdString();
        size_t pos = text.find(word, 0);
        while (pos != std::string::npos)
        {
            positions.push_back(pos);
            pos = text.find(word, pos + 1);
        }

        // no matches, exit
        if (positions.empty()) {
            return;
        }

        // color results
        for (auto& p : positions) {
            ctrl->IndicatorFillRange(p, word.size());
        }

        // selection
        auto caret = ctrl->GetSelectionNCaret(0);
        for (auto& p : positions) {
            if (p > caret) {
                auto anchor = p;
                caret = p + word.size();

                ctrl->SetAnchor(anchor);
                ctrl->SetSelectionNCaret(0, caret);

                ctrl->EnsureCaretVisible();
                return;
            }
        }

        // at this point, it means we reached the end. Scroll to beginning
        auto anchor = positions[0];
        ctrl->SetAnchor(anchor);
        ctrl->SetSelectionNCaret(0, anchor + word.size());
        ctrl->EnsureCaretVisible();
    }

    inline void search_partial_styledtextctrl(wxStyledTextCtrl* ctrl, std::string word) {
        if (word.size() < 3) {
            ctrl->SetSelectionNCaret(0, 0);
            ctrl->SetAnchor(0);
            ctrl->EnsureCaretVisible();
            return;
        }

        ctrl->IndicatorClearRange(0, ctrl->GetTextLength());

        // search for all matches
        auto str = ctrl->GetValue();
        ctrl->SetSelectionNCaret(0, 0);
        ctrl->SetAnchor(0);

        std::vector<size_t> positions; // holds all the positions that sub occurs within str

        size_t pos = str.find(word, 0);
        while (pos != std::string::npos)
        {
            positions.push_back(pos);
            pos = str.find(word, pos + 1);
        }

        // color results
        for (auto& p : positions) {
            ctrl->IndicatorFillRange(p, word.size());
        }

        if (positions.size() > 0) {
            ctrl->SetAnchor(positions[0]);
            ctrl->SetSelectionNCaret(0, positions[0] + word.size());
            ctrl->EnsureCaretVisible();
        }
    }

    inline void apply_graphic(wxButton* button, PNG png) {

        wxBitmap bitmap;
        switch (png)
        {
        case guitools::PNG::PLAY:
            bitmap = wxBitmap(play_png);
            break;
        case guitools::PNG::FILTER:
            bitmap = wxBitmap(filter_png);
            break;
        case guitools::PNG::LOGO4:
            bitmap = wxBitmap(logo4_png);
            break;
        case guitools::PNG::STOP:
            bitmap = wxBitmap(stop_png);
            break;
        case guitools::PNG::MATCH:
            bitmap = wxBitmap(match_png);
            break;
        default:
            break;
        }
        button->SetBitmap(bitmap);
        button->SetBitmapCurrent(bitmap);
        button->SetBitmapFocus(bitmap);
    }
}

#endif // GUI_TOOLS_H_