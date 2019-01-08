/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/utils.h"

#ifndef MATRIX_HEADER_FILE_INCLUDED
#define MATRIX_HEADER_FILE_INCLUDED

template<typename T>
  class Matrix
  {
    int m_n; // number of rows
    int m_m; // number of columns
    int m_sz; // total number of elements
    T *m_data; // values

  public:
    Matrix()
    {
      m_n = m_m = 0;
      m_sz = 0;
      m_data = NULL;
    }

    Matrix(const Matrix &v) : m_m(v.m_m), m_n(v.m_n)
    {
      m_data = NULL;
      reinit(v.m_n, v.m_m);
      memcpy(m_data, v.m_data, m_sz * sizeof(T));
    }

    Matrix(int n, int m)
    {
      m_data = NULL;
      reinit(n, m);
    }

    void reinit(int n, int m)
    {
      delete [] m_data;
      m_n = n;
      m_m = m;
      m_sz = m_n * m_m;
      m_data = m_sz > 0 ? new T [m_sz] : NULL;
    }

    virtual ~Matrix()
    {
      delete [] m_data;
    }

    inline int n() const { return m_n; }
    inline int m() const { return m_m; }

    inline T &operator() (int i, int j)
    {
      return m_data[i * m_m + j];
    }

    inline T *row(int i)
    {
      return m_data + i * m_m;
    }

    const Matrix<T> &operator= (const Matrix<T> &v)
    {
      if (m_n != v.m_n || m_m != v.m_m)
        throw Exception("Matrix sizes must be equal for this operation");
      memcpy(m_data, v.m_data, m_sz * sizeof(T));
    }

    void zero()
    {
      memset(m_data, 0, m_sz * sizeof(T));
    }

    void set(const T &val)
    {
      for (int i = 0; i < m_sz; i++)
        m_data[i] = val;
    }
  };

#endif // MATRIX_HEADER_FILE_INCLUDED
