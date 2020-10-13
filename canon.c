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

#include "canon.h"

static Edge* STARTEDGE[MAXORDER];
static int CODE[MAXSIZE];

static Edge* NUMBERING[MAXSIZE][MAXSIZE];

static VertexMarks _VERTEXMARKS;
static VertexMarks* VERTEXMARKS = &_VERTEXMARKS;

static void init_code(DoublePreDeco* dpd, Edge* edge) {
  int i, actual_number = 1, last_number = 2, vertex;
  Edge *run, **numbering = NUMBERING[0];
  int* code = CODE;

  reset_vertex_marks(VERTEXMARKS, dpd->order);

  mark_vertex(VERTEXMARKS, edge->start, 1);
  mark_vertex(VERTEXMARKS, edge->end, 2);
  STARTEDGE[1] = edge->inverse;

  while (actual_number <= dpd->order) {
    *numbering = edge;
    numbering++;
    for (run = edge->next; run != edge; run = run->next) {
      vertex = run->end;
      if (!(*code = vertex_mark(VERTEXMARKS, vertex))) {
        STARTEDGE[last_number++] = run->inverse;
        mark_vertex(VERTEXMARKS, vertex, last_number);
        *code = -degree(vertex);
      }
      code++;
      *numbering = run;
      numbering++;
    }
    *code = 0;
    code++;
    edge = STARTEDGE[actual_number++];
  }
}

static int compare_code(DoublePreDeco* dpd, Edge* edge, int number) {
  int i, actual_number = 1, last_number = 2, vertex, c;
  Edge *run, **numbering = NUMBERING[number];
  int* code = CODE;

  reset_vertex_marks(VERTEXMARKS, dpd->order);

  mark_vertex(VERTEXMARKS, edge->start, 1);
  mark_vertex(VERTEXMARKS, edge->end, 2);
  STARTEDGE[1] = edge->inverse;

  while (actual_number <= dpd->order) {
    *numbering = edge;
    numbering++;
    for (run = edge->next; run != edge; run = run->next) {
      vertex = run->end;
      if (!(c = vertex_mark(VERTEXMARKS, vertex))) {
        STARTEDGE[last_number++] = run->inverse;
        mark_vertex(VERTEXMARKS, vertex, last_number);
        c = -degree(vertex);
      }
      if (c > *code)
        return -1;
      if (c < *code)
        return 1;
      code++;
      *numbering = run;
      numbering++;
    }
    if (0 > *code)
      return -1;
    if (0 < *code)
      return 1;
    code++;
    edge = STARTEDGE[actual_number++];
  }

  return 0;
}

static int edge_cmp(DoublePreDeco* dpd, Edge* a, Edge* b) {
  if (degree(a->start) < degree(b->start))
    return -1;
  if (degree(a->start) > degree(b->start))
    return 1;
  if (degree(a->end) < degree(b->end))
    return -1;
  if (degree(a->end) > degree(b->end))
    return 1;
  return 0;
}

static int add_to_list(DoublePreDeco* dpd,
                       Edge** list,
                       Edge* edge,
                       Edge* candidate) {
  switch (edge_cmp(dpd, candidate, edge)) {
    case -1:
      return -1;
    case 0:
      *list = candidate;
      return 1;
    default:
      return 0;
  }
}

static Edge* EDGELIST[MAXSIZE];
static int NB_SYM = 0;

static EdgeMarks _EDGEMARKS;
static EdgeMarks* EDGEMARKS = &_EDGEMARKS;

