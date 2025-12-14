#include "headers/file_io.h"

#if defined(_WIN32) || defined(_WIN64) // Windows Implementation
    #include <windows.h>

#else // Linux/MacOS Implementation
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <errno.h>
#endif

bool is_valid_dir(const char *path) {
    if (path == NULL || strlen(path) == 0) {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64) // Windows Implementation
    DWORD attributes = GetFileAttributesA(path);

    if (attributes != INVALID_FILE_ATTRIBUTES) {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    return false;

#else // Linux/MacOS Implementation
    struct stat st;

    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
#endif
}

bool create_dir(const char *path) {
    if (path == NULL || strlen(path) == 0) {
        return false;
    }
    
#if defined(_WIN32) || defined(_WIN64) // Windows Implementation
    BOOL result = CreateDirectoryA(path, NULL);

    if (result) {
        return true;
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            return is_valid_dir(path);
        }
    }
    return false;
        
#else // Linux/MacOS Implementation
    if (mkdir(path, 0775) == 0) {
        return true;
    } else {
        return is_valid_dir(path);
    }
    return false;
#endif
}

bool file_exists(const char *filename) {
    FILE *file;
    if ((file = fopen(filename, "r"))) {
        fclose(file);
        return true;
    }
    return false;
}

bool is_dir_empty(const char *path) {
#if defined(_WIN32) || defined(_WIN64) // Windows Implementation
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    int file_count = 0;
    bool is_empty = true;

    char search_path[MAX_PATH];
    strncpy(search_path, path, MAX_PATH - 3);
    search_path[MAX_PATH - 3] = '\0';
    strncat(search_path, "\\*", MAX_PATH - strlen(search_path) - 1);

    hFind = FindFirstFile(search_path, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            is_empty = false;
            break;
        }
    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);
    return is_empty;

#else // Linux/MacOS Implementation
    DIR *dir;
    struct dirent *entry;
    int file_count = 0;

    dir = opendir(path);
    if (!is_valid_dir(path)) {
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            closedir(dir);
            return false;
        }
    }

    closedir(dir);
    return true;
#endif
}