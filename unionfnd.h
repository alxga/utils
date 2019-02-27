/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "Utils/libutils.h"

#ifndef UNIONFND_HEADER_FILE_INCLUDED
#define UNIONFND_HEADER_FILE_INCLUDED

struct UnionFindNode;

class LIBUTILS_API UnionFind
{
  UnionFindNode* m_arr;
  int m_count;

public:
  UnionFind(int count);
  virtual ~UnionFind();

  int find(int id);

  void merge(int e1, int e2);
};

#endif // UNIONFND_HEADER_FILE_INCLUDED
