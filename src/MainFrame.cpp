/*
Author: Guo Jia
Description: Defines the main application window.  Wires up the menu bar,
             address bar, file-listing panel, and status bar.  Contains every
             event handler for the file operations described in the assignment.
Date: 2026-01-31
*/

#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/filename.h>
#include "MainFrame.h"
#include "FileOperations.h"
#include <wx/app.h>
#include "FileManagerApp.h"

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

/*
Function: MainFrame
Description: Constructs the main application frame.  Creates the menu bar,
             address bar, file-listing panel, and status bar; binds all
             events; and loads the user's home directory.
Parameters: title - title of the window
Return: None
*/
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
      m_filePanel(nullptr),
      m_addressBar(nullptr),
      m_statusBar(nullptr),
      m_clipboardPath(""),
      m_clipboardIsCut(false)
{
    // --- Menu bar -----------------------------------------------------------
    InitializeMenuBar();

    // --- Address bar --------------------------------------------------------
    m_addressBar = new wxTextCtrl(
        this,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxSize(-1, 28),
        wxTE_PROCESS_ENTER
    );

    // --- File listing panel -------------------------------------------------
    m_filePanel = new FilePanel(this);

    // --- Status bar ---------------------------------------------------------
    InitializeStatusBar();

    // --- Layout -------------------------------------------------------------
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_addressBar, 0, wxEXPAND | wxALL, 4);
    sizer->Add(m_filePanel,  1, wxEXPAND | wxALL, 0);
    SetSizer(sizer);

    // --- Bind events --------------------------------------------------------
    Bind(wxEVT_TEXT_ENTER,          &MainFrame::OnAddressBarEnter, this, m_addressBar->GetId());
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &MainFrame::OnListDoubleClick, this, m_filePanel->GetListCtrl()->GetId());

    Bind(wxEVT_MENU, &MainFrame::OnNewFolder, this, ID_NEW_FOLDER);
    Bind(wxEVT_MENU, &MainFrame::OnRename,    this, ID_RENAME);
    Bind(wxEVT_MENU, &MainFrame::OnDelete,    this, ID_DELETE);
    Bind(wxEVT_MENU, &MainFrame::OnCopy,      this, ID_COPY);
    Bind(wxEVT_MENU, &MainFrame::OnCut,       this, ID_CUT);
    Bind(wxEVT_MENU, &MainFrame::OnPaste,     this, ID_PASTE);
    Bind(wxEVT_MENU, &MainFrame::OnRefresh,   this, ID_REFRESH);
    // wxID_EXIT is handled automatically by wxWidgets on macOS (Cmd+Q) and
    // falls back to the Exit menu item on other platforms.
    wxDECLARE_APP(FileManagerApp);

    Bind(wxEVT_MENU, [](wxCommandEvent& /*event*/) { wxGetApp().GetTopWindow()->Close(); }, wxID_EXIT);

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
// Initialisation helpers
// ---------------------------------------------------------------------------

/*
Function: InitializeMenuBar
Description: Builds the File menu with all operations and their keyboard
             shortcuts, then attaches it to the frame.
Parameters: None
Return: None
*/
void MainFrame::InitializeMenuBar()
{
    wxMenu* fileMenu = new wxMenu();

    fileMenu->Append(ID_NEW_FOLDER, "New Folder\tCtrl+Shift+N");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_RENAME,     "Rename\tF2");
    fileMenu->Append(ID_DELETE,     "Delete\tDelete");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_COPY,       "Copy\tCtrl+C");
    fileMenu->Append(ID_CUT,        "Cut\tCtrl+X");
    fileMenu->Append(ID_PASTE,      "Paste\tCtrl+V");
    fileMenu->AppendSeparator();
    fileMenu->Append(ID_REFRESH,    "Refresh\tF5");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT,     "Exit\tCtrl+Q");

    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "File");
    SetMenuBar(menuBar);
}

/*
Function: InitializeStatusBar
Description: Creates a single-pane status bar and attaches it to the frame.
             The status bar is used to show clipboard state messages.
Parameters: None
Return: None
*/
void MainFrame::InitializeStatusBar()
{
    m_statusBar = CreateStatusBar(1);
    m_statusBar->SetStatusText("Ready");
}

// ---------------------------------------------------------------------------
// Event handlers
// ---------------------------------------------------------------------------

/*
Function: OnAddressBarEnter
Description: Called when the user presses Enter in the address bar.  Attempts
             to navigate to the typed path.  On failure the bar reverts to the
             last valid directory.
Parameters: event - the text-enter command event (unused)
Return: None
*/
void MainFrame::OnAddressBarEnter(wxCommandEvent& /*event*/)
{
    wxString typed = m_addressBar->GetValue();
    typed.Trim();
    NavigateTo(typed);
}

