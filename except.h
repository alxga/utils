/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef EXCEPT_HEADER_FILE_INCLUDED
#define EXCEPT_HEADER_FILE_INCLUDED

#define L_DEBUG_EXCEPTIONS
#ifdef L_DEBUG_EXCEPTIONS
#  define EX_DECL       const char *_callerSrcFile, int _callerSrcLine,
#  define EX_PASS       _callerSrcFile, _callerSrcLine,
#  define EX_CALL       __FILE__, __LINE__,
#  define EX_VDECL      const char *_callerSrcFile, int _callerSrcLine
#  define EX_VPASS      _callerSrcFile, _callerSrcLine
#  define EX_VCALL      __FILE__, __LINE__
#else
#  define EX_DECL
#  define EX_PASS
#  define EX_PASS
#  define EX_VDECL
#  define EX_VPASS
#  define EX_VCALL
#endif

class LIBUTILS_API Exception
{
  char *m_msg;

public:
  Exception(const char *msg);
  virtual ~Exception();

  inline const char *message() const { return m_msg; }
};

class LIBUTILS_API SystemException : public Exception
{
protected:
  int m_sysError;

public:
  SystemException(const char *v, int errCode);

  inline int sysError() const { return m_sysError; }
};

#endif // EXCEPT_HEADER_FILE_INCLUDED
