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

#ifndef CANON_H_
#define CANON_H_

#include "util.h"

int canon(DoublePreDeco*, int, Edge*, Edge**);
int compute_vertex_orbits(DoublePreDeco*, int*);
int fix_vertex(DoublePreDeco*, int, int*, int*);
int fix_edges(DoublePreDeco*, Edge*, Edge*, int*, int*);
int is_lsp(DoublePreDeco*, int, int, int);

#endif
