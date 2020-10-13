// Copyright (C) 2022 Pieter Goetschalckx

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "util.h"
#include <string.h>

static Edge EDGES[MAXSIZE];
static Edge* FIRSTEDGE[MAXORDER];
static int DEG[MAXORDER];

int create_vertex(DoublePreDeco* dpd) {
  DEG[dpd->order] = 0;
  return dpd->order++;
}

void _increase_deg(DoublePreDeco* dpd, int vertex) {
  int i;
  switch (DEG[vertex]) {
    case 0:
      dpd->deg1[dpd->n1++] = vertex;
      break;
    case 1:
      for (i = 0; dpd->deg1[i] != vertex; i++)
        ;
      dpd->deg1[i] = dpd->deg1[--(dpd->n1)];
      dpd->deg2[dpd->n2++] = vertex;
      break;
    case 2:
      for (i = 0; dpd->deg2[i] != vertex; i++)
        ;
      dpd->deg2[i] = dpd->deg2[--(dpd->n2)];
      break;
  }
  DEG[vertex] += 1;
}

void _decrease_deg(DoublePreDeco* dpd, int vertex) {
  int i;
  switch (DEG[vertex]) {
    case 1:
      for (i = 0; dpd->deg1[i] != vertex; i++)
        ;
      dpd->deg1[i] = dpd->deg1[--(dpd->n1)];
      break;
    case 2:
      for (i = 0; dpd->deg2[i] != vertex; i++)
        ;
      dpd->deg2[i] = dpd->deg2[--(dpd->n2)];
      dpd->deg1[dpd->n1++] = vertex;
      break;
    case 3:
      dpd->deg2[dpd->n2++] = vertex;
      break;
  }
  DEG[vertex] -= 1;
}

Edge* create_edge(DoublePreDeco* dpd, int start, int end) {
  int i;

  Edge* edge = &EDGES[dpd->size++];
  Edge* inverse = &EDGES[dpd->size++];
  edge->inverse = inverse;
  inverse->inverse = edge;
  edge->start = inverse->end = start;
  edge->end = inverse->start = end;
  _increase_deg(dpd, start);
  _increase_deg(dpd, end);
  FIRSTEDGE[start] = edge;
  FIRSTEDGE[end] = inverse;
  return edge;
}

void set_next(Edge* edge, Edge* next) {
  edge->next = next;
  next->prev = edge;
}

void detach(DoublePreDeco* dpd, Edge* edge) {
  set_next(edge->prev, edge->next);
  if (FIRSTEDGE[edge->start] == edge) {
    FIRSTEDGE[edge->start] = edge->prev;
  }
  _decrease_deg(dpd, edge->start);
}

void attach(DoublePreDeco* dpd, Edge* prev, Edge* edge) {
  edge->start = edge->inverse->end = prev->start;
  _increase_deg(dpd, prev->start);
  set_next(edge, prev->next);
  set_next(prev, edge);
}

void remove_extension(DoublePreDeco* dpd) {
  int i, vertex = --(dpd->order);

  for (int i = 0; i < 4; i++) {
    Edge* edge = &EDGES[--(dpd->size)];
    if (edge->start != vertex)
      detach(dpd, edge);
  }

  switch (DEG[vertex]) {
    case 1:
      for (i = 0; dpd->deg1[i] != vertex; i++)
        ;
      dpd->deg1[i] = dpd->deg1[--(dpd->n1)];
      break;
    case 2:
      for (i = 0; dpd->deg2[i] != vertex; i++)
        ;
      dpd->deg2[i] = dpd->deg2[--(dpd->n2)];
      break;
  }
}

Edge* get_edge(int vertex) {
  return FIRSTEDGE[vertex];
}

int degree(int vertex) {
  return DEG[vertex];
}

void reset_vertex_marks(VertexMarks* marks, int range) {
  marks->unmarked += marks->range;
  if (marks->unmarked < marks->range) {
    memset(marks->marks, 0, sizeof(marks->marks));
    marks->unmarked = 0;
  }
  marks->range = range;
#ifdef DEBUG
  for (int i = 0; i < MAXORDER; i++)
    assert(marks->marks[i] <= marks->unmarked);
#endif
}

