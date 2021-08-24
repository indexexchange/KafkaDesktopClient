///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

UIFrame::UIFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	auinotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	bSizer8->Add( auinotebook, 1, wxEXPAND | wxALL, 0 );


	this->SetSizer( bSizer8 );
	this->Layout();
	m_menubar1 = new wxMenuBar( 0 );
	File = new wxMenu();
	wxMenuItem* gui_menu_import;
	gui_menu_import = new wxMenuItem( File, wxID_ANY, wxString( wxT("Import...") ) , wxEmptyString, wxITEM_NORMAL );
	File->Append( gui_menu_import );

	wxMenuItem* gui_menu_file_export;
	gui_menu_file_export = new wxMenuItem( File, wxID_ANY, wxString( wxT("Export...") ) , wxEmptyString, wxITEM_NORMAL );
	File->Append( gui_menu_file_export );

	wxMenuItem* gui_menu_file_quit;
	gui_menu_file_quit = new wxMenuItem( File, wxID_ANY, wxString( wxT("Quit") ) , wxEmptyString, wxITEM_NORMAL );
	File->Append( gui_menu_file_quit );

	m_menubar1->Append( File, wxT("File") );

	Filters = new wxMenu();
	wxMenuItem* gui_menu_filters_save;
	gui_menu_filters_save = new wxMenuItem( Filters, wxID_ANY, wxString( wxT("Add to Favorites") ) , wxEmptyString, wxITEM_NORMAL );
	Filters->Append( gui_menu_filters_save );

	wxMenuItem* gui_menu_filters_view;
	gui_menu_filters_view = new wxMenuItem( Filters, wxID_ANY, wxString( wxT("View All") ) , wxEmptyString, wxITEM_NORMAL );
	Filters->Append( gui_menu_filters_view );

	m_menubar1->Append( Filters, wxT("Filters") );

	Help = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( Help, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	Help->Append( m_menuItem3 );

	m_menubar1->Append( Help, wxT("Help") );

	this->SetMenuBar( m_menubar1 );


	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( UIFrame::OnClose ) );
	File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnSelectMenuImport ), this, gui_menu_import->GetId());
	File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnSelectMenuExport ), this, gui_menu_file_export->GetId());
	File->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnSelectMenuQuit ), this, gui_menu_file_quit->GetId());
	Filters->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnSelectFiltersSave ), this, gui_menu_filters_save->GetId());
	Filters->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnSelectMenuFiltersView ), this, gui_menu_filters_view->GetId());
	Help->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( UIFrame::OnAbout ), this, m_menuItem3->GetId());
}

UIFrame::~UIFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( UIFrame::OnClose ) );

}

