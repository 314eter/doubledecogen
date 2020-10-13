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

#include <getopt.h>
#include <stdlib.h>
#include "canon.h"
#include "complete.h"
#include "extensions.h"
#include "planar_code.h"
#include "util.h"

int OUTPUT = 0;
int DPD_OUTPUT = 0;
int ALL = 0;
int CONNECTIVITY = 3;
int RES = 0;
int MOD = 1;
int SPLIT_LEVEL = 3;
FILE* OUTFILE;

int FACTOR;

static Edge* CANONICAL_EDGES[MAXORDER][MAXSIZE];
static int CANONICAL_VERTICES[MAXORDER];

void grow(DoublePreDeco*, int);

void try_extension(DoublePreDeco* dpd,
                   int nb_edge_orbits,
                   int (*extension)(DoublePreDeco*, Edge*),
                   void (*reduction)(DoublePreDeco*, Edge*),
                   int ext) {
  int nb_edge_orbits_copy;

  for (int i = 0; i < nb_edge_orbits; i++) {
    Edge* edge = CANONICAL_EDGES[dpd->order][i];
    DoublePreDeco copy = *dpd;
    if (extension(&copy, edge)) {
      CHECK(&copy);
      if (copy.n1 + copy.n2 <= 4 &&
          (nb_edge_orbits_copy =
               canon(&copy, ext, edge, CANONICAL_EDGES[copy.order]))) {
        grow(&copy, nb_edge_orbits_copy);
      }
      reduction(&copy, edge);
      CHECK(dpd);
    }
  }
}

void grow(DoublePreDeco* dpd, int nb_edge_orbits) {
  if (dpd->order - 2 == (FACTOR + 1) / 2) {
    // Complete the double predecoration
    if (dpd->n1 + dpd->n2 <= 3) {
      precount(1);
      int nb_vertex_orbits = compute_vertex_orbits(dpd, CANONICAL_VERTICES);
      if (DPD_OUTPUT)
        write_planar_code(OUTFILE, dpd);
      if (FACTOR & 1) {
        complete_odd(dpd, nb_vertex_orbits, CANONICAL_VERTICES);
      } else {
        complete_even(dpd, nb_vertex_orbits, CANONICAL_VERTICES);
      }
    }
  } else {
    // Apply extensions
    try_extension(dpd, nb_edge_orbits, extension1, reduction1, 1);
    try_extension(dpd, nb_edge_orbits, extension2, reduction2, 2);
    try_extension(dpd, nb_edge_orbits, extension3, reduction3, 3);
    try_extension(dpd, nb_edge_orbits, extension4, reduction4, 4);
  }
}

void start_construction(DoublePreDeco* dpd) {
  /* First base */
  dpd->order = dpd->size = 0;
  dpd->n1 = dpd->n2 = 0;

  int v0 = create_vertex(dpd);
  int v1 = create_vertex(dpd);
  int v2 = create_vertex(dpd);
  Edge* edge0 = create_edge(dpd, v0, v1);
  Edge* inverse0 = edge0->inverse;
  Edge* edge1 = create_edge(dpd, v1, v2);
  Edge* inverse1 = edge1->inverse;

  set_next(edge0, edge0);
  set_next(inverse0, edge1);
  set_next(edge1, inverse0);
  set_next(inverse1, inverse1);

  int nb_edge_orbits = canon(dpd, 0, edge0, CANONICAL_EDGES[dpd->order]);
  CHECK(dpd);
  grow(dpd, nb_edge_orbits);

  if (FACTOR < 5)
    return;

  /* Second base */
  int v3 = create_vertex(dpd);
  Edge* edge2 = create_edge(dpd, 0, v3);
  Edge* inverse2 = edge2->inverse;
  Edge* edge3 = create_edge(dpd, v3, v2);
  Edge* inverse3 = edge3->inverse;

  set_next(edge2, edge0);
  set_next(edge0, edge2);
  set_next(inverse3, inverse1);
  set_next(inverse1, inverse3);
  set_next(edge3, inverse2);
  set_next(inverse2, edge3);

  nb_edge_orbits = canon(dpd, 0, edge0, CANONICAL_EDGES[dpd->order]);
  CHECK(dpd);
  grow(dpd, nb_edge_orbits);

  if (FACTOR < 11)
    return;

  /* Third base */
  int v4 = create_vertex(dpd);
  int v5 = create_vertex(dpd);
  int v6 = create_vertex(dpd);
  int v7 = create_vertex(dpd);

  Edge* edge4 = create_edge(dpd, v0, v4);
  Edge* inverse4 = edge4->inverse;
  Edge* edge5 = create_edge(dpd, v1, v5);
  Edge* inverse5 = edge5->inverse;
  Edge* edge6 = create_edge(dpd, v2, v6);
  Edge* inverse6 = edge6->inverse;
  Edge* edge7 = create_edge(dpd, v3, v7);
  Edge* inverse7 = edge7->inverse;
  Edge* edge8 = create_edge(dpd, v4, v5);
  Edge* inverse8 = edge8->inverse;
  Edge* edge9 = create_edge(dpd, v5, v6);
  Edge* inverse9 = edge9->inverse;
  Edge* edge10 = create_edge(dpd, v4, v7);
  Edge* inverse10 = edge10->inverse;
  Edge* edge11 = create_edge(dpd, v7, v6);
  Edge* inverse11 = edge11->inverse;

  set_next(edge4, edge0);
  set_next(edge2, edge4);
  set_next(edge5, edge1);
  set_next(inverse0, edge5);
  set_next(edge6, inverse3);
  set_next(inverse1, edge6);
  set_next(edge7, inverse2);
  set_next(edge3, edge7);

  set_next(edge8, inverse4);
  set_next(edge10, edge8);
  set_next(edge9, inverse5);
  set_next(inverse8, edge9);
  set_next(inverse4, edge10);
  set_next(inverse5, inverse8);
  set_next(inverse11, inverse6);
  set_next(inverse6, inverse9);
  set_next(inverse9, inverse11);
  set_next(inverse10, inverse7);
  set_next(edge11, inverse10);
  set_next(inverse7, edge11);

  nb_edge_orbits = canon(dpd, 0, edge0, CANONICAL_EDGES[dpd->order]);
  CHECK(dpd);
  grow(dpd, nb_edge_orbits);
}

