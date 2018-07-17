/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/except.h"
#include "Utils/app.h"

Exception::Exception(const char *str, ...)
{
  va_list aptr;
  va_start(aptr, str);
  vsprintf(m_msg, str, aptr);
  va_end(aptr);

  App *app = App::app();
  if (app != NULL)
    app->log("New Exception: %s\n", m_msg);
}

Exception::~Exception()
{
}


SystemException::SystemException(const char *msg, int errCode) :
  Exception(msg)
{
  m_sysError = errCode;
}
