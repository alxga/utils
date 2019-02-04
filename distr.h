/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef DISTR_HEADER_FILE_INCLUDED
#define DISTR_HEADER_FILE_INCLUDED


class LIBUTILS_API IDoubleDistr
{
public:
  virtual double genDouble() = 0;
};

class LIBUTILS_API IIntDistr : public IDoubleDistr
{
public:
  virtual int genInt() = 0;
  virtual double genDouble() { return genInt(); }
};


struct BoostBinDistrInternal;
class LIBUTILS_API BoostBinDistr : public IIntDistr
{
  BoostBinDistrInternal *m_internal;

public:
  BoostBinDistr(int n, double p);
  virtual ~BoostBinDistr();
  virtual int genInt();
};


class LIBUTILS_API RegularDistr : public IDoubleDistr
{
  double m_value;

public:
  RegularDistr(double k) : m_value(k)
  {
  }
  virtual ~RegularDistr() {}
  virtual inline double genDouble() { return m_value; }
};

#endif // DISTR_HEADER_FILE_INCLUDED
