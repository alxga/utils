/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/dpoint.h"

#define _USE_MATH_DEFINES
#include <math.h>


const unsigned int DPoint::EARTH_R = 6371000;

double DPoint::distance2D(const DPoint &v) const
{
  const double dx = v.m_x - m_x;
  const double dy = v.m_y - m_y;
  return std::sqrt((double) (dx * dx + dy * dy));
}

double DPoint::distanceWGS84(const DPoint &v) const
{
  const double lat1 = M_PI * m_y / 180;
  const double lng1 = M_PI * m_x / 180;
  const double lat2 = M_PI * v.m_y / 180;
  const double lng2 = M_PI * v.m_x / 180;
  const double dlat2 = (lat1 - lat2) / 2;
  const double dlng2 = (lng1 - lng2) / 2;
  const double sindlat2 = sin(dlat2);
  const double sindlng2 = sin(dlng2);
  const double a = sindlat2 * sindlat2 +
                   cos(lat1) * cos(lat2) * sindlng2 * sindlng2;
  const double c = atan2(sqrt(a), sqrt(1.0 - a));
  return c * EARTH_R;
}
