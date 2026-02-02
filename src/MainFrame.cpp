/*
Author: Guo Jia
Description: Defines the main application window.
Date: 2026-01-31
*/

#include "MainFrame.h"
#include <wx/sizer.h>
#include <wx/stdpaths.h>

/*
Function: MainFrame
Description: Constructs the main application frame.
Parameters: title - title of the window
Return: None
*/
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title),
      m_filePanel(nullptr)
{
    m_filePanel = new FilePanel(this);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_filePanel, 1, wxEXPAND);
    SetSizer(sizer);

    wxString homeDir = wxStandardPaths::Get().GetUserConfigDir();
    m_filePanel->LoadDirectory(homeDir);
}


/*
Function: ~MainFrame
Description: Destroys the main application frame.
Parameters: None
Return: None
*/
MainFrame::~MainFrame()
{
}
