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

#include "planar_code.h"

unsigned char HEADER[15] = ">>planar_code<<";

static Edge* STARTEDGE[MAXORDER];
int NUMBER[MAXORDER];

unsigned char CODE[MAXORDER + MAXSIZE + 1];

void write_planar_header(FILE* file) {
  fwrite(HEADER, sizeof(unsigned char), 15, file);
}

void write_planar_code(FILE* file, DoublePreDeco* dpd) {
  int i, actual_number = 1, last_number = 2, vertex;
  Edge *edge, *run;
  unsigned char* code = CODE;

  for (i = 0; i < dpd->order; i++)
    NUMBER[i] = 0;

  *code = dpd->order;
  code++;

  if (dpd->order < 2) {
    *code = 0;
    code++;
    return;
  }

  edge = get_edge(0);
  NUMBER[edge->start] = 1;
  NUMBER[edge->end] = 2;
  STARTEDGE[1] = edge->inverse;

  while (actual_number <= dpd->order) {
    *code = NUMBER[edge->end];
    code++;
    for (run = edge->next; run != edge; run = run->next) {
      vertex = run->end;
      if (!NUMBER[vertex]) {
        STARTEDGE[last_number++] = run->inverse;
        NUMBER[vertex] = last_number;
      }
      *code = NUMBER[vertex];
      code++;
    }
    *code = 0;
    code++;
    edge = STARTEDGE[actual_number++];
  }

  fwrite(CODE, sizeof(unsigned char), dpd->order + dpd->size + 1, file);
}
