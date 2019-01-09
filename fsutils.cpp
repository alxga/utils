/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/fsutils.h"

using namespace std;


#ifdef WINDOWS
  static OSVERSIONINFO *WinVersion()
  {
    static OSVERSIONINFO osvi;
    static bool done = false;
    if (!done)
    {
      ::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
      osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
      if(!::GetVersionEx(&osvi))
        return 0;
      done = true;
    }
    return &osvi;
  }
#endif

namespace FS
{
  bool isPathAbsolute(const char *path)
  {
#ifdef WINDOWS
    return path && strchr(path, ':');
#else
    return path && *path == '/';
#endif
  }

  bool fsEntryExist(const char *path)
  {
#ifdef WINDOWS
    OSVERSIONINFO *osvi = WinVersion();
    if (osvi && osvi->dwMajorVersion >= 5)
      return PathFileExistsA(path) == TRUE;
    else
      return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat buffer;
    return !stat(path, &buffer);
#endif
  }

  const char *curDir()
  {
    static char ret[MAXPATHLENGTH];
#ifdef WINDOWS
    _getcwd(ret, MAXPATHLENGTH);
#else
    getcwd(ret, MAXPATHLENGTH);
#endif
    return ret;
  }

#ifdef WINDOWS
  bool makeDir(const char *rootDir, const char *dirPath)
#else
  bool makeDir(const char *rootDir, const char *dirPath, int imode)
#endif
  {
    if (!rootDir || !*rootDir || !isPathAbsolute(rootDir) ||
        !dirPath || !*dirPath)
      return false;

#ifndef WINDOWS
    mode_t mode = imode == -1 ? S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH : (mode_t) imode;

    char originalCWD[MAXPATHLENGTH];
    if (!getcwd(originalCWD, MAXPATHLENGTH))
      return false;

    char path[MAXPATHLENGTH];
    strcpy(path, dirPath);
    
    if (chdir(rootDir))
      return false;

    int lastPS = -1;
    for (int i = 0; ; i++)
    {
      if (isPathSep(path[i]) || !path[i])
      {
        if (i - lastPS > 1)
        {
          char tmp = path[i];
          path[i] = 0;
          char *ptr = path + lastPS + 1;
          if (chdir(ptr) &&
              (mkdir(ptr, mode) ||
               chdir(ptr)))
          {
            chdir(originalCWD);
            return false;
          }
          path[i] = tmp;
        }
        lastPS = i;
        if (!path[i])
          break;
      }
    }
    chdir(originalCWD);
#else
    const char *path = dirPath;

    char p[MAXPATHLENGTH];
    strcpy(p, rootDir);

    size_t len = strlen(p);
    if (!isPathSep(p[len - 1]))
      p[len++] = pathSep();

    for (int i = 0; ; i++)
    {
      if (isPathSep(path[i]) || !path[i])
      {
        if (isPathSep(p[len - 1]))
        {
          if (!path[i])
            break;
          else
            continue;
        }
        p[len] = 0;
        if (!CreateDirectoryA(p, 0) && GetLastError() != ERROR_ALREADY_EXISTS)
          return false;

        p[len++] = pathSep();
        if (!path[i])
          break;
      }
      else
        p[len++] = path[i];
    }
#endif

    return true;
  }

  bool listDir(const char *dirPath, std::vector<Entry> &list)
  {
    list.clear();

#ifndef WINDOWS
    DIR *hDir = opendir(dirPath);
    if (hDir == NULL)
      return false;

    struct dirent *de;
    while (de = readdir(hDir))
    {
      bool isDir;
#  ifdef _DIRENT_HAVE_D_TYPE
      if (de->d_type != DT_UNKNOWN && de->d_type != DT_LNK)
        // don't have to stat if we have d_type info,
        // unless it's a symlink (since we stat, not lstat)
        isDir = (de->d_type == DT_DIR);
      else
#  endif
      {
        // the only method if d_type isn't available, otherwise
        // this is a fallback for FSes where the kernel leaves it DT_UNKNOWN
        struct stat stbuf;
        // stat follows symlinks, lstat doesn't.
        stat(de->d_name, &stbuf);
        isDir = S_ISDIR(stbuf.st_mode);
      }
      closedir(hDir);
    }
#else
    char path[MAXPATHLENGTH];
    sprintf(path, "%s\\*", dirPath);

    WIN32_FIND_DATAA FFData;
    HANDLE hFind;
    hFind = FindFirstFileA(path, &FFData);
    if (hFind == INVALID_HANDLE_VALUE)
      // return true if no files were found as false indicates an error
      return GetLastError() == ERROR_FILE_NOT_FOUND;
    else 
    {
      char *ptr = FFData.cFileName;
      bool isDir = (FFData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
      // skip "." and ".."
      if (ptr[0] != '.' || (ptr[1] != 0 && (ptr[1] != '.' || ptr[2] != 0)))
        list.push_back(Entry(ptr, isDir));

      while (FindNextFileA(hFind, &FFData))
      {
        ptr = FFData.cFileName;
        isDir = (FFData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
        // skip "." and ".."
        if (ptr[0] != '.' || (ptr[1] != 0 && (ptr[1] != '.' || ptr[2] != 0)))
          list.push_back(Entry(ptr, isDir));
      }

      FindClose(hFind);
    }
#endif
  }
}
