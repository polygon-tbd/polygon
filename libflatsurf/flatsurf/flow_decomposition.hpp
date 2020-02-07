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

#ifndef LIBFLATSURF_FLOW_DECOMPOSITION_HPP
#define LIBFLATSURF_FLOW_DECOMPOSITION_HPP

#include <functional>
#include <iosfwd>
#include <vector>

#include <boost/logic/tribool.hpp>

#include "flow_component.hpp"
#include "moveable.hpp"

namespace flatsurf {

template <typename Surface>
class FlowDecomposition {
  static_assert(std::is_same_v<Surface, std::decay_t<Surface>>, "type must not have modifiers such as const");

  using T = typename Surface::Coordinate;

 public:
  FlowDecomposition(std::unique_ptr<Surface>, const Vector<T>& vertical);

  // Return whether all resulting components satisfy target, i.e., target could
  // be established for all components without exceeding the limit.
  bool decompose(
      std::function<bool(const FlowComponent<Surface>&)> target = [](const auto& c) {
        return (c.cylinder() || c.withoutPeriodicTrajectory()) ? true : false;
      },
      int limit = -1);

  std::vector<FlowComponent<Surface>> components() const noexcept;

  Vector<T> vertical() const noexcept;

  template <typename S>
  friend std::ostream& operator<<(std::ostream&, const FlowDecomposition<S>&);

 private:
  using Implementation = ::flatsurf::Implementation<FlowDecomposition>;
  Moveable<Implementation> impl;
  friend Implementation;
};

}  // namespace flatsurf

#endif
