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
}