void mark_vertex(VertexMarks* marks, int vertex, int mark) {
  assert(mark <= marks->range);
  marks->marks[vertex] = marks->unmarked + mark;
}

int vertex_mark(VertexMarks* marks, int vertex) {
  return marks->marks[vertex] > marks->unmarked && marks->marks[vertex];
}

void reset_edge_marks(EdgeMarks* marks, int range) {
  marks->unmarked += marks->range;
  if (marks->unmarked < marks->range) {
    memset(marks->marks, 0, sizeof(marks->marks));
    marks->unmarked = 0;
  }
  marks->range = range;
#ifdef DEBUG
  for (int i = 0; i < MAXSIZE; i++)
    assert(marks->marks[i] <= marks->unmarked);
#endif
}

void mark_edge(EdgeMarks* marks, Edge* edge, int mark) {
  assert(mark <= marks->range);
  marks->marks[edge - EDGES] = marks->unmarked + mark;
}

int edge_mark(EdgeMarks* marks, Edge* edge) {
  return marks->marks[edge - EDGES] > marks->unmarked &&
         marks->marks[edge - EDGES];
}

static unsigned long long PRECOUNT = 0;

void precount(int n) {
  PRECOUNT += n;
}

unsigned long long get_precount() {
  return PRECOUNT;
}

static unsigned long long COUNT = 0;

void count(int n) {
  COUNT += n;
}

unsigned long long get_count() {
  return COUNT;
}

static int FILTER_LSP = 0;

void filter_lsp(int value) {
  FILTER_LSP = value;
}

int get_filter_lsp() {
  return FILTER_LSP;
}

#ifdef DEBUG

#include <stdio.h>

static EdgeMarks _CHECKMARKS;
static EdgeMarks* CHECKMARKS = &_CHECKMARKS;

void check(DoublePreDeco* dpd) {
  int i, vertex;
  Edge *edge, *temp;
  int size = 0, n1 = 0, n2 = 0;

  assert(dpd->order <= MAXORDER);
  assert(dpd->size <= MAXSIZE);

  for (vertex = 0; vertex < dpd->order; vertex++) {
    size += degree(vertex);
    if (degree(vertex) == 1)
      n1++;
    else if (degree(vertex) == 2)
      n2++;

    for (edge = get_edge(vertex), i = 0; i < degree(vertex);
         edge = edge->next, i++) {
      assert(edge->start == vertex);
      assert(edge->end != vertex);
      assert(edge->end >= 0);
      assert(edge->end < dpd->order);
      assert(edge->prev->next == edge);
    }
    assert(edge == get_edge(vertex));
  }
  assert(dpd->size == size);
  assert(dpd->n1 == n1);
  assert(dpd->n2 == n2);

  for (i = 0; i < dpd->n1; i++) {
    assert(degree(dpd->deg1[i]) == 1);
  }
  for (i = 0; i < dpd->n2; i++) {
    assert(degree(dpd->deg2[i]) == 2);
  }

  reset_edge_marks(CHECKMARKS, 1);
  for (edge = EDGES; edge < EDGES + dpd->size; edge++) {
    if (!edge_mark(CHECKMARKS, edge)) {
      for (temp = edge, i = 0; i < 4; temp = temp->inverse->prev, i++) {
        assert(!edge_mark(CHECKMARKS, temp));
        mark_edge(CHECKMARKS, temp, 1);
      }
      assert(temp == edge);
    }
  }
}

void print(DoublePreDeco* dpd) {
  int i, vertex;
  Edge* edge;

  printf("order: %d, size: %d, n1: %d, n2: %d\n", dpd->order, dpd->size,
         dpd->n1, dpd->n2);

  for (vertex = 0; vertex < dpd->order; vertex++) {
    printf("\n");
    edge = get_edge(vertex);
    do {
      printf("%d -> %d\n", edge->start, edge->end);
      edge = edge->next;
    } while (edge != get_edge(vertex));
  }
}

#endif
