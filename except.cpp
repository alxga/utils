/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/except.h"
#include "Utils/app.h"

Exception::Exception(const char *msg)
{
  App *app = App::app();
  if (app != NULL)
    app->log("New Exception: %s", msg);
  size_t len = strlen(msg);
  m_msg = new char [len + 1];
  strcpy(m_msg, msg);
}

Exception::~Exception()
{
  delete [] m_msg;
}


SystemException::SystemException(const char *msg, int errCode) :
  Exception(msg)
{
  m_sysError = errCode;
}
