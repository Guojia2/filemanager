/*
Author: Guo Jia
Description: Declaration of MainFrame – the top-level application window
             containing the address bar and the file-listing panel.
Date: 2026-01-31
*/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/frame.h>
#include <wx/textctrl.h>
#include "FilePanel.h"

class MainFrame : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);
    virtual ~MainFrame();

private:
    FilePanel*   m_filePanel;
    wxTextCtrl*  m_addressBar;

    // Event handler: user pressed Enter inside the address bar.
    void OnAddressBarEnter(wxCommandEvent& event);
};

#endif // MAINFRAME_H