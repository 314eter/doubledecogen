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

#ifndef EXTENSIONS_H_
#define EXTENSIONS_H_

#include "util.h"

int extension1(DoublePreDeco* dpd, Edge* edge);
int extension2(DoublePreDeco* dpd, Edge* edge);
int extension3(DoublePreDeco* dpd, Edge* edge);
int extension4(DoublePreDeco* dpd, Edge* edge);

void reduction1(DoublePreDeco* dpd, Edge* edge);
void reduction2(DoublePreDeco* dpd, Edge* edge);
void reduction3(DoublePreDeco* dpd, Edge* edge);
void reduction4(DoublePreDeco* dpd, Edge* edge);

#endif
