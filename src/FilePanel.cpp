/*
Author: Guo Jia
Description: Panel responsible for displaying directory contents.
Date: 2026-01-31
*/

#include "FilePanel.h"

#include <wx/dir.h>
#include <wx/filename.h>

#include <wx/dir.h>
#include <wx/datetime.h>

/*
Function: FilePanel
Description: Constructs the file panel and initializes UI controls.
Parameters: parent - parent window
Return: None
*/


FilePanel::FilePanel(wxWindow* parent)
    : wxPanel(parent),
      m_fileList(nullptr)

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

/*
Function: InitializeListControl
Description: Creates and configures the file list control.
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

    m_fileList->InsertColumn(0, "Name", wxLIST_FORMAT_LEFT, 300);
    m_fileList->InsertColumn(1, "Type", wxLIST_FORMAT_LEFT, 100);
}

/*
Function: LoadDirectory
Description: Loads the contents of a directory into the list control.
Parameters: path - filesystem path to load
Return: None
*/
void FilePanel::LoadDirectory(const wxString& path)
{
    m_fileList->DeleteAllItems();

    wxDir directory(path);
    if (!directory.IsOpened())
    {
        return;
    }

    wxString filename;
    bool hasFile = directory.GetFirst(&filename);

    long index = 0;
    while (hasFile)
    {
        wxFileName file(path, filename);

        m_fileList->InsertItem(index, filename);

        if (file.DirExists())
        {
            m_fileList->SetItem(index, 1, "Directory");
        }
        else
        {
            m_fileList->SetItem(index, 1, "File");
        }

        ++index;
        hasFile = directory.GetNext(&filename);
    }
}
