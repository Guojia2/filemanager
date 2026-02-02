#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/frame.h>
#include "FilePanel.h"


class MainFrame : public wxFrame
{
public:
    explicit MainFrame(const wxString& title);
    virtual ~MainFrame();

private:
    FilePanel* m_filePanel;
};

#endif
