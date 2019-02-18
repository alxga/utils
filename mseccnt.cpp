/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/mseccnt.h"


#ifndef WINDOWS

#define TSPEC2DBLMS(_ts) \
  (1000.0 * (_ts).tv_sec + (_ts).tv_nsec / 1000000.0)

#endif

MSecCounter::MSecCounter()
{
#ifndef WINDOWS
  m_start = new struct timespec();
#endif
  reinit();
}

MSecCounter::~MSecCounter()
{
#ifndef WINDOWS
  delete m_start;
#endif
}

void MSecCounter::reinit()
{
#ifdef WINDOWS
  m_start = (unsigned int)GetTickCount();
#else
  clock_gettime(CLOCK_MONOTONIC, m_start);
#endif
}

unsigned int MSecCounter::elapsed() const
{
#ifdef WINDOWS
  return (unsigned int) (GetTickCount() - m_start);
#else
  struct timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);
  return (unsigned int) (TSPEC2DBLMS(*start) - TSPEC2DBLMS(end));
#endif
}
