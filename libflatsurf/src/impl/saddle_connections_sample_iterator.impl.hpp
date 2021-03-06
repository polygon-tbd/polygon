/**********************************************************************
 *  This file is part of flatsurf.
 *
 *        Copyright (C) 2020 Julian Rüth
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

#ifndef LIBFLATSURF_SADDLE_CONNECTIONS_SAMPLE_ITERATOR_IMPL_HPP
#define LIBFLATSURF_SADDLE_CONNECTIONS_SAMPLE_ITERATOR_IMPL_HPP

#include <random>
#include <tuple>
#include <unordered_set>

#include "../../flatsurf/saddle_connection.hpp"
#include "../../flatsurf/saddle_connections_sample_iterator.hpp"

namespace flatsurf {

template <typename Surface>
class ImplementationOf<SaddleConnectionsSampleIterator<Surface>> {
  using T = typename Surface::Coordinate;

 public:
  ImplementationOf(const SaddleConnectionsSample<Surface>&);

  void increment();

  const SaddleConnectionsSample<Surface>& connections;
  std::unordered_set<SaddleConnection<Surface>> seen;

  SaddleConnection<Surface> current;

  std::mt19937 rand;
};

template <typename Surface>
template <typename... Args>
SaddleConnectionsSampleIterator<Surface>::SaddleConnectionsSampleIterator(PrivateConstructor, Args&&... args) :
  self(spimpl::make_impl<ImplementationOf<SaddleConnectionsSampleIterator>>(std::forward<Args>(args)...)) {}

}  // namespace flatsurf

#endif
