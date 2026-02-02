#pragma once


#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/string.h>

class FilePanel : public wxPanel
{
public:
    explicit FilePanel(wxWindow* parent);
    virtual ~FilePanel();
    void Populate(const wxString& path);

    void LoadDirectory(const wxString& path);

private:
    wxListCtrl* m_fileList;
    wxListCtrl* m_listCtrl;
    enum Columns {
        COL_NAME = 0,
        COL_TYPE,
        COL_SIZE,
        COL_MODIFIED,
        COL_COUNT
    };

    void InitializeListControl();
};

#endif
