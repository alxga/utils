/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/unionfnd.h"
#include "Utils/utils.h"


struct UnionFindNode
{
  int m_rootId;
  int m_rank;
  UnionFindNode() : m_rootId(0), m_rank(0)
  {
  }
  UnionFindNode(int rootId, int rank) : m_rootId(rootId), m_rank(rank)
  {
  }
};


UnionFind::UnionFind(int count)
{
  m_arr = new UnionFindNode [count];
  m_count = count;
  for (int i = 0; i < m_count; i++)
    m_arr[i].m_rootId = i;
}

UnionFind::~UnionFind()
{
  delete[] m_arr;
}

int UnionFind::find(int id)
{
  int rootId = id;
  while (rootId != m_arr[rootId].m_rootId)
    rootId = m_arr[rootId].m_rootId;
  m_arr[id].m_rootId = rootId;
  return rootId;
}

void UnionFind::mergeDistinctRoots(int root1, int root2)
{
  UnionFindNode *n1 = &m_arr[root1];
  UnionFindNode *n2 = &m_arr[root2];
  if (n1->m_rank > n2->m_rank)
    n2->m_rootId = n1->m_rootId;
  else if (n1->m_rank < n2->m_rank)
    n1->m_rootId = n2->m_rootId;
  else
  {
    n1->m_rootId = n2->m_rootId;
    n2->m_rank ++;
  }
}

int UnionFind::countDistinct()
{
  bool *checks = new bool[m_count];
  auto_del<bool> del_checks(checks, true);
  memset(checks, 0, m_count * sizeof(bool));
  int ret = 0;
  for (int i = 0; i < m_count; i++)
  {
    int rootId = find(i);
    if (!checks[rootId])
    {
      checks[rootId] = true;
      ret++;
    }
  }
  return ret;
}