static void write_help(FILE* file) {
  fprintf(file, "Usage: decogen [-d] [-a] [-c 1|2|3] [-o OUTFILE] FACTOR\n\n");
  fprintf(file, " -d,--decocode      write decocode to stdout or outfile\n");
  fprintf(file,
          " -a,--all           generate decorations with smaller factors\n");
  fprintf(file,
          " -c,--connectivity  generate decorations with connectivity 1/2/3\n");
  fprintf(file, " -o,--output        write to OUTFILE instead of stdout\n");
  fprintf(file,
          " FACTOR             generate decorations with factor FACTOR (or "
          "smaller with -a)\n");
}

int main(int argc, char* argv[]) {
  int c, option_index;

  OUTFILE = stdout;

  static struct option long_options[] = {
      {"decocode", no_argument, 0, 'd'},
      {"all", no_argument, 0, 'a'},
      {"connectivity", required_argument, 0, 'c'},
      {"output", required_argument, 0, 'o'},
      {"help", no_argument, 0, 'h'},
      {"mod", required_argument, 0, 'm'},
      {"res", required_argument, 0, 'r'},
      {"split", required_argument, 0, 's'},
      {"predeco", no_argument, 0, 'p'},
      {"lsp", no_argument, 0, 'l'},
  };

  while (1) {
    c = getopt_long(argc, argv, "dac:o:hm:r:s:pl", long_options, &option_index);
    if (c == -1)
      break;
    switch (c) {
      case 'd':
        OUTPUT = 1;
        break;
      case 'a':
        ALL = 1;
        break;
      case 'c':
        CONNECTIVITY = strtol(optarg, NULL, 10);
        if (CONNECTIVITY < 1 || CONNECTIVITY > 3) {
          fprintf(stderr, "Connectivity has to be 1, 2 or 3.\n");
          return 1;
        }
        break;
      case 'o':
        OUTFILE = fopen(optarg, "w");
        break;
      case 'h':
        write_help(stdout);
        return 0;
      case 'm':
        MOD = strtoul(optarg, NULL, 10);
        break;
      case 'r':
        RES = strtoul(optarg, NULL, 10);
        break;
      case 's':
        SPLIT_LEVEL = strtoul(optarg, NULL, 10);
        break;
      case 'p':
        DPD_OUTPUT = 1;
        break;
      case 'l':
        filter_lsp(1);
        break;
      default:
        write_help(stderr);
        return 1;
    }
  }

  if (OUTPUT && DPD_OUTPUT) {
    fprintf(stderr, "-d and -p are mutually exclusive\n");
    return 1;
  }

  if (optind == argc) {
    write_help(stderr);
    return 1;
  }

  FACTOR = strtol(argv[optind], NULL, 10);
  if (FACTOR < 1) {
    fprintf(stderr, "\"%s\" is no positive numeric value.\n", argv[optind]);
    return 1;
  } else if (FACTOR > MAXFACTOR) {
    fprintf(stderr, "inflation rates higher than %d are not supported.\n",
            MAXSIZE / 2);
    return 1;
  }

  // if (OUTPUT) write_deco_header(OUTFILE);
  if (DPD_OUTPUT)
    write_planar_header(OUTFILE);

  DoublePreDeco dpd;
  start_construction(&dpd);

  fprintf(stderr, "%lld decorations (%lld predecorations)\n", 2 * get_count(),
          get_precount());

  return 0;
}
