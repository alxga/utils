// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#include "Utils/libutils.h"

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS

#include <stdio.h>
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <iterator>
#include <random>
#include <algorithm>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#ifdef WINDOWS
#  include "Utils/targetver.h"

#  define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#  include <windows.h>
#  include <direct.h>
#  include "shlwapi.h"
#else
#  include <string.h>
#  include <unistd.h>
#  include <dirent.h>
#  include <sys/stat.h>
#  include <time.h>
#endif