int canon(DoublePreDeco* dpd, int ext, Edge* edge, Edge** canonical_edges) {
  // Check whether the given extension applied at the given edge is the
  // canonical reduction for the given double predecoration.
  // If so, return the number of edge orbits and store a canonical edge for
  // each orbit in the given array.

  Edge* run = edge;
  int listlength = 0, added, i;

  if (dpd->n2 != 0 && ext > 2)
    return 0;
  if (dpd->n1 != 0 && ext > 3)
    return 0;

  /* Find extension 1 */
  if (ext == 0) {
    for (i = 0; i < dpd->order; i++) {
      run = get_edge(i);
      do {
        listlength += added =
            add_to_list(dpd, EDGELIST + listlength, edge, run);
        if (added == -1)
          return 0;
        run = run->next;
      } while (run != get_edge(i));
    }
  } else {
    for (i = 0; i < dpd->n2; i++) {
      run = get_edge(dpd->deg2[i]);
      if (degree(run->end) > 3 || degree(run->inverse->prev->end) == 1) {
        if (ext != 1)
          return 0;
        listlength += added =
            add_to_list(dpd, EDGELIST + listlength, edge, run->inverse->prev);
        if (added == -1)
          return 0;
      }
      run = run->next;
      if (degree(run->end) > 3 || degree(run->inverse->prev->end) == 1) {
        if (ext != 1)
          return 0;
        listlength += added =
            add_to_list(dpd, EDGELIST + listlength, edge, run->inverse->prev);
        if (added == -1)
          return 0;
      }
    }
  }

  /* Find extension 2 */
  if (ext > 1) {
    for (i = 0; i < dpd->n2; i++) {
      run = get_edge(dpd->deg2[i]);
      listlength += added =
          add_to_list(dpd, EDGELIST + listlength, edge, run->inverse->prev);
      if (added == -1)
        return 0;
      run = run->next;
      listlength += added =
          add_to_list(dpd, EDGELIST + listlength, edge, run->inverse->prev);
      if (added == -1)
        return 0;
    }
  }

  /* Find extension 3 */
  if (ext > 2) {
    for (i = 0; i < dpd->n1; i++) {
      run = get_edge(dpd->deg1[i]);
      listlength += added =
          add_to_list(dpd, EDGELIST + listlength, edge, run->inverse->prev);
      if (added == -1)
        return 0;
    }
  }

  /* Find extension 4 */
  if (ext > 3) {
    for (i = 0; i < dpd->order; i++)
      if (degree(i) == 3) {
        run = get_edge(i);
        if (run->end != run->next->end && run->end != run->prev->end &&
            run->next->end != run->prev->end) {
          if (degree(run->end) > 3) {
            listlength += added = add_to_list(dpd, EDGELIST + listlength, edge,
                                              run->inverse->prev);
            if (added == -1)
              return 0;
          }
          run = run->next;
          if (degree(run->end) > 3) {
            listlength += added = add_to_list(dpd, EDGELIST + listlength, edge,
                                              run->inverse->prev);
            if (added == -1)
              return 0;
          }
          run = run->next;
          if (degree(run->end) > 3) {
            listlength += added = add_to_list(dpd, EDGELIST + listlength, edge,
                                              run->inverse->prev);
            if (added == -1)
              return 0;
          }
        }
      }
  }

  init_code(dpd, edge);

  NB_SYM = 1;
  for (i = 0; i < listlength; i++)
    if (EDGELIST[i] != edge) {
      switch (compare_code(dpd, EDGELIST[i], NB_SYM)) {
        case 1:
          return 0;
        case 0:
          NB_SYM += 1;
      }
    }

  int nb_edge_orbits = 0;
  reset_edge_marks(EDGEMARKS, 1);

  for (i = 0; i < dpd->size; i++) {
    run = NUMBERING[0][i];
    if (!edge_mark(EDGEMARKS, run)) {
      canonical_edges[nb_edge_orbits++] = run;
      mark_edge(EDGEMARKS, run, 1);
      for (int n = 1; n < NB_SYM; n++) {
        mark_edge(EDGEMARKS, NUMBERING[n][i], 1);
      }
    }
  }

  return nb_edge_orbits;
}

