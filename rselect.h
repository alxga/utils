/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"
#include "Utils/rand01.h"

#ifndef RSELECT_HEADER_FILE
#define RSELECT_HEADER_FILE

template<typename T>
  class ICdfIndexer
  {
  public:
    virtual ~ICdfIndexer() {}
    virtual int index(double cdf) = 0;
    virtual void reinit(int count, T **ptr) = 0;
  };

template<typename T>
  class UniformCdfIndexer : public ICdfIndexer<T>
  {
    int m_count;
  public:
    UniformCdfIndexer(int count) { m_count = count; }
    virtual int index(double cdf) { return (int) (m_count * cdf); }
    virtual void reinit(int count, T **ptr) { m_count = count; }
  };

template<typename T>
  class RandomSelector
  {
    T **m_ptr;
    int m_count;
    int m_selected;
    ICdfIndexer<T> *m_indexer;

  public:
    RandomSelector(T **ptr, int count, ICdfIndexer<T> *indexer)
    {
      m_ptr = ptr;
      m_count = count;
      m_selected = 0;
      if (indexer)
        m_indexer = indexer;
      else
        m_indexer = new UniformCdfIndexer<T>(m_count);
    }

    virtual ~RandomSelector()
    {
      delete [] m_ptr;
      delete m_indexer;
    }

    void resetCounters(int count)
    {
      m_count = count;
      m_selected = 0;
    }

    void add(T *x)
    {
      m_ptr[m_count++] = x;
    }

    inline int remains() const { return m_count - m_selected; }

    T *sel(bool (*chk)(T *) = 0)
    {
      if (m_count - m_selected < 1)
        return NULL;

      int ix = m_indexer->index(RAND_0_1);
      int attemptsCount = 10;
      while (m_ptr[ix] == NULL)
      {
        if (attemptsCount < 1)
        {
          ix = 0;
          for (int i = 0; i < m_count; i++)
            if (m_ptr[i] != NULL)
              m_ptr[ix ++] = m_ptr[i];
          m_count -= m_selected;
          m_indexer->reinit(m_count, m_ptr);
          m_selected = 0;
          attemptsCount = 10;
        }
        ix = (int) (m_count * RAND_0_1);
        attemptsCount --;
      }
      T *ret = m_ptr[ix];
      if (chk == NULL || chk(ret))
      {
        m_ptr[ix] = NULL;
        m_selected ++;
      }
      return ret;
    }
  };

#endif // RSELECT_HEADER_FILE
