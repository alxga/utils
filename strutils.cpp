/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/strutils.h"

using namespace std;

int FindCaseInsensitiveStr(const std::string &str,
                           StringVector &vect)
{
  for (size_t i = 0; i < vect.size(); i++)
    if (_stricmp(str.data(), vect[i].data()) == 0)
      return (int)i;
  return -1;
}
