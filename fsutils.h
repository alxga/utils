/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/
#include <vector>

#include "Utils/libutils.h"

#ifndef FSUTILS_HEADER_FILE_INCLUDED
#define FSUTILS_HEADER_FILE_INCLUDED


#define MAXPATHLENGTH 4096


namespace FS
{
  bool LIBUTILS_API isPathAbsolute(const char *path);
  bool LIBUTILS_API fsEntryExist(const char *name);
  const char LIBUTILS_API *curDir();
#ifdef WINDOWS
  bool LIBUTILS_API makeDir(const char *rootDir, const char *dirPath);
#else
  bool LIBUTILS_API makeDir(const char *rootDir, const char *dirPath, int imode = -1);
#endif
  
  struct Entry
  {
    static const int MAXNAMELENGTH = 256;
    char Name[MAXNAMELENGTH];
    bool IsDir;

    Entry(const char *name, bool isDir);
    Entry(const Entry &v);
  };
  bool LIBUTILS_API listDir(const char *dirPath, std::vector<Entry> &list);

  inline bool LIBUTILS_API isPathSep(int sym)
  {
#ifdef WINDOWS
    return sym == '\\' || sym == '/';
#else
    return sym == '/';
#endif
  }

  inline char LIBUTILS_API pathSep()
  {
#ifdef WINDOWS
    return '\\';
#else
    return '/';
#endif
  }
};

class LIBUTILS_API auto_file
{
  FILE *m_file;

public:
  auto_file(FILE *f)
  {
    m_file = f;
  }
  virtual ~auto_file()
  {
    if (m_file != NULL)
    {
      fclose(m_file);
      m_file = NULL;
    }
  }
};

#endif // FSUTILS_HEADER_FILE_INCLUDED
