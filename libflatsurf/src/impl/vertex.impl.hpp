/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2019-2020 Julian Rüth
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

#ifndef LIBFLATSURF_VERTEX_IMPL_HPP
#define LIBFLATSURF_VERTEX_IMPL_HPP

#include <vector>

#include "../../flatsurf/half_edge_set.hpp"
#include "../../flatsurf/vertex.hpp"

namespace flatsurf {

template <>
class ImplementationOf<Vertex> {
 public:
  ImplementationOf(const HalfEdgeSet&);

  static bool comparable(const HalfEdgeSet&, const HalfEdgeSet&);
  static void afterFlip(Vertex&, const FlatTriangulationCombinatorial&, HalfEdge flip);

  static Vertex make(const std::vector<HalfEdge> sources);

  static const HalfEdgeSet& outgoing(const Vertex&);

  // The half edges starting at this vertex.
  HalfEdgeSet sources;
};

template <typename... Args>
Vertex::Vertex(PrivateConstructor, Args&&... args) :
  self(spimpl::make_impl<ImplementationOf<Vertex>>(std::forward<Args>(args)...)) {
}

}  // namespace flatsurf

#endif
