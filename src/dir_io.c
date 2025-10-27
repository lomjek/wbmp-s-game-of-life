#include "headers/dir_io.h"

#ifdef _WIN32 //Widows
    #include <windows.h>
#else // POSIX includes (Linux, macOS, Unix)
    #include <sys/stat.h>
#endif

bool is_valid_dir(const char *path) {
    if (path == NULL || strlen(path) == 0) {
        return false;
    }

#ifdef _WIN32
    // Windows implementation using GetFileAttributesA
    DWORD attributes = GetFileAttributesA(path);

    // Check if the file/dir exists and if it is a directory
    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    return false;
#else
    // POSIX implementation
    struct stat st;

    // stat returns 0 on success, -1 on failure (e.g., path not found)
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode); // Check if the path is a directory
    }
    return false;
#endif
}

bool create_dir(const char *path) {
    if (path == NULL || strlen(path) == 0) {
        return false;
    }
    
#ifdef _WIN32
    // Windows implementation using CreateDirectoryA
    BOOL result = CreateDirectoryA(path, NULL);

    if (result) {
        return true;
    } else {
        DWORD error = GetLastError();
        // If the error is that it already exists, do nothing and verify it's a directory
        if (error == ERROR_ALREADY_EXISTS) {
            return is_valid_dir(path);
        }
    }
    return false; // Creation failed for other reasons
        
#else
    // POSIX implementation using mkdir
    // 0775 grants rwx for owner, group, and others
    if (mkdir(path, 0777) == 0) {
        return true;
    } else {
        return is_valid_dir(path); //If the dir already exists, return true
    }
    return false; // Creation failed for other reasons
#endif
}
