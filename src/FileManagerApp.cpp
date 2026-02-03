/*
Author: Guo Jia
Description: Application entry point for the file manager.
Date: 2026-01-31
*/

#include "FileManagerApp.h"
#include "MainFrame.h"

#include <wx/wx.h>
#include <wx/app.h> 
/*
Function: FileManagerApp
Description: Constructs the application object.
Parameters: None
Return: None
*/
FileManagerApp::FileManagerApp()
{
}

/*
Function: ~FileManagerApp
Description: Destroys the application object.
Parameters: None
Return: None
*/
FileManagerApp::~FileManagerApp()
{
}

/*
Function: OnInit
Description: Initializes the application and creates the main window.
Parameters: None
Return: True if initialization succeeds, false otherwise
*/
bool FileManagerApp::OnInit()
{
    MainFrame* frame = new MainFrame("Simple File Manager");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(FileManagerApp);
