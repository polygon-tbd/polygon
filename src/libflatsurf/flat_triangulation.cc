/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2019 Julian Rüth
 *
 *  Flatsurf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
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

#include <ostream>
#include <vector>

#include "flatsurf/flat_triangulation.hpp"
#include "flatsurf/half_edge.hpp"
#include "flatsurf/half_edge_map.hpp"
#include "flatsurf/vector.hpp"
#include "util/assert.ipp"

using std::ostream;
using std::vector;

namespace flatsurf {
namespace {
template <typename T>
void updateAfterFlip(HalfEdgeMap<T> &map, HalfEdge halfEdge,
                     const FlatTriangulationCombinatorial &parent) {
  map.set(halfEdge, map.get(-parent.nextInFace(halfEdge)) +
                        map.get(parent.nextAtVertex(halfEdge)));
}
}  // namespace

template <typename T>
class FlatTriangulation<T>::Implementation {
 public:
  Implementation(HalfEdgeMap<Vector> &&vectors) : vectors(std::move(vectors)) {}

  const HalfEdgeMap<Vector> vectors;
};

template <typename T>
const Vector<T> &FlatTriangulation<T>::fromEdge(const HalfEdge e) const {
  return impl->vectors.get(e);
}

template <typename T>
FlatTriangulation<T>::FlatTriangulation(
    FlatTriangulationCombinatorial &&combinatorial,
    const vector<Vector> &vectors)
    : FlatTriangulation(std::move(combinatorial),
                        HalfEdgeMap<Vector>(combinatorial, vectors,
                                            updateAfterFlip<Vector>)) {}

template <typename T>
FlatTriangulation<T>::FlatTriangulation(
    FlatTriangulationCombinatorial &&combinatorial,
    HalfEdgeMap<Vector> &&vectors)
    : FlatTriangulationCombinatorial(std::move(combinatorial)),
      impl(spimpl::make_unique_impl<Implementation>(std::move(vectors))) {
  // check that faces are closed
  for (auto edge : halfEdges()) {
    auto zero = fromEdge(edge);
    edge = nextInFace(edge);
    zero += fromEdge(edge);
    edge = nextInFace(edge);
    zero += fromEdge(edge);
    CHECK_ARGUMENT(!zero, "some face is not closed");
  }
}

template <typename T>
FlatTriangulation<T> FlatTriangulation<T>::clone() const {
  std::vector<Vector> vectors;
  for (int e = 1; e <= halfEdges().size() / 2; e++)
    vectors.push_back(fromEdge(HalfEdge(e)));
  return FlatTriangulation(FlatTriangulationCombinatorial::clone(),
                           std::move(vectors));
}

template <typename T>
ostream &operator<<(ostream &os, const FlatTriangulation<T> &self) {
  return os << static_cast<const FlatTriangulationCombinatorial &>(self)
            << " with vectors " << self.impl->vectors;
}
}  // namespace flatsurf

// Instantiations of templates so implementations are generated for the linker
#include <e-antic/renfxx_fwd.h>
#include <exact-real/integer_ring_traits.hpp>
#include <exact-real/number_field_traits.hpp>
#include <exact-real/rational_field_traits.hpp>

using namespace flatsurf;

template class flatsurf::FlatTriangulation<long long>;
template ostream &flatsurf::operator<<(ostream &, const FlatTriangulation<long long> &);
template class flatsurf::FlatTriangulation<eantic::renf_elem_class>;
template ostream &flatsurf::operator<<(ostream &, const FlatTriangulation<eantic::renf_elem_class> &);
template class flatsurf::FlatTriangulation<exactreal::Element<exactreal::IntegerRingTraits>>;
template ostream &flatsurf::operator<<(ostream &, const FlatTriangulation<exactreal::Element<exactreal::IntegerRingTraits>> &);
template class flatsurf::FlatTriangulation<exactreal::Element<exactreal::RationalFieldTraits>>;
template ostream &flatsurf::operator<<(ostream &, const FlatTriangulation<exactreal::Element<exactreal::RationalFieldTraits>> &);
template class flatsurf::FlatTriangulation<exactreal::Element<exactreal::NumberFieldTraits>>;
template ostream &flatsurf::operator<<(ostream &, const FlatTriangulation<exactreal::Element<exactreal::NumberFieldTraits>> &);