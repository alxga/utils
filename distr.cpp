/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"

#include "Utils/distr.h"

#include "Utils/except.h"

#ifdef HAVE_BOOST

#include <boost/version.hpp>
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/binomial_distribution.hpp>
struct BoostBinDistrInternal
{
  boost::mt19937 m_eng;
  boost::random::binomial_distribution<> m_dist;
  boost::variate_generator<boost::mt19937&, boost::random::binomial_distribution<>> m_var;

  BoostBinDistrInternal(int n, double p) :
    m_eng(((unsigned int) time(NULL)) * rand()),
    m_dist(n, p),
    m_var(m_eng, m_dist)
  {
  }
};

#else

struct BoostBinDistrInternal
{
  BoostBinDistrInternal(int n, double p)
  {
    throw Exception("Rebuild the Utils library with HAVE_BOOST macro "
                    "defined to use BoostBinDistr");
  }

  inline int m_var() const
  {
    throw Exception("Rebuild the Utils library with HAVE_BOOST macro "
                    "defined to use BoostBinDistr");
  }
};

#endif

BoostBinDistr::BoostBinDistr(int n, double p)
{
  m_internal = new BoostBinDistrInternal(n, p);
}

BoostBinDistr::~BoostBinDistr()
{
  delete m_internal;
}

int BoostBinDistr::genInt()
{
  return m_internal->m_var();
}