int compute_vertex_orbits(DoublePreDeco* dpd, int* canonical_vertices) {
  // Return the number of vertex orbits and store a canonical vertex for
  // each orbit in the given array.

  int nb_vertex_orbits = 0;
  reset_vertex_marks(VERTEXMARKS, 1);

  for (int i = 0; i < dpd->size; i++) {
    int vertex = NUMBERING[0][i]->start;
    if (!vertex_mark(VERTEXMARKS, vertex)) {
      canonical_vertices[nb_vertex_orbits++] = vertex;
      for (int n = 0; n < NB_SYM; n++) {
        mark_vertex(VERTEXMARKS, NUMBERING[n][i]->start, 1);
      }
    }
  }

  return nb_vertex_orbits;
}

static int SYMMETRIES[MAXSIZE];

int fix_vertex(DoublePreDeco* dpd,
               int vertex,
               int* canonical_vertices,
               int* fixpoint) {
  // Make the given vertex a fixpoint. The remaining symmetries will all be
  // rotations around this vertex.
  // Return the number of vertex orbits consisting of more than one vertex
  // and store a canonical vertex for each orbit in the given array.
  // If there is another vertex that is a fixpoint for this symmetry group,
  // store this vertex too.

  int rotations = 0, i, k = 0, nb_vertex_orbits = 0;

  *fixpoint = -1;

  while (NUMBERING[0][k]->start != vertex)
    k++;
  for (i = 1; i < NB_SYM; i++)
    if (NUMBERING[i][k]->start == vertex) {
      SYMMETRIES[rotations++] = i;
    }

  if (rotations == 0)
    return nb_vertex_orbits;

  reset_vertex_marks(VERTEXMARKS, 1);
  mark_vertex(VERTEXMARKS, vertex, 1);

  for (i = 0; i < dpd->size; i++) {
    int canon = NUMBERING[0][i]->start;
    if (!vertex_mark(VERTEXMARKS, canon)) {
      canonical_vertices[nb_vertex_orbits++] = canon;
      mark_vertex(VERTEXMARKS, canon, 1);
      for (k = 0; k < rotations; k++) {
        int v = NUMBERING[SYMMETRIES[k]][i]->start;
        if (!vertex_mark(VERTEXMARKS, v)) {
          mark_vertex(VERTEXMARKS, v, 1);
        } else {
          *fixpoint = canon;
          nb_vertex_orbits--;
        }
      }
    }
  }

  return nb_vertex_orbits;
}

static Edge* STARTEDGE2[MAXORDER];

static VertexMarks _VERTEXMARKS2;
static VertexMarks* VERTEXMARKS2 = &_VERTEXMARKS2;

int fix_edges(DoublePreDeco* dpd,
              Edge* edge,
              Edge* edge2,
              int* canonical_vertices,
              int* fixpoint) {
  // Fix the 2-cycle consisting of the given edges.
  // Return the number of vertex orbits consisting of more than one vertex
  // and store a canonical vertex for each orbit in the given array.
  // If there is another vertex that is a fixpoint for this symmetry group,
  // store this vertex too.

  int i, actual_number = 1, last_number = 2, vertex, vertex2,
         nb_vertex_orbits = 0;
  Edge *run, *run2;

  *fixpoint = -1;

  if (degree(edge->start) < degree(edge2->start))
    return 0;
  if (degree(edge->start) > degree(edge2->start))
    return -1;

  reset_vertex_marks(VERTEXMARKS, dpd->order);
  reset_vertex_marks(VERTEXMARKS2, dpd->order);

  mark_vertex(VERTEXMARKS, edge->start, 1);
  mark_vertex(VERTEXMARKS, edge->end, 2);
  mark_vertex(VERTEXMARKS2, edge2->start, 1);
  mark_vertex(VERTEXMARKS2, edge2->end, 2);
  STARTEDGE[0] = edge;
  STARTEDGE2[0] = edge2;
  STARTEDGE[1] = edge->inverse;
  STARTEDGE2[1] = edge2->inverse;

  while (actual_number <= last_number) {
    for (run = edge->next, run2 = edge2->next; run != edge;
         run = run->next, run2 = run2->next) {
      vertex = run->end;
      vertex2 = run2->end;

      if (vertex_mark(VERTEXMARKS, vertex) < vertex_mark(VERTEXMARKS2, vertex2))
        return 0;
      if (vertex_mark(VERTEXMARKS, vertex) > vertex_mark(VERTEXMARKS2, vertex2))
        return -1;

      if (!vertex_mark(VERTEXMARKS, vertex)) {
        STARTEDGE[last_number] = run->inverse;
        STARTEDGE2[last_number++] = run2->inverse;
        mark_vertex(VERTEXMARKS, vertex, last_number);
        mark_vertex(VERTEXMARKS2, vertex2, last_number);

        if (degree(vertex) < degree(vertex2))
          return 0;
        if (degree(vertex) > degree(vertex2))
          return -1;
      }
    }
    edge = STARTEDGE[actual_number];
    edge2 = STARTEDGE2[actual_number++];
  }

  reset_vertex_marks(VERTEXMARKS, 1);

  for (i = 0; i < last_number; i++) {
    int canon = STARTEDGE[i]->start;
    if (!vertex_mark(VERTEXMARKS, canon)) {
      canonical_vertices[nb_vertex_orbits++] = canon;
      mark_vertex(VERTEXMARKS, canon, 1);
      int v = STARTEDGE2[i]->start;
      if (!vertex_mark(VERTEXMARKS, v)) {
        mark_vertex(VERTEXMARKS, v, 1);
      } else {
        *fixpoint = canon;
        nb_vertex_orbits--;
      }
    }
  }

  return nb_vertex_orbits;
}

