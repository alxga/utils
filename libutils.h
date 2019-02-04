/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#ifndef LIBUTILS_HEADER_FILE_INCLUDED
#define LIBUTILS_HEADER_FILE_INCLUDED

#if defined(WIN32) || defined(WIN64)
#  define WINDOWS
#endif

#ifdef WINDOWS
#  ifdef LIBUTILS_EXPORTS
#    define LIBUTILS_API __declspec(dllexport)
#  else
#    define LIBUTILS_API __declspec(dllimport)
#  endif
#else
#  define LIBUTILS_API
#endif

#endif // LIBUTILS_HEADER_FILE_INCLUDED
