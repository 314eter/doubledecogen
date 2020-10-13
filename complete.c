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

#include "complete.h"
#include "canon.h"

static int CANONICAL_VERTICES[MAXORDER];

void check_and_count(DoublePreDeco* dpd, int v0, int v1, int v2, int n) {
  if (!get_filter_lsp() || is_lsp(dpd, v0, v1, v2))
    count(n);
}

void complete02(DoublePreDeco* dpd,
                int nb_vertex_orbits,
                int fixpoint,
                int v1) {
  // Given a double predecoration, a vertex v1 and the vertex orbits different
  // from v1, finish the completion by selecting v0 and v2 in all possible
  // non isomorphic ways.

  int i = 0, v0, v2, d = 1;

  switch (dpd->n1 + dpd->n2 - (degree(v1) < 3)) {
    case 0:
      if (nb_vertex_orbits == 0) {
        for (v0 = 0; v0 < dpd->order - 1; v0++)
          if (v0 != v1) {
            for (v2 = v0 + 1; v2 < dpd->order; v2++)
              if (v2 != v1) {
                check_and_count(dpd, v0, v1, v2, 2);
              }
          }
      } else {
        for (i = 0; i < nb_vertex_orbits; i++) {
          v0 = CANONICAL_VERTICES[i];
          for (v2 = 0; v2 < dpd->order; v2++)
            if (v2 != v1 && v2 != v0) {
              check_and_count(dpd, v0, v1, v2, 1);
            }
          if (fixpoint >= 0)
            check_and_count(dpd, v0, v1, fixpoint, 2);
        }
      }
      break;
    case 1:
      if (dpd->n1 - (degree(v1) == 1)) {
        v0 = dpd->deg1[0] != v1 ? dpd->deg1[0] : dpd->deg1[1];
      } else {
        v0 = dpd->deg2[0] != v1 ? dpd->deg2[0] : dpd->deg2[1];
      }
      if (v0 == fixpoint) {
        for (i = 0; i < nb_vertex_orbits; i++) {
          check_and_count(dpd, fixpoint, v1, CANONICAL_VERTICES[i], 2);
        }
      } else {
        for (v2 = 0; v2 < dpd->order; v2++)
          if (v2 != v1 && v2 != v0) {
            check_and_count(dpd, v0, v1, v2, 2);
          }
      }
      break;
    case 2:
      switch (dpd->n1 - (degree(v1) == 1)) {
        case 0:
          d = 2;
        case 2:
          if (nb_vertex_orbits == 0) {
            for (v0 = 0; v0 < dpd->order - 1; v0++)
              if (v0 != v1 && degree(v0) == d) {
                for (v2 = v0 + 1; v2 < dpd->order; v2++)
                  if (v2 != v1 && degree(v2) == d) {
                    check_and_count(dpd, v0, v1, v2, 2);
                  }
              }
          } else {
            for (i = 0; i < nb_vertex_orbits; i++) {
              if (degree(v0 = CANONICAL_VERTICES[i]) == d) {
                for (v2 = 0; v2 < dpd->order; v2++)
                  if (v2 != v1 && v2 != v0 && degree(v2) == d) {
                    check_and_count(dpd, v0, v1, v2, 1);
                  }
              }
            }
          }
          break;
        case 1:
          v0 = dpd->deg1[0] != v1 ? dpd->deg1[0] : dpd->deg1[1];
          v2 = dpd->deg2[0] != v1 ? dpd->deg2[0] : dpd->deg2[1];
          check_and_count(dpd, v0, v1, v2, 2);
          break;
      }
      break;
  }
}

void complete_odd(DoublePreDeco* dpd,
                  int nb_vertex_orbits,
                  int* canonical_vertices) {
  // Complete the double predecoration to all possible double decorations with
  // odd factor. Start by selecting a vertex of degree 1 as v1. Detach this
  // vertex, and compute the new symmetry.

  int i, v1, nb_vertex_orbits_fixed, fixpoint;

  for (i = 0; i < nb_vertex_orbits; i++) {
    if (degree(v1 = canonical_vertices[i]) == 1) {
      Edge* edge = get_edge(v1);

      if (degree(edge->end) == 3 && dpd->n1 + dpd->n2 == 3)
        continue;

      Edge* edgeA = edge->inverse->prev;

      detach(dpd, edge->inverse);

      nb_vertex_orbits_fixed = fix_edges(dpd, edgeA, edgeA->inverse->prev,
                                         CANONICAL_VERTICES, &fixpoint);
      if (nb_vertex_orbits_fixed >= 0) {
        complete02(dpd, nb_vertex_orbits_fixed, fixpoint, v1);
      }

      attach(dpd, edgeA, edge->inverse);
    }
  }
}

void complete_even(DoublePreDeco* dpd,
                   int nb_vertex_orbits,
                   int* canonical_vertices) {
  // Complete the double predecoration to all possible double decorations with
  // even factor. Start by selecting a vertex v1, and compute the new symmetry.

  int i, v1, nb_vertex_orbits_fixed, fixpoint;

  for (i = 0; i < nb_vertex_orbits; i++) {
    v1 = canonical_vertices[i];

    if (dpd->n1 + dpd->n2 < 3 ? degree(v1) > 1 : degree(v1) == 2) {
      nb_vertex_orbits_fixed =
          fix_vertex(dpd, v1, CANONICAL_VERTICES, &fixpoint);
      complete02(dpd, nb_vertex_orbits_fixed, fixpoint, v1);
    }
  }
}
