/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/utils.h"


void DelayThread(int millisec)
{
#ifdef WINDOWS
  Sleep(millisec);
#else
  struct timeval tv = { millisec / 1000, 1000 * (millisec % 1000) };
  select (0, 0, 0, 0, &tv);
#endif
}
