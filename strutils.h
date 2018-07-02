/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cctype>
#include <algorithm>
#include "Utils/libutils.h"

#ifndef STRUTILS_HEADER_FILE_INCLUDED
#define STRUTILS_HEADER_FILE_INCLUDED

#define MAXTEXTLENGTH 4096

#ifdef WINDOWS
#  define ENDL "\n\r"
#else
#  define ENDL "\n"
#endif

#ifndef WINDOWS
#  define _stricmp strcasecmp
#endif

typedef std::list<std::string> StringList;
typedef std::vector<std::string> StringVector;

#define ToString(arg) \
  static_cast<std::ostringstream &>( \
    (std::ostringstream() << arg)).str()

static inline void LTrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
          std::not1(std::ptr_fun<int, int>(std::isspace))));
}

static inline void RTrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(),
          std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
          s.end());
}

static inline void Trim(std::string &s) {
    LTrim(s);
    RTrim(s);
}


template<typename T>
  void SplitStr(const std::string &str, T &tokens,
                const std::string &delimiters = " ",
                bool trimEmpty = false)
  {
    std::string::size_type pos, lastPos = 0, length = str.length();

    while (lastPos < length + 1)
    {
      pos = str.find_first_of(delimiters, lastPos);
      if (pos == std::string::npos)
        pos = length;

      if (pos != lastPos || !trimEmpty)
        tokens.push_back(typename T::value_type(str.data() + lastPos,
                                                (typename T::size_type)pos -
                                                lastPos));

      lastPos = pos + 1;
    }
  }

int LIBUTILS_API FindCaseInsensitiveStr(const std::string &str,
                                        StringVector &vect);

template<typename T>
  void OutputContainer(std::ostream &os, T &cont, const char *sep)
  {
    for (size_t i = 0; i < cont.size(); i++)
    {
      if (i > 0)
        os << sep;
      os << cont[i];
    }
  }
#endif // STRUTILS_HEADER_FILE_INCLUDED