/*
Function: OnListDoubleClick
Description: Called when the user double-clicks a row in the file listing.
             Directories are navigated into; files are opened with the system
             default application.
Parameters: event - the list-item-activated event (unused beyond triggering)
Return: None
*/
void MainFrame::OnListDoubleClick(wxListEvent& event)
{
    // Get the index of the row that was double-clicked from the event itself.
    // Don't rely on the selection state, as the item may not be selected yet
    // when the activation event fires.
    long index = event.GetIndex();
    if (index == -1)
    {
        return;
    }

    // Get the name from that specific row.
    wxString name = m_filePanel->GetListCtrl()->GetItemText(index);
    if (name.IsEmpty())
    {
        return;
    }

    wxString fullPath = FullPath(name);
    wxFileName fn(fullPath);

    if (fn.DirExists())
    {
        NavigateTo(fullPath);
    }
    else
    {
        OpenFile(fullPath);
    }
}

/*
Function: OnNewFolder
Description: Prompts the user for a directory name, then creates it inside
             the current directory.  Shows an error dialog if the name is
             empty or creation fails.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnNewFolder(wxCommandEvent& /*event*/)
{
    wxString name = wxGetTextFromUser(
        "Enter the name for the new folder:",
        "New Folder",
        "",
        this
    );

    if (name.IsEmpty())
    {
        return;   // user cancelled or left blank
    }

    wxString fullPath = FullPath(name);

    if (!FileOperations::CreateDirectory(fullPath))
    {
        wxMessageBox("Failed to create folder \"" + name + "\".\n"
                     "A file or folder with that name may already exist.",
                     "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    m_statusBar->SetStatusText("Created folder \"" + name + "\"");
    m_filePanel->LoadDirectory(m_filePanel->CurrentPath());
}

/*
Function: OnRename
Description: Prompts the user for a new name for the selected item, then
             renames it.  Shows appropriate errors if nothing is selected or
             the rename fails.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnRename(wxCommandEvent& /*event*/)
{
    wxString name = m_filePanel->GetSelectedName();
    if (name.IsEmpty())
    {
        wxMessageBox("Please select a file or folder to rename.",
                     "Nothing Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    wxString newName = wxGetTextFromUser(
        "Enter the new name:",
        "Rename",
        name,       // pre-fill with the current name
        this
    );

    if (newName.IsEmpty())
    {
        return;   // cancelled
    }

    wxString oldPath = FullPath(name);
    wxString newPath = FullPath(newName);

    if (!FileOperations::Rename(oldPath, newPath))
    {
        wxMessageBox("Failed to rename \"" + name + "\" to \"" + newName + "\".",
                     "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    m_statusBar->SetStatusText("Renamed \"" + name + "\" to \"" + newName + "\"");
    m_filePanel->LoadDirectory(m_filePanel->CurrentPath());
}

/*
Function: OnDelete
Description: Asks the user to confirm deletion of the selected item, then
             deletes it.  Works for both files and directories (recursive).
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnDelete(wxCommandEvent& /*event*/)
{
    wxString name = m_filePanel->GetSelectedName();
    if (name.IsEmpty())
    {
        wxMessageBox("Please select a file or folder to delete.",
                     "Nothing Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    int answer = wxMessageBox(
        "Are you sure you want to delete \"" + name + "\"?\n"
        "This cannot be undone.",
        "Confirm Delete",
        wxYES_NO | wxNO_DEFAULT | wxICON_WARNING,
        this
    );

    if (answer != wxYES)
    {
        return;
    }

    wxString fullPath = FullPath(name);

    if (!FileOperations::Delete(fullPath))
    {
        wxMessageBox("Failed to delete \"" + name + "\".",
                     "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    m_statusBar->SetStatusText("Deleted \"" + name + "\"");
    m_filePanel->LoadDirectory(m_filePanel->CurrentPath());
}

/*
Function: OnCopy
Description: Marks the selected item in the virtual clipboard for a later
             copy-paste.  Updates the status bar to confirm.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnCopy(wxCommandEvent& /*event*/)
{
    wxString name = m_filePanel->GetSelectedName();
    if (name.IsEmpty())
    {
        wxMessageBox("Please select a file or folder to copy.",
                     "Nothing Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    m_clipboardPath  = FullPath(name);
    m_clipboardIsCut = false;
    m_statusBar->SetStatusText("Copied \"" + name + "\" (paste to place it)");
}

/*
Function: OnCut
Description: Marks the selected item in the virtual clipboard for a later
             cut-paste (move).  Updates the status bar to confirm.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnCut(wxCommandEvent& /*event*/)
{
    wxString name = m_filePanel->GetSelectedName();
    if (name.IsEmpty())
    {
        wxMessageBox("Please select a file or folder to cut.",
                     "Nothing Selected", wxOK | wxICON_WARNING, this);
        return;
    }

    m_clipboardPath  = FullPath(name);
    m_clipboardIsCut = true;
    m_statusBar->SetStatusText("Cut \"" + name + "\" (paste to move it)");
}

/*
Function: OnPaste
Description: Completes a pending copy or cut by placing the clipboard item
             into the current directory.  If a name collision exists the user
             is asked whether to overwrite.  Clears the clipboard and updates
             the status bar when done.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnPaste(wxCommandEvent& /*event*/)
{
    if (m_clipboardPath.IsEmpty())
    {
        wxMessageBox("Nothing to paste.  Copy or cut a file first.",
                     "Empty Clipboard", wxOK | wxICON_WARNING, this);
        return;
    }

    // Derive the destination name from the source path's filename component.
    wxFileName srcFn(m_clipboardPath);
    wxString   destName = srcFn.GetFullName();   // filename + extension
    wxString   destPath = FullPath(destName);

    // Check for collision.
    bool overwrite = false;
    if (FileOperations::Exists(destPath))
    {
        int answer = wxMessageBox(
            "\"" + destName + "\" already exists in this directory.\n"
            "Do you want to overwrite it?",
            "Overwrite?",
            wxYES_NO | wxNO_DEFAULT | wxICON_WARNING,
            this
        );
        if (answer != wxYES)
        {
            return;
        }
        overwrite = true;
    }

    bool success = false;
    if (m_clipboardIsCut)
    {
        success = FileOperations::Move(m_clipboardPath, destPath, overwrite);
    }
    else
    {
        success = FileOperations::Copy(m_clipboardPath, destPath, overwrite);
    }

    if (!success)
    {
        wxMessageBox("Paste failed.",
                     "Error", wxOK | wxICON_ERROR, this);
        return;
    }

    // Clear the clipboard and update the UI.
    m_clipboardPath.Clear();
    m_statusBar->SetStatusText("Clipboard is now empty");
    m_filePanel->LoadDirectory(m_filePanel->CurrentPath());
}

/*
Function: OnRefresh
Description: Reloads the current directory listing from disk.  This picks up
             any changes made outside the application.
Parameters: event - the menu command event (unused)
Return: None
*/
void MainFrame::OnRefresh(wxCommandEvent& /*event*/)
{
    m_filePanel->LoadDirectory(m_filePanel->CurrentPath());
    m_statusBar->SetStatusText("Refreshed");
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

/*
Function: NavigateTo
Description: Attempts to load the given directory into the file panel and
             sync the address bar.  If the directory cannot be opened an error
             dialog is shown and the address bar is reverted.
Parameters: path - the directory path to navigate to
Return: None
*/
void MainFrame::NavigateTo(const wxString& path)
{
    if (!m_filePanel->LoadDirectory(path))
    {
        wxMessageBox("Could not open directory:\n" + path,
                     "Error", wxOK | wxICON_ERROR, this);
    }
    m_addressBar->SetValue(m_filePanel->CurrentPath());
}

/*
Function: OpenFile
Description: Opens a file using the operating system's default application
             for its type.  Shows an error dialog on failure.
Parameters: path - full path of the file to open
Return: None
*/
void MainFrame::OpenFile(const wxString& path)
{
    if (!FileOperations::Open(path))
    {
        wxMessageBox("Could not open file:\n" + path,
                     "Error", wxOK | wxICON_ERROR, this);
    }
    m_statusBar->SetStatusText("Opened \"" + wxFileName(path).GetFullName() + "\"");
}

/*
Function: FullPath
Description: Joins the current directory path with a filename to produce a
             complete filesystem path.
Parameters: name - filename or folder name to append
Return: Full path string
*/
wxString MainFrame::FullPath(const wxString& name) const
{
    wxString currentPath = m_filePanel->CurrentPath();
    
    // Manually join the current directory with the name.
    // Ensure there's exactly one separator between them.
    if (!currentPath.EndsWith(wxFileName::GetPathSeparator()))
    {
        currentPath += wxFileName::GetPathSeparator();
    }
    
    return currentPath + name;
}