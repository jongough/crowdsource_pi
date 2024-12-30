// -*- C++ -*-
//
// generated by wxGlade
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//

#include "preferences.h"

// begin wxGlade: ::extracode
// end wxGlade



PreferencesWindow::PreferencesWindow(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxDialog(parent, id, title, pos, size, wxDEFAULT_DIALOG_STYLE)
{
    // begin wxGlade: PreferencesWindow::PreferencesWindow
    SetTitle(wxT("Crowdsource preferences"));
    wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* grid_sizer_1 = new wxFlexGridSizer(9, 2, 5, 5);
    sizer_1->Add(grid_sizer_1, 0, wxALL|wxEXPAND, 10);
    wxStaticText* label_1 = new wxStaticText(this, wxID_ANY, wxT("Server name:"));
    grid_sizer_1->Add(label_1, 0, 0, 0);
    server = new wxTextCtrl(this, wxID_ANY, wxT("crowdsource.kahu.earth"));
    server->SetMinSize(wxSize(300, 32));
    grid_sizer_1->Add(server, 0, 0, 0);
    wxStaticText* label_2 = new wxStaticText(this, wxID_ANY, wxT("Port:"));
    grid_sizer_1->Add(label_2, 0, 0, 0);
    port = new wxSpinCtrl(this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 65535);
    grid_sizer_1->Add(port, 0, 0, 0);
    wxStaticText* label_13 = new wxStaticText(this, wxID_ANY, wxT("API key:"));
    grid_sizer_1->Add(label_13, 0, 0, 0);
    wxBoxSizer* sizer_4 = new wxBoxSizer(wxHORIZONTAL);
    grid_sizer_1->Add(sizer_4, 1, wxEXPAND, 0);
    api_key = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
    api_key->SetMinSize(wxSize(200, 32));
    sizer_4->Add(api_key, 0, 0, 0);
    hyperlink_1 = new wxHyperlinkCtrl(this, wxID_ANY, wxT("Make account"), wxT("http://crowdsource.kahu.earth"));
    sizer_4->Add(hyperlink_1, 0, 0, 0);
    wxStaticText* label_3 = new wxStaticText(this, wxID_ANY, wxT("Minimum reconnect delay (s):"));
    grid_sizer_1->Add(label_3, 0, 0, 0);
    min_reconnect_time = new wxSpinCtrlDouble(this, wxID_ANY, wxT("100.0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.0, 100000.0);
    min_reconnect_time->SetDigits(0);
    grid_sizer_1->Add(min_reconnect_time, 0, 0, 0);
    wxStaticText* label_4 = new wxStaticText(this, wxID_ANY, wxT("Maximum reconnect delay (s):"));
    grid_sizer_1->Add(label_4, 0, 0, 0);
    max_reconnect_time = new wxSpinCtrlDouble(this, wxID_ANY, wxT("600.0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0.0, 100000.0);
    max_reconnect_time->SetDigits(0);
    grid_sizer_1->Add(max_reconnect_time, 0, 0, 0);
    wxStaticText* label_5 = new wxStaticText(this, wxID_ANY, wxT("Last connection:"));
    grid_sizer_1->Add(label_5, 0, 0, 0);
    wxStaticText* last_connection = new wxStaticText(this, wxID_ANY, wxT("2024-12-24 00:00:00"));
    grid_sizer_1->Add(last_connection, 0, 0, 0);
    wxStaticText* label_7 = new wxStaticText(this, wxID_ANY, wxT("Unsent datapoints:"));
    grid_sizer_1->Add(label_7, 0, 0, 0);
    wxStaticText* unsent_datapoints = new wxStaticText(this, wxID_ANY, wxT("1024"));
    grid_sizer_1->Add(unsent_datapoints, 0, 0, 0);
    wxStaticText* label_8 = new wxStaticText(this, wxID_ANY, wxT("Unsent tracks:"));
    grid_sizer_1->Add(label_8, 0, 0, 0);
    wxStaticText* unsent_tracks = new wxStaticText(this, wxID_ANY, wxT("32"));
    grid_sizer_1->Add(unsent_tracks, 0, 0, 0);
    wxStaticText* label_9 = new wxStaticText(this, wxID_ANY, wxT("Sent/received data:"));
    grid_sizer_1->Add(label_9, 0, 0, 0);
    wxStaticText* transferred_data = new wxStaticText(this, wxID_ANY, wxT("512kB"));
    grid_sizer_1->Add(transferred_data, 0, 0, 0);
    wxBoxSizer* sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    sizer_1->Add(sizer_2, 0, wxALIGN_RIGHT|wxALL, 4);
    button_SAVE = new wxButton(this, wxID_SAVE, wxEmptyString);
    button_SAVE->SetDefault();
    sizer_2->Add(button_SAVE, 0, 0, 0);
    button_CANCEL = new wxButton(this, wxID_CANCEL, wxEmptyString);
    sizer_2->Add(button_CANCEL, 0, 0, 0);
    
    grid_sizer_1->AddGrowableRow(0);
    grid_sizer_1->AddGrowableRow(1);
    grid_sizer_1->AddGrowableRow(3);
    grid_sizer_1->AddGrowableRow(4);
    grid_sizer_1->AddGrowableRow(5);
    grid_sizer_1->AddGrowableRow(9);
    grid_sizer_1->AddGrowableCol(0);
    grid_sizer_1->AddGrowableCol(1);
    SetSizer(sizer_1);
    sizer_1->Fit(this);
    SetAffirmativeId(button_SAVE->GetId());
    SetEscapeId(button_CANCEL->GetId());
    
    Layout();
    // end wxGlade
}
