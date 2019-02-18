/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef MSECCNT_HEADER_FILE_INCLUDED
#define MSECCNT_HEADER_FILE_INCLUDED

#ifndef WINDOWS
struct timespec;
#endif

class LIBUTILS_API MSecCounter
{
#ifdef WINDOWS
  unsigned int m_start;
#else
  struct timespec *m_start;
#endif

public:
  MSecCounter();
  virtual ~MSecCounter();

  void reinit();
  unsigned int elapsed() const;
};

#endif // MSECCNT_HEADER_FILE_INCLUDED