int compare_edges(DoublePreDeco* dpd, Edge* edge, Edge* edge2, int v1, int v2) {
  int i, actual_number = 1, last_number = 2, vertex, vertex2;
  Edge *run, *run2;

  if (degree(edge->end) != degree(edge2->end))
    return 0;
  if ((edge->end == v1 || edge->end == v2 || edge2->end == v1 ||
       edge2->end == v2) &&
      edge->end != edge2->end)
    return 0;

  reset_vertex_marks(VERTEXMARKS, dpd->order);
  reset_vertex_marks(VERTEXMARKS2, dpd->order);

  mark_vertex(VERTEXMARKS, edge->start, 1);
  mark_vertex(VERTEXMARKS, edge->end, 2);
  mark_vertex(VERTEXMARKS2, edge2->start, 1);
  mark_vertex(VERTEXMARKS2, edge2->end, 2);
  STARTEDGE[0] = edge;
  STARTEDGE2[0] = edge2;
  STARTEDGE[1] = edge->inverse;
  STARTEDGE2[1] = edge2->inverse;

  while (actual_number <= last_number) {
    for (run = edge->next, run2 = edge2->prev; run != edge;
         run = run->next, run2 = run2->prev) {
      vertex = run->end;
      vertex2 = run2->end;

      if (vertex_mark(VERTEXMARKS, vertex) !=
          vertex_mark(VERTEXMARKS2, vertex2))
        return 0;

      if (!vertex_mark(VERTEXMARKS, vertex)) {
        STARTEDGE[last_number] = run->inverse;
        STARTEDGE2[last_number++] = run2->inverse;
        mark_vertex(VERTEXMARKS, vertex, last_number);
        mark_vertex(VERTEXMARKS2, vertex2, last_number);

        if (degree(vertex) != degree(vertex2))
          return 0;
        if ((vertex == v1 || vertex == v2 || vertex2 == v1 || vertex2 == v2) &&
            vertex != vertex2)
          return 0;
      }
    }
    edge = STARTEDGE[actual_number];
    edge2 = STARTEDGE2[actual_number++];
  }

  return 1;
}

int is_lsp(DoublePreDeco* dpd, int v0, int v1, int v2) {
  Edge *edge, *run;

  run = edge = get_edge(v0);
  do {
    if (compare_edges(dpd, edge, run, v1, v2))
      return 1;
    run = run->next;
  } while (run != edge);

  return 0;
}
