/*
Author: Guo Jia
Description: Declaration of FilePanel – the panel that displays a directory
             listing with Name, Type, Size, and Modified columns.
Date: 2026-01-31
*/

#ifndef FILEPANEL_H
#define FILEPANEL_H

#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/string.h>
#include <wx/filename.h>

class FilePanel : public wxPanel
{
public:
    explicit FilePanel(wxWindow* parent);
    virtual ~FilePanel();

    // Reload the list from the given path.  Returns false if the directory
    // could not be opened (the previous listing is left intact).
    bool LoadDirectory(const wxString& path);

    // Read-only accessor so MainFrame can keep its address bar in sync.
    const wxString& CurrentPath() const { return m_currentPath; }

private:
    // Column indices – kept in sync with InitializeListControl().
    enum Columns {
        COL_NAME = 0,
        COL_TYPE,
        COL_SIZE,
        COL_MODIFIED,
        COL_COUNT          // sentinel – not a real column
    };

    wxListCtrl* m_fileList;
    wxString    m_currentPath;   // last successfully loaded directory

    void InitializeListControl();

    // Pretty-print helpers
    static wxString FormatSize(wxUIntPtr bytes);
    static wxString FormatDate(const wxFileName& fn);
};

#endif // FILEPANEL_H