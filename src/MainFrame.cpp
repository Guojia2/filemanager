/*
Author: Guo Jia
Description: Defines the main application window.
Date: 2026-01-31
*/

#include <wx/sizer.h>
#include "MainFrame.h"

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

/*
Function: MainFrame
Description: Constructs the main application frame.  Lays out an editable
             address bar on top and the file-listing panel below, then loads
             the user's home directory.
Parameters: title - title of the window
Return: None
*/
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
      m_filePanel(nullptr),
      m_addressBar(nullptr)
{
    // --- Address bar --------------------------------------------------------
    m_addressBar = new wxTextCtrl(
        this,
        wxID_ANY,
        "",                      // initial value set after first LoadDirectory
        wxDefaultPosition,
        wxSize(-1, 28),          // full width, fixed height
        wxTE_PROCESS_ENTER       // so we receive EVT_TEXT_ENTER
    );

    // --- File listing panel -------------------------------------------------
    m_filePanel = new FilePanel(this);

    // --- Layout -------------------------------------------------------------
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_addressBar, 0, wxEXPAND | wxALL, 4);   // shrink-to-fit vertically
    sizer->Add(m_filePanel,  1, wxEXPAND | wxALL, 0);   // fill remaining space
    SetSizer(sizer);

    // --- Bind events --------------------------------------------------------
    Bind(wxEVT_TEXT_ENTER, &MainFrame::OnAddressBarEnter, this, m_addressBar->GetId());

    // --- Initial directory --------------------------------------------------
    wxString homeDir = wxGetHomeDir();
    m_filePanel->LoadDirectory(homeDir);
    m_addressBar->SetValue(homeDir);
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

// ---------------------------------------------------------------------------
// Event handlers
// ---------------------------------------------------------------------------

/*
Function: OnAddressBarEnter
Description: Called when the user presses Enter in the address bar.  Attempts
             to navigate to the typed path.  If the path is invalid the bar
             is reverted to the last successfully loaded directory.
Parameters: event - the text-enter command event (unused beyond signature)
Return: None
*/
void MainFrame::OnAddressBarEnter(wxCommandEvent& /*event*/)
{
    wxString typed = m_addressBar->GetValue();
    typed.Trim();                           // strip leading/trailing whitespace

    if (!m_filePanel->LoadDirectory(typed))
    {
        // Navigation failed – fall through; the address bar will be
        // restored to the last valid path below.
    }

    // Sync the bar: on success this normalises the path (e.g. trailing
    // slash); on failure it reverts to the previous valid directory.
    m_addressBar->SetValue(m_filePanel->CurrentPath());
}