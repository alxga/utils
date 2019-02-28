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

  void mergeDistinctRoots(int root1, int root2);
  inline void mergeSafe(int e1, int e2)
  {
    int root1 = find(e1);
    int root2 = find(e2);
    if (root1 != root2)
      mergeDistinctRoots(root1, root2);
  }

  inline int count() const { return m_count; }
  int countDistinct();
};

#endif // UNIONFND_HEADER_FILE_INCLUDED
