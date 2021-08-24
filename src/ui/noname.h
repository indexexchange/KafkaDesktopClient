///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/gdicmn.h>
#include <wx/aui/auibook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/grid.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/dataview.h>
#include <wx/stc/stc.h>
#include <wx/splitter.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wx/srchctrl.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/radiobox.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UIFrame
///////////////////////////////////////////////////////////////////////////////
class UIFrame : public wxFrame
{
	private:

	protected:
		wxAuiNotebook* auinotebook;
		wxMenuBar* m_menubar1;
		wxMenu* File;
		wxMenu* Filters;
		wxMenu* Help;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnSelectMenuImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectMenuExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectMenuQuit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectFiltersSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectMenuFiltersView( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }


	public:

		UIFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("KafkaDesktopClient X.Y"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~UIFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class FilterFrame
///////////////////////////////////////////////////////////////////////////////
class FilterFrame : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel4;
		wxTextCtrl* gui_filters_search;
		wxGrid* gui_filters_grid;
		wxButton* gui_filters_button_import;
		wxButton* gui_filters_button_add;
		wxButton* gui_filters_button_delete;
		wxButton* gui_filters_button_export;
		wxButton* gui_filters_button_close;
		wxButton* gui_filters_button_apply;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnTextFiltersSearch( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectFiltersCell( wxGridEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersImport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersExport( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickFiltersApply( wxCommandEvent& event ) { event.Skip(); }


	public:

		FilterFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1000,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~FilterFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PacketViewFrame
///////////////////////////////////////////////////////////////////////////////
class PacketViewFrame : public wxFrame
{
	private:

	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel5;
		wxDataViewListCtrl* gui_list_view;
		wxPanel* m_panel6;
		wxStyledTextCtrl* gui_text_view;

		// Virtual event handlers, overide them in your derived class
		virtual void OnSelect( wxDataViewEvent& event ) { event.Skip(); }


	public:

		PacketViewFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~PacketViewFrame();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PacketViewFrame::m_splitter1OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class CustomMessageFrame
///////////////////////////////////////////////////////////////////////////////
class CustomMessageFrame : public wxFrame
{
	private:

	protected:
		wxPanel* m_panel9;
		wxStaticBitmap* gui_logo;
		wxStaticText* gui_version;
		wxStaticText* gui_build;
		wxStaticText* m_staticText6;
		wxHyperlinkCtrl* m_hyperlink1;
		wxStaticText* m_staticText61;
		wxHyperlinkCtrl* m_hyperlink5;
		wxPanel* m_panel11;
		wxRichTextCtrl* gui_text;
		wxButton* gui_button_ok;

		// Virtual event handlers, overide them in your derived class
		virtual void OnOk( wxCommandEvent& event ) { event.Skip(); }


	public:

		CustomMessageFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 450,400 ), long style = wxFRAME_FLOAT_ON_PARENT|wxSTAY_ON_TOP|wxTAB_TRAVERSAL );

		~CustomMessageFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class InspectorPanel
///////////////////////////////////////////////////////////////////////////////
class InspectorPanel : public wxPanel
{
	private:

	protected:
		wxButton* gui_button_run;
		wxSpinCtrl* gui_buffer_size;
		wxComboBox* gui_topic;
		wxComboBox* gui_brokers;
		wxButton* gui_button_filter;
		wxComboBox* gui_filter;
		wxStaticText* gui_console;
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel5;
		wxDataViewListCtrl* gui_list_view;
		wxPanel* m_panel6;
		wxSearchCtrl* gui_search;
		wxStyledTextCtrl* gui_text_view;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKeyUpFilter( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnSelect( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnSearchCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearch( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearchPartial( wxCommandEvent& event ) { event.Skip(); }


	public:

		InspectorPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,600 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~InspectorPanel();

		void m_splitter1OnIdle( wxIdleEvent& )
		{
			m_splitter1->SetSashPosition( 0 );
			m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( InspectorPanel::m_splitter1OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class StatisticsPanel
///////////////////////////////////////////////////////////////////////////////
class StatisticsPanel : public wxPanel
{
	private:

	protected:
		wxButton* gui_button_run;
		wxSpinCtrl* gui_buffer_size;
		wxComboBox* gui_topic;
		wxComboBox* gui_brokers;
		wxStaticText* m_staticText4;
		wxComboBox* gui_filter;
		wxStaticText* gui_console;
		wxGrid* gui_grid;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }


	public:

		StatisticsPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,600 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~StatisticsPanel();

};

///////////////////////////////////////////////////////////////////////////////
/// Class TrafficMatchPanel
///////////////////////////////////////////////////////////////////////////////
class TrafficMatchPanel : public wxPanel
{
	private:

	protected:
		wxButton* gui_button_run;
		wxSpinCtrl* gui_buffer_size;
		wxComboBox* gui_brokers;
		wxComboBox* gui_topic1;
		wxComboBox* gui_filter1;
		wxChoice* gui_choice;
		wxComboBox* gui_topic2;
		wxComboBox* gui_filter2;
		wxStaticText* m_staticText5;
		wxComboBox* gui_prefilter1;
		wxStaticText* m_staticText6;
		wxComboBox* gui_prefilter2;
		wxStaticLine* m_staticline1;
		wxRadioBox* gui_browse;
		wxButton* gui_button_filter;
		wxComboBox* gui_filter;
		wxStaticText* gui_console;
		wxSplitterWindow* m_splitter7;
		wxPanel* m_panel14;
		wxDataViewListCtrl* gui_list_view;
		wxButton* gui_button_match;
		wxPanel* m_panel15;
		wxStyledTextCtrl* gui_text_view;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRun( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnKeyUpTopic1( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnCompareChoice( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnkeyUpTopic2( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnBrowse( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnFilter( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnSelect( wxDataViewEvent& event ) { event.Skip(); }
		virtual void OnMatch( wxCommandEvent& event ) { event.Skip(); }


	public:

		TrafficMatchPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 900,600 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
		~TrafficMatchPanel();

		void m_splitter7OnIdle( wxIdleEvent& )
		{
			m_splitter7->SetSashPosition( 0 );
			m_splitter7->Disconnect( wxEVT_IDLE, wxIdleEventHandler( TrafficMatchPanel::m_splitter7OnIdle ), NULL, this );
		}

};

