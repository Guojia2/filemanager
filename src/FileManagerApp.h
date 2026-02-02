#ifndef FILEMANAGERAPP_H
#define FILEMANAGERAPP_H

#include <wx/wx.h>

class FileManagerApp : public wxApp
{
public:
    FileManagerApp();
    virtual ~FileManagerApp();

    virtual bool OnInit() override;

private:
};

#endif
