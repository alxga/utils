#include <string>
#include <map>

#include "Utils/strutils.h"

#ifndef CMDARG_HEADER_FILE_INCLUDED
#define CMDARG_HEADER_FILE_INCLUDED

struct CmdArg
{
  std::string m_name;
  std::string m_value;
  double m_num;

  CmdArg()
  {
    m_num = 0;
  }

  CmdArg(const std::string &name, const std::string &value) :
    m_name(name), m_value(value)
  {
    if (sscanf(m_value.c_str(), "%lf", &m_num) != 1)
      m_num = 0;
  }

  void parse(char *str)
  {
    while (*str == '-')
      str++;
    char* eqPos = strchr(str, '=');
    if (eqPos == NULL)
    {
      m_name = str;
      m_value = "";
    }
    else
    {
      *eqPos = 0;
      m_name = str;
      m_value = eqPos + 1;
    }

    Trim(m_name);
    Trim(m_value);

    if (sscanf(m_value.c_str(), "%lf", &m_num) != 1)
      m_num = 0;
  }
};

typedef std::map<std::string, CmdArg> StrCmdArgMap;

#endif // CMDARG_HEADER_FILE_INCLUDED
