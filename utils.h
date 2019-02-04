/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/strutils.h"
#include "Utils/fsutils.h"
#include "Utils/except.h"
#include "Utils/dpoint.h"
#include "Utils/app.h"
#include "Utils/mpimsg.h"
#include "Utils/rand01.h"
#include "Utils/rselect.h"
#include "Utils/distr.h"


#ifndef UTILS_HEADER_FILE_INCLUDED
#define UTILS_HEADER_FILE_INCLUDED


void LIBUTILS_API DelayThread(int msec);



template<typename T>
  class auto_del
  {
    T *m_ptr;
    bool m_isarr;

  public:
    auto_del(T *ptr, bool isarr) : m_isarr(isarr)
    {
      m_ptr = ptr;
    }

    virtual ~auto_del()
    {
      if (m_ptr != NULL)
      {
        if (m_isarr)
          delete [] m_ptr;
        else
          delete m_ptr;
        m_ptr = NULL;
      }
    }

    inline void setPtr(T *ptr) { m_ptr = ptr; }
  };

template<typename T>
  class auto_del_container
  {
    T *m_container;
    bool m_isarr;

  public:
    auto_del_container(T *container, bool isarr) : m_isarr(isarr)
    {
      m_container = container;
    }

    virtual ~auto_del_container()
    {
      if (m_container != NULL)
      {
        for (typename T::iterator it = m_container->begin();
             it != m_container->end(); it++)
          if (m_isarr)
            delete [] *it;
          else
            delete *it;
        m_container = NULL;
      }
    }
  };

#endif // UTILS_HEADER_FILE_INCLUDED
