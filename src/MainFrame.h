/*
Author: Guo Jia
Description: Declaration of MainFrame – the top-level application window
             containing the menu bar, address bar, file-listing panel, and
             status bar.  Also owns the virtual clipboard state and declares
             every file-operation event handler.
Date: 2026-01-31
*/

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/statusbr.h>
#include <wx/menu.h>
#include <wx/listctrl.h>
#include "FilePanel.h"


class MainFrame : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);
    virtual ~MainFrame();

private:
    // -----------------------------------------------------------------------
    // UI controls
    // -----------------------------------------------------------------------
    FilePanel*    m_filePanel;
    wxTextCtrl*   m_addressBar;
    wxStatusBar*  m_statusBar;

    // -----------------------------------------------------------------------
    // Virtual clipboard – just a path and a flag; no real OS clipboard used.
    // -----------------------------------------------------------------------
    wxString  m_clipboardPath;   // full path of the file/dir marked for copy/cut
    bool      m_clipboardIsCut;  // true = cut (move), false = copy

    // -----------------------------------------------------------------------
    // Menu IDs – unique values for every action so Bind() can distinguish them.
    // -----------------------------------------------------------------------
    enum MenuId {
        ID_NEW_FOLDER = wxID_HIGHEST + 1,
        ID_RENAME,
        ID_DELETE,
        ID_COPY,
        ID_CUT,
        ID_PASTE,
        ID_REFRESH
    };

    // -----------------------------------------------------------------------
    // Initialisation helpers (called once from the constructor)
    // -----------------------------------------------------------------------
    void InitializeMenuBar();
    void InitializeStatusBar();

    // -----------------------------------------------------------------------
    // Event handlers – one per user action, in menu order
    // -----------------------------------------------------------------------
    void OnAddressBarEnter(wxCommandEvent& event);
    void OnListDoubleClick(wxListEvent& event);
    void OnNewFolder(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);
    void OnRefresh(wxCommandEvent& event);

    // -----------------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------------
    // Navigate to a directory and sync the address bar.  Shows an error dialog
    // on failure.
    void NavigateTo(const wxString& path);

    // Open a file with the system default application.
    void OpenFile(const wxString& path);

    // Returns the full path that results from joining m_filePanel->CurrentPath()
    // with the given filename.
    wxString FullPath(const wxString& name) const;
};

#endif // MAINFRAME_H