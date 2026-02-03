/*
Author: Guo Jia
Description: Panel responsible for displaying directory contents.
Date: 2026-01-31
*/

#include "FilePanel.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/datetime.h>
#include <wx/sizer.h>

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

/*
Function: FilePanel
Description: Constructs the file panel and initializes UI controls.
Parameters: parent - parent window
Return: None
*/
FilePanel::FilePanel(wxWindow* parent)
    : wxPanel(parent),
      m_fileList(nullptr),
      m_currentPath("")
{
    InitializeListControl();
}

/*
Function: ~FilePanel
Description: Destroys the file panel.
Parameters: None
Return: None
*/
FilePanel::~FilePanel()
{
}

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

/*
Function: InitializeListControl
Description: Creates and configures the file list control with all four
             columns: Name, Type, Size, and Modified.
Parameters: None
Return: None
*/
void FilePanel::InitializeListControl()
{
    m_fileList = new wxListCtrl(
        this,
        wxID_ANY,
        wxDefaultPosition,
        wxDefaultSize,
        wxLC_REPORT | wxLC_SINGLE_SEL
    );

    m_fileList->InsertColumn(COL_NAME,     "Name",     wxLIST_FORMAT_LEFT,  300);
    m_fileList->InsertColumn(COL_TYPE,     "Type",     wxLIST_FORMAT_LEFT,  80);
    m_fileList->InsertColumn(COL_SIZE,     "Size",     wxLIST_FORMAT_RIGHT, 100);
    m_fileList->InsertColumn(COL_MODIFIED, "Modified", wxLIST_FORMAT_LEFT,  160);

    // Give this panel its own sizer so the list control fills it fully
    // and resizes along with the window.
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_fileList, 1, wxEXPAND);
    SetSizer(sizer);
}

// ---------------------------------------------------------------------------
// Directory loading
// ---------------------------------------------------------------------------

/*
Function: LoadDirectory
Description: Loads the contents of a directory into the list control.
             Populates Name, Type, Size, and Modified for every entry.
             Returns false and leaves the previous listing intact if the
             directory cannot be opened.
Parameters: path - filesystem path to load
Return: true if the directory was listed successfully, false otherwise
*/
bool FilePanel::LoadDirectory(const wxString& path)
{
    wxDir directory(path);
    if (!directory.IsOpened())
    {
        return false;   // leave the current listing and m_currentPath unchanged
    }

    // Directory opened successfully – commit to the new path now.
    m_currentPath = path;
    m_fileList->DeleteAllItems();

    wxString filename;
    bool hasFile = directory.GetFirst(&filename);

    long index = 0;
    while (hasFile)
    {
        wxFileName file(path, filename);

        // --- Name -----------------------------------------------------------
        m_fileList->InsertItem(index, filename);

        // --- Type -----------------------------------------------------------
        if (file.DirExists())
        {
            m_fileList->SetItem(index, COL_TYPE, "Directory");
        }
        else
        {
            m_fileList->SetItem(index, COL_TYPE, "File");
        }

        // --- Size -----------------------------------------------------------
        // Directories don't have a meaningful "size" in most file managers;
        // show a dash for them.  For regular files, stat the size via wxFileName.
        if (file.DirExists())
        {
            m_fileList->SetItem(index, COL_SIZE, "—");
        }
        else
        {
            if (file.GetSize().ToULong() != 0)
            {
                m_fileList->SetItem(index, COL_SIZE,
                                    FormatSize(file.GetSize().ToULong()));
            }
            else
            {
                m_fileList->SetItem(index, COL_SIZE, "—");
            }
        }

        // --- Modified -------------------------------------------------------
        m_fileList->SetItem(index, COL_MODIFIED, FormatDate(file));

        ++index;
        hasFile = directory.GetNext(&filename);
    }

    return true;
}

/*
Function: GetSelectedName
Description: Returns the filename of the currently selected row in the list
             control.  Used by MainFrame to know which item the user wants to
             act on.  Returns an empty string if no row is selected.
Parameters: None
Return: Name-column text of the selected item, or "" if none selected
*/
wxString FilePanel::GetSelectedName() const
{
    long selected = m_fileList->GetNextItem(-1);
    if (selected == -1)
    {
        return "";
    }
    return m_fileList->GetItemText(selected, COL_NAME);
}

// ---------------------------------------------------------------------------
// Formatting helpers
// ---------------------------------------------------------------------------

/*
Function: FormatSize
Description: Converts a byte count into a human-readable string using
             appropriate units (B, KB, MB, GB, TB).
Parameters: bytes - file size in bytes
Return: Formatted size string
*/
wxString FilePanel::FormatSize(wxUIntPtr bytes)
{
    // Use 1024-based (binary) units.
    static const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    double  size = static_cast<double>(bytes);
    unsigned int unit = 0;

    while (size >= 1024.0 && unit < 4)   // 4 == last valid index
    {
        size /= 1024.0;
        ++unit;
    }

    // Show no decimals for bytes, one decimal for everything else.
    if (unit == 0)
        return wxString::Format("%d B", static_cast<int>(size));
    else
        return wxString::Format("%.1f %s", size, units[unit]);
}

/*
Function: FormatDate
Description: Returns a short, human-readable modification-date string for
             the given file.  Falls back to "—" if the timestamp cannot be
             retrieved.
Parameters: fn - wxFileName to query
Return: Formatted date string (e.g. "2026-01-31 14:05")
*/
wxString FilePanel::FormatDate(const wxFileName& fn)
{
    wxDateTime mod;
    if (fn.GetTimes(nullptr, &mod, nullptr) && mod.IsValid())
    {
        return mod.Format("%Y-%m-%d %H:%M");
    }
    return "—";
}