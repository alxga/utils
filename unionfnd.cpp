/*
  Copyright (c) 2018-2019 Alexander A. Ganin. All rights reserved.
  Twitter: @alxga. Website: alexganin.com.
  Licensed under the MIT License.
  See LICENSE file in the project root for full license information.
*/

#include "stdafx.h"
#include "Utils/unionfnd.h"


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
  int tc = 0;
  int temp[1024];

  int rootId = id;
  while (rootId != m_arr[rootId].m_rootId)
  {
    temp[tc++] = rootId;
    rootId = m_arr[rootId].m_rootId;
  }
  for (int i = 0; i < tc - 1; i++)
    m_arr[temp[i]].m_rootId = rootId;
  return rootId;
}

void UnionFind::merge(int e1, int e2)
{
  UnionFindNode *n1 = &m_arr[e1];
  UnionFindNode *n2 = &m_arr[e2];
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
