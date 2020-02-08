/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2019 Julian Rüth
 *
 *  Flatsurf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Flatsurf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with flatsurf. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef LIBFLATSURF_FLAT_TRIANGULATION_COMBINATORIAL_IMPL_HPP
#define LIBFLATSURF_FLAT_TRIANGULATION_COMBINATORIAL_IMPL_HPP

#include <functional>
#include <set>
#include <unordered_map>

#include "../external/slimsig/include/slimsig/slimsig.h"

#include "../../flatsurf/edge.hpp"
#include "../../flatsurf/flat_triangulation_combinatorial.hpp"
#include "../../flatsurf/half_edge.hpp"
#include "../../flatsurf/permutation.hpp"

namespace flatsurf {

template <>
class Implementation<FlatTriangulationCombinatorial> {
 public:
  Implementation(const Permutation<HalfEdge>&, const std::set<HalfEdge>&);
  ~Implementation();

  void resetVertexes();
  void resetVertices();
  void resetEdges();
  void swap(HalfEdge a, HalfEdge b);

  // Sanity check this triangulation
  void check();

  std::vector<Edge> edges;
  Permutation<HalfEdge> vertices;
  Permutation<HalfEdge> faces;
  std::vector<Vertex> vertexes;

  slimsig::signal<void(HalfEdge)> afterFlip;
  slimsig::signal<void(Edge)> beforeCollapse;
  slimsig::signal<void(HalfEdge, HalfEdge)> beforeSwap;
  slimsig::signal<void(const std::set<Edge>&)> beforeErase;
  slimsig::signal<void(FlatTriangulationCombinatorial*)> afterMove;
};

}  // namespace flatsurf

#endif
