/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"

#include <cmath>

#include "Utils/math.h"


#ifdef WINDOWS
#  ifdef HAVE_BOOST
#    define ACOSH_BOOST
#  else
#    define ACOSH_THROW
#  endif
#else
#  define ACOSH_CMATH
#endif

#ifdef ACOSH_BOOST
#  include <boost/math/special_functions/acosh.hpp>
#endif

using namespace std;

namespace Utils
{
  double acosh(double v)
  {
#ifdef ACOSH_BOOST
    return boost::math::acosh(v);
#elif defined(ACOSH_THROW)
    throw Exception("Rebuild the Utils library with HAVE_BOOST macro "
                    "defined to use Utils::acosh");
#else
    return std::acosh(v);
#endif
  }
}