FilterFrame::FilterFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );

	m_panel4 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer131;
	bSizer131 = new wxBoxSizer( wxHORIZONTAL );

	gui_filters_search = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer131->Add( gui_filters_search, 1, wxALL, 5 );


	bSizer10->Add( bSizer131, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );

	gui_filters_grid = new wxGrid( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	gui_filters_grid->CreateGrid( 0, 4 );
	gui_filters_grid->EnableEditing( true );
	gui_filters_grid->EnableGridLines( true );
	gui_filters_grid->EnableDragGridSize( false );
	gui_filters_grid->SetMargins( 0, 0 );

	// Columns
	gui_filters_grid->SetColSize( 0, 120 );
	gui_filters_grid->SetColSize( 1, 200 );
	gui_filters_grid->SetColSize( 2, 80 );
	gui_filters_grid->SetColSize( 3, 480 );
	gui_filters_grid->EnableDragColMove( false );
	gui_filters_grid->EnableDragColSize( true );
	gui_filters_grid->SetColLabelSize( 30 );
	gui_filters_grid->SetColLabelValue( 0, wxT("Name") );
	gui_filters_grid->SetColLabelValue( 1, wxT("Description") );
	gui_filters_grid->SetColLabelValue( 2, wxT("Topic") );
	gui_filters_grid->SetColLabelValue( 3, wxT("Filter") );
	gui_filters_grid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	gui_filters_grid->EnableDragRowSize( true );
	gui_filters_grid->SetRowLabelSize( 80 );
	gui_filters_grid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	gui_filters_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer13->Add( gui_filters_grid, 1, wxALL, 5 );


	bSizer10->Add( bSizer13, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	gui_filters_button_import = new wxButton( m_panel4, wxID_ANY, wxT("Import from File"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( gui_filters_button_import, 0, wxALL, 5 );

	gui_filters_button_add = new wxButton( m_panel4, wxID_ANY, wxT("Add Row"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( gui_filters_button_add, 0, wxALL, 5 );

	gui_filters_button_delete = new wxButton( m_panel4, wxID_ANY, wxT("Delete Row"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( gui_filters_button_delete, 0, wxALL, 5 );

	gui_filters_button_export = new wxButton( m_panel4, wxID_ANY, wxT("Export to file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( gui_filters_button_export, 0, wxALL, 5 );

	gui_filters_button_close = new wxButton( m_panel4, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( gui_filters_button_close, 0, wxALL, 5 );


	bSizer12->Add( bSizer15, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );

	gui_filters_button_apply = new wxButton( m_panel4, wxID_ANY, wxT("Apply Selected Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer14->Add( gui_filters_button_apply, 0, wxALL, 5 );


	bSizer12->Add( bSizer14, 0, wxALIGN_RIGHT|wxEXPAND, 5 );


	bSizer10->Add( bSizer12, 0, wxEXPAND, 5 );


	m_panel4->SetSizer( bSizer10 );
	m_panel4->Layout();
	bSizer10->Fit( m_panel4 );
	bSizer9->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer9 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FilterFrame::OnClose ) );
	gui_filters_search->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FilterFrame::OnTextFiltersSearch ), NULL, this );
	gui_filters_grid->Connect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( FilterFrame::OnSelectFiltersCell ), NULL, this );
	gui_filters_button_import->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersImport ), NULL, this );
	gui_filters_button_add->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersAdd ), NULL, this );
	gui_filters_button_delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersDelete ), NULL, this );
	gui_filters_button_export->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersExport ), NULL, this );
	gui_filters_button_close->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersClose ), NULL, this );
	gui_filters_button_apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersApply ), NULL, this );
}

FilterFrame::~FilterFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FilterFrame::OnClose ) );
	gui_filters_search->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FilterFrame::OnTextFiltersSearch ), NULL, this );
	gui_filters_grid->Disconnect( wxEVT_GRID_SELECT_CELL, wxGridEventHandler( FilterFrame::OnSelectFiltersCell ), NULL, this );
	gui_filters_button_import->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersImport ), NULL, this );
	gui_filters_button_add->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersAdd ), NULL, this );
	gui_filters_button_delete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersDelete ), NULL, this );
	gui_filters_button_export->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersExport ), NULL, this );
	gui_filters_button_close->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersClose ), NULL, this );
	gui_filters_button_apply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FilterFrame::OnClickFiltersApply ), NULL, this );

}

PacketViewFrame::PacketViewFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( PacketViewFrame::m_splitter1OnIdle ), NULL, this );

	m_panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );

	gui_list_view = new wxDataViewListCtrl( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES );
	bSizer22->Add( gui_list_view, 1, wxALL|wxEXPAND, 5 );


	m_panel5->SetSizer( bSizer22 );
	m_panel5->Layout();
	bSizer22->Fit( m_panel5 );
	m_panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	gui_text_view = new wxStyledTextCtrl( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	gui_text_view->SetUseTabs( true );
	gui_text_view->SetTabWidth( 4 );
	gui_text_view->SetIndent( 4 );
	gui_text_view->SetTabIndents( true );
	gui_text_view->SetBackSpaceUnIndents( true );
	gui_text_view->SetViewEOL( false );
	gui_text_view->SetViewWhiteSpace( false );
	gui_text_view->SetMarginWidth( 2, 0 );
	gui_text_view->SetIndentationGuides( true );
	gui_text_view->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	gui_text_view->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	gui_text_view->SetMarginWidth( 1, 16);
	gui_text_view->SetMarginSensitive( 1, true );
	gui_text_view->SetProperty( wxT("fold"), wxT("1") );
	gui_text_view->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	gui_text_view->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	gui_text_view->SetMarginWidth( 0, gui_text_view->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	gui_text_view->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer23->Add( gui_text_view, 1, wxEXPAND | wxALL, 2 );


	m_panel6->SetSizer( bSizer23 );
	m_panel6->Layout();
	bSizer23->Fit( m_panel6 );
	m_splitter1->SplitVertically( m_panel5, m_panel6, 0 );
	bSizer20->Add( m_splitter1, 1, wxEXPAND, 0 );


	this->SetSizer( bSizer20 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	gui_list_view->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( PacketViewFrame::OnSelect ), NULL, this );
}

PacketViewFrame::~PacketViewFrame()
{
	// Disconnect Events
	gui_list_view->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( PacketViewFrame::OnSelect ), NULL, this );

}

