/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef DPOINT_HEADER_FILE_INCLUDED
#define DPOINT_HEADER_FILE_INCLUDED

class LIBUTILS_API DPoint
{
protected:
  double m_x;
  double m_y;

public:
  static const unsigned int EARTH_R;

  DPoint() : m_x(0), m_y(0)
  {
  }

  DPoint(int x, int y) : m_x(x), m_y(y)
  {
  }

  DPoint(const DPoint &v) : m_x(v.m_x), m_y(v.m_y)
  {
  }

  const DPoint &operator= (const DPoint &v)
  {
    set(v.m_x, v.m_y);
    return *this;
  }

  bool operator== (const DPoint &v) const
  {
    return m_x == v.m_x && m_y == v.m_y;
  }

  bool operator< (const DPoint &v) const
  {
    if (m_y < v.m_y)
      return true;
    if (m_y > v.m_y)
      return false;
    return m_x < v.m_x;
  }

  bool operator> (const DPoint &v) const
  {
    if (m_y > v.m_y)
      return true;
    if (m_y < v.m_y)
      return false;
    return m_x > v.m_x;
  }

  inline void set(double x, double y)
  {
    m_x = x;
    m_y = y;
  }

  inline double x() const { return m_x; }
  inline void setX(double x) { m_x = x; }

  inline double y() const { return m_y; }
  inline void setY(double y) { m_y = y; }
  
  double distance2D(const DPoint &v) const;
  double distanceWGS84(const DPoint &v) const;

  inline double lat() const { return m_y; }
  inline void setLat(double v) { m_y = v; }

  inline double lng() const { return m_x; }
  inline void setLng(double v) { m_x = v; }
};

#endif // DPOINT_HEADER_FILE_INCLUDED
