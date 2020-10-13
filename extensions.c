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

#include "extensions.h"

static int _extension1(DoublePreDeco* dpd, Edge* edgeA) {
  Edge* edgeB = edgeA->inverse->prev->inverse;

  int vertex = create_vertex(dpd);
  Edge* edge0 = create_edge(dpd, edgeA->start, vertex);
  Edge* inverse0 = edge0->inverse;
  Edge* edge1 = create_edge(dpd, edgeB->start, vertex);
  Edge* inverse1 = edge1->inverse;

  set_next(edge0, edgeA->next);
  set_next(edgeA, edge0);
  set_next(edgeB->prev, edge1);
  set_next(edge1, edgeB);
  set_next(inverse0, inverse1);
  set_next(inverse1, inverse0);

  return 1;
}

int extension1(DoublePreDeco* dpd, Edge* edgeA) {
  if (degree(edgeA->end) > 1 && degree(edgeA->start) < 3)
    return 0;

  return _extension1(dpd, edgeA);
}

void reduction1(DoublePreDeco* dpd, Edge* edgeA) {
  remove_extension(dpd);
}

int extension2(DoublePreDeco* dpd, Edge* edgeA) {
  if (degree(edgeA->start) != 2 || degree(edgeA->inverse->prev->end) != 2)
    return 0;

  return _extension1(dpd, edgeA);
}

void reduction2(DoublePreDeco* dpd, Edge* edgeA) {
  remove_extension(dpd);
}

int extension3(DoublePreDeco* dpd, Edge* edge) {
  Edge* inverse = edge->inverse;

  int vertex = create_vertex(dpd);
  Edge* edge0 = create_edge(dpd, edge->start, edge->end);
  Edge* inverse0 = edge0->inverse;
  Edge* edge1 = create_edge(dpd, edge->start, vertex);
  Edge* inverse1 = edge1->inverse;

  set_next(edge0, edge->next);
  set_next(edge, edge1);
  set_next(edge1, edge0);
  set_next(inverse1, inverse1);
  set_next(inverse->prev, inverse0);
  set_next(inverse0, inverse);

  return 1;
}

void reduction3(DoublePreDeco* dpd, Edge* edge) {
  remove_extension(dpd);
}

int extension4(DoublePreDeco* dpd, Edge* edgeA) {
  if (degree(edgeA->start) < 3 || degree(edgeA->end) < 4)
    return 0;

  Edge* inverseA = edgeA->inverse;
  Edge* edgeC = inverseA->prev;
  Edge* inverseB = edgeC->prev;
  Edge* edgeB = inverseB->inverse;

  if (edgeA->start == edgeB->start || degree(edgeB->start) < 2)
    return 0;

  int vertex = create_vertex(dpd);
  Edge* edge0 = create_edge(dpd, edgeA->start, vertex);
  Edge* inverse0 = edge0->inverse;
  Edge* edge1 = create_edge(dpd, edgeB->start, vertex);
  Edge* inverse1 = edge1->inverse;

  detach(dpd, edgeC);

  set_next(edge0, edgeA->next);
  set_next(edgeA, edge0);
  set_next(edgeB->prev, edge1);
  set_next(edge1, edgeB);
  set_next(inverse0, inverse1);
  set_next(inverse1, inverse0);

  attach(dpd, inverse1, edgeC);

  return 1;
}

void reduction4(DoublePreDeco* dpd, Edge* edgeA) {
  Edge* edgeC = edgeA->next->inverse->prev;

  remove_extension(dpd);
  attach(dpd, edgeA->inverse->prev, edgeC);
}
