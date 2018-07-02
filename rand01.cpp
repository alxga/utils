/*
  Copyright (c) 2018 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/rand01.h"

#ifdef HAVE_BOOST

#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
using namespace boost;
using namespace boost::random;

static double boost_rand_0_1()
{
  static mt19937 eng(((unsigned int) time(NULL)) * rand());
  static uniform_01<> dist;
  static variate_generator<mt19937 &, uniform_01<>> var(eng, dist);
  return var();
}

#endif


// Minimal random number generator of Park and Miller with Bays-Durham shuffle and added
// safeguards. Returns a uniform random deviate between 0.0 and 1.0 (exclusive of the endpoint
// values). Call with idum a negative integer to initialize; thereafter, do not alter idum between
// successive deviates in a sequence. RNMX should approximate the largest floating value that is
// less than 1

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

static double park_miller_rand_0_1(long *idum)
{
  int j;
  long k;
  static long iy = 0;
  static long iv[NTAB];
  double temp;

  if (*idum <= 0 || !iy)
  {
    // Initialize
    if (-(*idum) < 1)
      *idum = 1; // Be sure to prevent idum = 0
    else
      *idum = -(*idum);

    for (j = NTAB + 7; j >= 0; j--)
    { // Load the shuffle table (after 8 warm-ups)
      k = (*idum) / IQ;
      *idum = IA * (*idum - k * IQ) - IR * k;
      if (*idum < 0) *idum += IM;
      if (j < NTAB) iv[j] = *idum;
    }
    iy = iv[0];
  }

  k = (*idum) / IQ;  // Start here when not initializing
  *idum = IA * (*idum - k * IQ) - IR*k; // Compute idum = (IA * idum) % IM without overlows by Schrage's method
  if (*idum < 0)
    *idum += IM;

  j = iy / NDIV;  // Will be in the range 0 ... NTAB-1
  iy = iv[j]; // Output previously stored value and refill the  shuffle table
  iv[j] = *idum;
  if ((temp = AM * iy) > RNMX)
    return RNMX; // Because users don't expect endpoint values
  else
    return temp;
}


double Rand01::generate()
{
  // return boost_rand_0_1();
  static long seed = -rand();
  return park_miller_rand_0_1(&seed);
}
