/*
Author: Guo Jia
Description: Declaration of FileOperations – a stateless utility class whose
             static methods wrap std::filesystem calls for open, mkdir, rename,
             delete, copy, and move.
Date: 2026-02-02
*/

#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <wx/string.h>

class FileOperations
{
public:
    FileOperations();
    virtual ~FileOperations();

    // Open a file or directory with the platform's default application.
    // Returns true on success.
    static bool Open(const wxString& path);

    // Create a new directory at the given full path.
    // Returns true on success.
    static bool CreateDirectory(const wxString& path);

    // Rename (move within the same parent) a file or directory.
    // Returns true on success.
    static bool Rename(const wxString& oldPath, const wxString& newPath);

    // Delete a single file or directory (recursive for directories).
    // Returns true on success.
    static bool Delete(const wxString& path);

    // Copy a file or directory to a destination path.
    // If overwrite is true an existing destination is replaced.
    // Returns true on success.
    static bool Copy(const wxString& src, const wxString& dest, bool overwrite);

    // Move a file or directory to a destination path.
    // If overwrite is true an existing destination is replaced.
    // Returns true on success.
    static bool Move(const wxString& src, const wxString& dest, bool overwrite);

    // Returns true if something already exists at the given path.
    static bool Exists(const wxString& path);

private:
};

#endif // FILEOPERATIONS_H