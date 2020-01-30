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

#ifndef LIBFLATSURF_CONTOUR_CONNECTION_HPP
#define LIBFLATSURF_CONTOUR_CONNECTION_HPP

#include <list>

#include "external/spimpl/spimpl.h"

#include "forward.hpp"

namespace flatsurf {
// A non-vertical connection.
template <typename Surface>
class ContourConnection : boost::equality_comparable<ContourComponent<Surface>> {
 private:
  // Connections can not be created directly (other than by copying & moving
  // them.) They are byproducts of a ContourDecomposition.
  ContourConnection();

  static_assert(std::is_same_v<Surface, std::decay_t<Surface>>, "type must not have modifiers such as const");

 public:
  using T = typename Surface::Coordinate;
  using SaddleConnection = typename Surface::SaddleConnection;

  SaddleConnection connection() const;

  // The vertical connections on the left of this non-vertical connection going
  // from the left end of `connection` towards the interior.
  std::list<SaddleConnection> left() const;
  // The vertical connections on the right of this non-vertical connection going
  // from the right end of `connection` towards the interior.
  std::list<SaddleConnection> right() const;

  ContourConnection nextInPerimeter() const;
  ContourConnection previousInPerimeter() const;

  ContourComponent<Surface> component() const;

  bool top() const;
  bool bottom() const;

  ContourConnection<Surface> operator-() const;

  bool operator==(const ContourConnection<Surface>&) const;

  template <typename S>
  friend std::ostream &operator<<(std::ostream &, const ContourConnection<S> &);

 private:
  using Implementation = ::flatsurf::Implementation<ContourConnection>;
  spimpl::impl_ptr<Implementation> impl;
  friend Implementation;
};
}  // namespace flatsurf

#endif