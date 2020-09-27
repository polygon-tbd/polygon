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

#ifndef LIBFLATSURF_COPYABLE_HPP
#define LIBFLATSURF_COPYABLE_HPP

#include "external/spimpl/spimpl.h"
#include "forward.hpp"

namespace flatsurf {

// A pointer-to-implementation (pimpl) for types that are copyable and
// moveable. Such types are copied by copying this underlying pimpl.
template <typename T>
using Copyable = spimpl::impl_ptr<ImplementationOf<T>>;

}  // namespace flatsurf

#endif
