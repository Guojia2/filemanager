/*
Author: Guo Jia
Description: Implementation of FileOperations – wraps std::filesystem calls
             for all file/directory manipulation the file manager performs.
Date: 2026-02-02
*/

#include <filesystem>
#include <wx/utils.h>
#include "FileOperations.h"

using namespace std::filesystem;

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

/*
Function: FileOperations
Description: Default constructor.  No state to initialise – all methods are static.
Parameters: None
Return: None
*/
FileOperations::FileOperations()
{
}

/*
Function: ~FileOperations
Description: Destructor.  No resources to release.
Parameters: None
Return: None
*/
FileOperations::~FileOperations()
{
}

// ---------------------------------------------------------------------------
// File-system operations
// ---------------------------------------------------------------------------

/*
Function: Open
Description: Opens the given path with the operating system's default
             application.  Uses wxLaunchDefaultApplication for files and
             directories, which delegates to the OS-specific handler.
Parameters: path - full path to the file or directory to open
Return: true if the launch succeeded
*/
bool FileOperations::Open(const wxString& path)
{
    // wxLaunchDefaultApplication works for both files and directories on macOS.
    return wxLaunchDefaultApplication(path);
}

/*
Function: CreateDirectory
Description: Creates a new directory at the specified path.  Fails if the
             path already exists or if the parent directory does not exist.
Parameters: path - full path of the directory to create
Return: true if the directory was created successfully
*/
bool FileOperations::CreateDirectory(const wxString& path)
{
    try
    {
        return create_directory(path.ToStdString());
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}

/*
Function: Rename
Description: Renames (or moves within the same parent) a file or directory.
             Wraps std::filesystem::rename.
Parameters: oldPath - current full path of the item
            newPath - desired full path after rename
Return: true if the rename succeeded
*/
bool FileOperations::Rename(const wxString& oldPath, const wxString& newPath)
{
    try
    {
        rename(oldPath.ToStdString(), newPath.ToStdString());
        return true;
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}

/*
Function: Delete
Description: Deletes a file or directory.  For directories the removal is
             recursive (all contents are deleted first).  Uses
             std::filesystem::remove_all for both cases for simplicity.
Parameters: path - full path of the item to delete
Return: true if the item was removed successfully
*/
bool FileOperations::Delete(const wxString& path)
{
    try
    {
        remove_all(path.ToStdString());
        return true;
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}

/*
Function: Copy
Description: Copies a file or directory to a destination path.  Directories
             are copied recursively.  If overwrite is true and the destination
             already exists it is replaced; otherwise the call fails when the
             destination exists.
Parameters: src       - full source path
            dest      - full destination path
            overwrite - if true, replace an existing destination
Return: true if the copy completed successfully
*/
bool FileOperations::Copy(const wxString& src, const wxString& dest, bool overwrite)
{
    try
    {
        copy_options opts = copy_options::none;
        if (overwrite)
        {
            opts = copy_options::overwrite_existing | copy_options::update_existing;
        }

        path srcPath(src.ToStdString());
        if (is_directory(srcPath))
        {
            opts |= copy_options::recursive;
        }

        copy(srcPath, path(dest.ToStdString()), opts);
        return true;
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}

/*
Function: Move
Description: Moves a file or directory to a destination path.  If overwrite
             is true and the destination already exists it is deleted first
             (required because std::filesystem::rename will fail on some
             platforms when the target exists).
Parameters: src       - full source path
            dest      - full destination path
            overwrite - if true, remove an existing destination before moving
Return: true if the move completed successfully
*/
bool FileOperations::Move(const wxString& src, const wxString& dest, bool overwrite)
{
    try
    {
        if (overwrite && exists(path(dest.ToStdString())))
        {
            remove_all(path(dest.ToStdString()));
        }

        rename(path(src.ToStdString()), path(dest.ToStdString()));
        return true;
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}

/*
Function: Exists
Description: Checks whether anything (file or directory) exists at the
             given path.
Parameters: path - full path to check
Return: true if the path exists on the filesystem
*/
bool FileOperations::Exists(const wxString& path)
{
    try
    {
        return exists(std::filesystem::path(path.ToStdString()));
    }
    catch (const filesystem_error&)
    {
        return false;
    }
}