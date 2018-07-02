/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef RAND01_HEADER_FILE
#define RAND01_HEADER_FILE

class LIBUTILS_API Rand01
{
public:
  static double generate();
};

#define RAND_0_1 Rand01::generate()

#endif // RAND01_HEADER_FILE
