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

#ifndef UTIL_H_
#define UTIL_H_

#define MAXFACTOR 100
#define MAXORDER 52  // (MAXFACTOR + 1) / 2 + 2
#define MAXSIZE 202  // 2 * (MAXFACTOR + 1)

typedef struct _Edge {
  int start;
  int end;

  struct _Edge* prev;
  struct _Edge* next;
  struct _Edge* inverse;
} Edge;

typedef struct {
  int order;
  int size;

  int n1;
  int deg1[5];
  int n2;
  int deg2[5];
} DoublePreDeco;

int create_vertex(DoublePreDeco*);
Edge* create_edge(DoublePreDeco*, int, int);

void set_next(Edge*, Edge*);
void detach(DoublePreDeco*, Edge*);
void attach(DoublePreDeco*, Edge*, Edge*);

void remove_extension(DoublePreDeco*);

Edge* get_edge(int);
int degree(int);

typedef struct {
  unsigned int unmarked;
  unsigned int range;
  unsigned int marks[MAXORDER];
} VertexMarks;

void reset_vertex_marks(VertexMarks*, int);
void mark_vertex(VertexMarks*, int, int);
int vertex_mark(VertexMarks*, int);

typedef struct {
  unsigned int unmarked;
  unsigned int range;
  unsigned int marks[MAXSIZE];
} EdgeMarks;

void reset_edge_marks(EdgeMarks*, int);
void mark_edge(EdgeMarks*, Edge*, int);
int edge_mark(EdgeMarks*, Edge*);

void precount(int);
unsigned long long get_precount();

void count(int);
unsigned long long get_count();

void filter_lsp(int);
int get_filter_lsp();

#ifdef DEBUG

void check(DoublePreDeco* dpd);
void print(DoublePreDeco* dpd);
#define CHECK(dpd) check(dpd)

#else

#define NDEBUG
#define CHECK(dpd)

#endif

#include <assert.h>

#endif