CustomMessageFrame::CustomMessageFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );

	m_panel9 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel9->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );
	m_panel9->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxHORIZONTAL );

	gui_logo = new wxStaticBitmap( m_panel9, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	gui_logo->SetMinSize( wxSize( 64,64 ) );

	bSizer39->Add( gui_logo, 0, wxALL, 5 );

	wxBoxSizer* bSizer50;
	bSizer50 = new wxBoxSizer( wxVERTICAL );

	gui_version = new wxStaticText( m_panel9, wxID_ANY, wxT("Kafka Desktop Client v2.0"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_version->Wrap( -1 );
	bSizer50->Add( gui_version, 0, wxALL, 5 );

	gui_build = new wxStaticText( m_panel9, wxID_ANY, wxT("Build time: 29 Nov 2020 - 02:34:30"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_build->Wrap( -1 );
	bSizer50->Add( gui_build, 0, wxALL, 5 );


	bSizer39->Add( bSizer50, 1, wxEXPAND, 5 );


	bSizer52->Add( bSizer39, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( m_panel9, wxID_ANY, wxT("Home:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer40->Add( m_staticText6, 0, wxALL, 5 );

	m_hyperlink1 = new wxHyperlinkCtrl( m_panel9, wxID_ANY, wxT("KafkaDesktopClient"), wxT("KafkaDesktopClient"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer40->Add( m_hyperlink1, 0, wxALL, 5 );


	bSizer52->Add( bSizer40, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer401;
	bSizer401 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText61 = new wxStaticText( m_panel9, wxID_ANY, wxT("Slack:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	bSizer401->Add( m_staticText61, 0, wxALL, 5 );

	m_hyperlink5 = new wxHyperlinkCtrl( m_panel9, wxID_ANY, wxT("#kafkadesktopclient"), wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer401->Add( m_hyperlink5, 0, wxALL, 5 );


	bSizer52->Add( bSizer401, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer49;
	bSizer49 = new wxBoxSizer( wxVERTICAL );

	m_panel11 = new wxPanel( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("test") );
	wxBoxSizer* bSizer55;
	bSizer55 = new wxBoxSizer( wxVERTICAL );

	gui_text = new wxRichTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer55->Add( gui_text, 1, wxEXPAND | wxALL, 5 );


	m_panel11->SetSizer( bSizer55 );
	m_panel11->Layout();
	bSizer55->Fit( m_panel11 );
	bSizer49->Add( m_panel11, 1, wxEXPAND | wxALL, 5 );


	bSizer52->Add( bSizer49, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer48;
	bSizer48 = new wxBoxSizer( wxVERTICAL );

	gui_button_ok = new wxButton( m_panel9, wxID_ANY, wxT("ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer48->Add( gui_button_ok, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bSizer52->Add( bSizer48, 0, wxEXPAND, 5 );


	m_panel9->SetSizer( bSizer52 );
	m_panel9->Layout();
	bSizer52->Fit( m_panel9 );
	bSizer38->Add( m_panel9, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer38 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	gui_button_ok->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomMessageFrame::OnOk ), NULL, this );
}

CustomMessageFrame::~CustomMessageFrame()
{
	// Disconnect Events
	gui_button_ok->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CustomMessageFrame::OnOk ), NULL, this );

}

InspectorPanel::InspectorPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	gui_button_run = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_button_run->SetToolTip( wxT("Collect kafka requests") );

	bSizer18->Add( gui_button_run, 0, wxALL, 5 );

	gui_buffer_size = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 99999, 100 );
	gui_buffer_size->SetToolTip( wxT("Max requests to collect") );

	bSizer18->Add( gui_buffer_size, 1, wxALL, 5 );

	gui_topic = new wxComboBox( this, wxID_ANY, wxT("topic_name"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_topic->SetSelection( 0 );
	gui_topic->SetToolTip( wxT("topic") );

	bSizer18->Add( gui_topic, 4, wxALL, 5 );

	gui_brokers = new wxComboBox( this, wxID_ANY, wxT("10.5.5.10,10.5.5.11"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_brokers->SetToolTip( wxT("IX brokers") );

	bSizer18->Add( gui_brokers, 10, wxALL, 5 );


	bSizer14->Add( bSizer18, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	gui_button_filter = new wxButton( this, wxID_ANY, wxT("Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_button_filter->SetToolTip( wxT("execute filter") );

	bSizer19->Add( gui_button_filter, 0, wxALL, 5 );

	gui_filter = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_filter->Append( wxT(".request") );
	gui_filter->SetToolTip( wxT("filter jq") );

	bSizer19->Add( gui_filter, 14, wxALL, 5 );


	bSizer14->Add( bSizer19, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );

	gui_console = new wxStaticText( this, wxID_ANY, wxT("[statistics]"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_console->Wrap( -1 );
	bSizer81->Add( gui_console, 0, wxALL, 2 );


	bSizer14->Add( bSizer81, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );

	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( InspectorPanel::m_splitter1OnIdle ), NULL, this );

	m_panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );

	gui_list_view = new wxDataViewListCtrl( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_HORIZ_RULES|wxDV_ROW_LINES );
	bSizer22->Add( gui_list_view, 1, wxALL|wxEXPAND, 5 );


	m_panel5->SetSizer( bSizer22 );
	m_panel5->Layout();
	bSizer22->Fit( m_panel5 );
	m_panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxHORIZONTAL );

	gui_search = new wxSearchCtrl( m_panel6, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	#ifndef __WXMAC__
	gui_search->ShowSearchButton( true );
	#endif
	gui_search->ShowCancelButton( true );
	bSizer37->Add( gui_search, 1, wxALL, 2 );


	bSizer23->Add( bSizer37, 0, wxEXPAND, 5 );

	gui_text_view = new wxStyledTextCtrl( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	gui_text_view->SetUseTabs( true );
	gui_text_view->SetTabWidth( 4 );
	gui_text_view->SetIndent( 4 );
	gui_text_view->SetTabIndents( true );
	gui_text_view->SetBackSpaceUnIndents( true );
	gui_text_view->SetViewEOL( false );
	gui_text_view->SetViewWhiteSpace( false );
	gui_text_view->SetMarginWidth( 2, 0 );
	gui_text_view->SetIndentationGuides( true );
	gui_text_view->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	gui_text_view->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	gui_text_view->SetMarginWidth( 1, 16);
	gui_text_view->SetMarginSensitive( 1, true );
	gui_text_view->SetProperty( wxT("fold"), wxT("1") );
	gui_text_view->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	gui_text_view->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	gui_text_view->SetMarginWidth( 0, gui_text_view->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	gui_text_view->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer23->Add( gui_text_view, 1, wxEXPAND | wxALL, 2 );


	m_panel6->SetSizer( bSizer23 );
	m_panel6->Layout();
	bSizer23->Fit( m_panel6 );
	m_splitter1->SplitVertically( m_panel5, m_panel6, 0 );
	bSizer20->Add( m_splitter1, 1, wxEXPAND, 0 );


	bSizer14->Add( bSizer20, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer14 );
	this->Layout();

	// Connect Events
	gui_button_run->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InspectorPanel::OnRun ), NULL, this );
	gui_button_filter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InspectorPanel::OnFilter ), NULL, this );
	gui_filter->Connect( wxEVT_KEY_UP, wxKeyEventHandler( InspectorPanel::OnKeyUpFilter ), NULL, this );
	gui_list_view->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( InspectorPanel::OnSelect ), NULL, this );
	gui_search->Connect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( InspectorPanel::OnSearchCancel ), NULL, this );
	gui_search->Connect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( InspectorPanel::OnSearch ), NULL, this );
	gui_search->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InspectorPanel::OnSearchPartial ), NULL, this );
}

InspectorPanel::~InspectorPanel()
{
	// Disconnect Events
	gui_button_run->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InspectorPanel::OnRun ), NULL, this );
	gui_button_filter->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( InspectorPanel::OnFilter ), NULL, this );
	gui_filter->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( InspectorPanel::OnKeyUpFilter ), NULL, this );
	gui_list_view->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( InspectorPanel::OnSelect ), NULL, this );
	gui_search->Disconnect( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, wxCommandEventHandler( InspectorPanel::OnSearchCancel ), NULL, this );
	gui_search->Disconnect( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, wxCommandEventHandler( InspectorPanel::OnSearch ), NULL, this );
	gui_search->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( InspectorPanel::OnSearchPartial ), NULL, this );

}

StatisticsPanel::StatisticsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	gui_button_run = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_button_run->SetToolTip( wxT("start counting") );

	bSizer17->Add( gui_button_run, 0, wxALL, 5 );

	gui_buffer_size = new wxSpinCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 999999990, 0 );
	gui_buffer_size->SetToolTip( wxT("buffer size; if 0 will run until manual stop") );

	bSizer17->Add( gui_buffer_size, 1, wxALL, 5 );

	gui_topic = new wxComboBox( this, wxID_ANY, wxT("topic_name"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_topic->SetToolTip( wxT("topic") );

	bSizer17->Add( gui_topic, 4, wxALL, 5 );

	gui_brokers = new wxComboBox( this, wxID_ANY, wxT("10.5.5.10,10.5.5.11"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_brokers->SetToolTip( wxT("IX brokers") );

	bSizer17->Add( gui_brokers, 10, wxALL, 5 );


	bSizer16->Add( bSizer17, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Prefilter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer35->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );

	gui_filter = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_filter->SetToolTip( wxT("this filter runs first") );

	bSizer35->Add( gui_filter, 1, wxALL, 5 );


	bSizer16->Add( bSizer35, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer201;
	bSizer201 = new wxBoxSizer( wxVERTICAL );

	gui_console = new wxStaticText( this, wxID_ANY, wxT("Total packets processed: 0"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_console->Wrap( -1 );
	bSizer201->Add( gui_console, 0, wxALL, 5 );


	bSizer16->Add( bSizer201, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer181;
	bSizer181 = new wxBoxSizer( wxHORIZONTAL );

	gui_grid = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	gui_grid->CreateGrid( 15, 3 );
	gui_grid->EnableEditing( true );
	gui_grid->EnableGridLines( true );
	gui_grid->EnableDragGridSize( false );
	gui_grid->SetMargins( 0, 0 );

	// Columns
	gui_grid->SetColSize( 0, 599 );
	gui_grid->SetColSize( 1, 80 );
	gui_grid->SetColSize( 2, 89 );
	gui_grid->EnableDragColMove( false );
	gui_grid->EnableDragColSize( true );
	gui_grid->SetColLabelSize( 30 );
	gui_grid->SetColLabelValue( 0, wxT("Filter") );
	gui_grid->SetColLabelValue( 1, wxT("# Hits") );
	gui_grid->SetColLabelValue( 2, wxT("% of Total") );
	gui_grid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	gui_grid->EnableDragRowSize( true );
	gui_grid->SetRowLabelSize( 80 );
	gui_grid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	gui_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer181->Add( gui_grid, 1, wxALL, 5 );


	bSizer16->Add( bSizer181, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer16 );
	this->Layout();

	// Connect Events
	gui_button_run->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StatisticsPanel::OnRun ), NULL, this );
}

StatisticsPanel::~StatisticsPanel()
{
	// Disconnect Events
	gui_button_run->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( StatisticsPanel::OnRun ), NULL, this );

}

TrafficMatchPanel::TrafficMatchPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer202;
	bSizer202 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );

	gui_button_run = new wxButton( this, wxID_ANY, wxT("Run"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_button_run->SetToolTip( wxT("start search for matching requests") );

	bSizer25->Add( gui_button_run, 0, wxALL, 5 );

	gui_buffer_size = new wxSpinCtrl( this, wxID_ANY, wxT("30"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000, 0 );
	gui_buffer_size->SetToolTip( wxT("Careful: do not use large numbers. The topic is buffered into a temporary file until the max number is reached. If this is too large, the temporary file can be very large") );

	bSizer25->Add( gui_buffer_size, 0, wxALL, 5 );

	gui_brokers = new wxComboBox( this, wxID_ANY, wxT("10.5.5.10,10.5.5.11"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer25->Add( gui_brokers, 10, wxALL, 5 );


	bSizer202->Add( bSizer25, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );

	gui_topic1 = new wxComboBox( this, wxID_ANY, wxT("topic1"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_topic1->SetToolTip( wxT("topic1") );

	bSizer26->Add( gui_topic1, 1, wxALL, 5 );

	gui_filter1 = new wxComboBox( this, wxID_ANY, wxT(".id"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_filter1->SetToolTip( wxT("field to match with topic 2") );

	bSizer26->Add( gui_filter1, 2, wxALL, 5 );

	wxString gui_choiceChoices[] = { wxT("=="), wxT("in") };
	int gui_choiceNChoices = sizeof( gui_choiceChoices ) / sizeof( wxString );
	gui_choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, gui_choiceNChoices, gui_choiceChoices, 0 );
	gui_choice->SetSelection( 0 );
	gui_choice->SetToolTip( wxT("use equality of fields for match") );

	bSizer26->Add( gui_choice, 0, wxALL, 5 );

	gui_topic2 = new wxComboBox( this, wxID_ANY, wxT("topic2"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_topic2->SetToolTip( wxT("topic2") );

	bSizer26->Add( gui_topic2, 1, wxALL, 5 );

	gui_filter2 = new wxComboBox( this, wxID_ANY, wxT(".id"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	gui_filter2->SetToolTip( wxT("field to match with topic 1") );

	bSizer26->Add( gui_filter2, 2, wxALL, 5 );


	bSizer202->Add( bSizer26, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Topic1 Prefilter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer36->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );

	gui_prefilter1 = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer36->Add( gui_prefilter1, 1, wxALL, 5 );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Topic2 Prefilter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer36->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );

	gui_prefilter2 = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer36->Add( gui_prefilter2, 1, wxALL, 5 );


	bSizer202->Add( bSizer36, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxHORIZONTAL );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer28->Add( m_staticline1, 1, wxEXPAND | wxALL, 5 );


	bSizer202->Add( bSizer28, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxVERTICAL );

	wxString gui_browseChoices[] = { wxT("topic1"), wxT("topic2") };
	int gui_browseNChoices = sizeof( gui_browseChoices ) / sizeof( wxString );
	gui_browse = new wxRadioBox( this, wxID_ANY, wxT("Browse"), wxDefaultPosition, wxDefaultSize, gui_browseNChoices, gui_browseChoices, 1, wxRA_SPECIFY_COLS );
	gui_browse->SetSelection( 0 );
	gui_browse->SetToolTip( wxT("browse results from topic 1 or 2") );

	bSizer42->Add( gui_browse, 0, wxALL, 5 );


	bSizer40->Add( bSizer42, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer46;
	bSizer46 = new wxBoxSizer( wxHORIZONTAL );

	gui_button_filter = new wxButton( this, wxID_ANY, wxT("Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer46->Add( gui_button_filter, 0, wxALL, 5 );

	gui_filter = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer46->Add( gui_filter, 1, wxALL, 5 );


	bSizer43->Add( bSizer46, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );

	gui_console = new wxStaticText( this, wxID_ANY, wxT("[console]"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_console->Wrap( -1 );
	bSizer45->Add( gui_console, 1, wxALIGN_CENTER|wxALL, 5 );


	bSizer43->Add( bSizer45, 1, wxEXPAND, 5 );


	bSizer40->Add( bSizer43, 1, wxEXPAND, 5 );


	bSizer30->Add( bSizer40, 0, wxEXPAND, 5 );

	m_splitter7 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter7->Connect( wxEVT_IDLE, wxIdleEventHandler( TrafficMatchPanel::m_splitter7OnIdle ), NULL, this );

	m_panel14 = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );

	gui_list_view = new wxDataViewListCtrl( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer38->Add( gui_list_view, 1, wxALL|wxEXPAND, 5 );

	gui_button_match = new wxButton( m_panel14, wxID_ANY, wxT("find match"), wxDefaultPosition, wxDefaultSize, 0 );
	gui_button_match->SetToolTip( wxT("after requests are collected, select a request and click this") );

	bSizer38->Add( gui_button_match, 0, wxALIGN_RIGHT|wxALL, 5 );


	m_panel14->SetSizer( bSizer38 );
	m_panel14->Layout();
	bSizer38->Fit( m_panel14 );
	m_panel15 = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );

	gui_text_view = new wxStyledTextCtrl( m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxEmptyString );
	gui_text_view->SetUseTabs( true );
	gui_text_view->SetTabWidth( 4 );
	gui_text_view->SetIndent( 4 );
	gui_text_view->SetTabIndents( true );
	gui_text_view->SetBackSpaceUnIndents( true );
	gui_text_view->SetViewEOL( false );
	gui_text_view->SetViewWhiteSpace( false );
	gui_text_view->SetMarginWidth( 2, 0 );
	gui_text_view->SetIndentationGuides( true );
	gui_text_view->SetMarginType( 1, wxSTC_MARGIN_SYMBOL );
	gui_text_view->SetMarginMask( 1, wxSTC_MASK_FOLDERS );
	gui_text_view->SetMarginWidth( 1, 16);
	gui_text_view->SetMarginSensitive( 1, true );
	gui_text_view->SetProperty( wxT("fold"), wxT("1") );
	gui_text_view->SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
	gui_text_view->SetMarginType( 0, wxSTC_MARGIN_NUMBER );
	gui_text_view->SetMarginWidth( 0, gui_text_view->TextWidth( wxSTC_STYLE_LINENUMBER, wxT("_99999") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDER, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUS );
	gui_text_view->MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("BLACK") ) );
	gui_text_view->MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, wxColour( wxT("WHITE") ) );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY );
	gui_text_view->SetSelBackground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
	gui_text_view->SetSelForeground( true, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	bSizer39->Add( gui_text_view, 1, wxEXPAND | wxALL, 5 );


	m_panel15->SetSizer( bSizer39 );
	m_panel15->Layout();
	bSizer39->Fit( m_panel15 );
	m_splitter7->SplitVertically( m_panel14, m_panel15, 0 );
	bSizer30->Add( m_splitter7, 1, wxEXPAND, 5 );


	bSizer202->Add( bSizer30, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer202 );
	this->Layout();

	// Connect Events
	gui_button_run->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnRun ), NULL, this );
	gui_topic1->Connect( wxEVT_KEY_UP, wxKeyEventHandler( TrafficMatchPanel::OnKeyUpTopic1 ), NULL, this );
	gui_choice->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TrafficMatchPanel::OnCompareChoice ), NULL, this );
	gui_topic2->Connect( wxEVT_KEY_UP, wxKeyEventHandler( TrafficMatchPanel::OnkeyUpTopic2 ), NULL, this );
	gui_browse->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TrafficMatchPanel::OnBrowse ), NULL, this );
	gui_button_filter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnFilter ), NULL, this );
	gui_list_view->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( TrafficMatchPanel::OnActivated ), NULL, this );
	gui_list_view->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( TrafficMatchPanel::OnSelect ), NULL, this );
	gui_button_match->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnMatch ), NULL, this );
}

TrafficMatchPanel::~TrafficMatchPanel()
{
	// Disconnect Events
	gui_button_run->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnRun ), NULL, this );
	gui_topic1->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( TrafficMatchPanel::OnKeyUpTopic1 ), NULL, this );
	gui_choice->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TrafficMatchPanel::OnCompareChoice ), NULL, this );
	gui_topic2->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( TrafficMatchPanel::OnkeyUpTopic2 ), NULL, this );
	gui_browse->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( TrafficMatchPanel::OnBrowse ), NULL, this );
	gui_button_filter->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnFilter ), NULL, this );
	gui_list_view->Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( TrafficMatchPanel::OnActivated ), NULL, this );
	gui_list_view->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( TrafficMatchPanel::OnSelect ), NULL, this );
	gui_button_match->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TrafficMatchPanel::OnMatch ), NULL, this );

}
