# -*- coding: utf-8 -*-
r"""
Vector wrapper and sage conversion for ``flatsurf.Vector``.
"""
#*********************************************************************
#  This file is part of flatsurf.
#
#        Copyright (C) 2020 Vincent Delecroix
#                      2020 Julian Rüth
#
#  Flatsurf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Flatsurf is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with flatsurf. If not, see <https://www.gnu.org/licenses/>.
#*********************************************************************

import cppyy
import gmpxxyy

from pyflatsurf import flatsurf
from gmpxxyy import mpz, mpq
from pyeantic import eantic, real_embedded_number_field
from pyexactreal.exact_reals import ExactReals

from sage.all import ZZ, QQ, FreeModule, FreeModules, Morphism, Hom, SetsWithPartialMaps, NumberFields
from sage.rings.number_field.number_field_base import NumberField as SageNumberField
from sage.structure.element import Vector as SageVector
from sage.structure.parent import Parent
from sage.structure.unique_representation import UniqueRepresentation

class Vector(SageVector):
    r"""
    A two-dimensional vector that wraps ``flatsurf.Vector``.

    EXAMPLES::

        sage: from pyflatsurf import flatsurf
        sage: from pyflatsurf.vector import Vectors

    A vector with integer coordinates::

        sage: from gmpxxyy import mpz
        sage: V = Vectors(ZZ)
        sage: V(1, 2)
        (1, 2)
        sage: V(flatsurf.Vector[mpz](1, 2))
        (1, 2)

    A vector with rational coordinates::

        sage: from gmpxxyy import mpz, mpq
        sage: V = Vectors(QQ)
        sage: V(1/3, -2/7)
        (1/3, -2/7)
        sage: V(flatsurf.Vector[mpq]('1/3', '-2/7'))
        (1/3, -2/7)

    A vector with number field coordinates::

        sage: from pyeantic import eantic, RealEmbeddedNumberField
        sage: x = polygen(QQ)
        sage: K = NumberField(x^3 - 2, 'a', embedding=AA(2)**(1/3))
        sage: R = RealEmbeddedNumberField(K)
        sage: V = Vectors(K)
        sage: x = R(K.gen())
        sage: y = R(K.gen()**2 - 2/3)
        sage: V(x, y)
        ((a ~ 1.2599210), (a^2 - 2/3 ~ 0.92073439))
        sage: V(flatsurf.Vector[eantic.renf_elem_class](x.renf_elem, y.renf_elem))
        ((a ~ 1.2599210), (a^2 - 2/3 ~ 0.92073439))

    A vector with real coordinates::

        sage: from pyexactreal import ExactReals
        sage: R = ExactReals()
        sage: V = Vectors(R)
        sage: x = R.random_element()
        sage: y = R.random_element()
        sage: V(x, y)
        (ℝ(0...…), ℝ(0...…))

    """
    def __init__(self, parent, vector, y = None):
        SageVector.__init__(self, parent)

        if y is not None:
            vector = (vector, y)

        R = parent.base_ring()

        if isinstance(vector, parent.Vector):
            self.vector = vector
        elif isinstance(vector[0], parent.coordinate) and isinstance(vector[1], parent.coordinate):
            self.vector = parent.Vector(*vector)
        else:
            self.vector = parent.Vector(parent._to_coordinate(vector[0]), parent._to_coordinate(vector[1]))

    def _repr_(self):
        return repr(self.vector)

    def monomial_coefficients(self, *args, **kwargs):
        r"""
        Return the coefficients of this vector.

        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(ZZ)
            sage: v = V(13, 37)
            sage: v.monomial_coefficients()
            {0: 13, 1: 37}

        """
        coefficients = {}
        x = self.parent().base_ring()(self.vector.x())
        y = self.parent().base_ring()(self.vector.y())
        if x:
            coefficients[0] = x
        if y:
            coefficients[1] = y
        return coefficients

    def __iter__(self):
        r"""
        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(ZZ)
            sage: v = V(13, 37)
            sage: list(v)
            [13, 37]

        """
        yield self.parent().base_ring()(self.vector.x())
        yield self.parent().base_ring()(self.vector.y())

    def _add_(self, other):
        r"""
        TESTS::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(QQ)
            sage: V((1,1)) + V((2,-1))
            (3, 0)
        """
        P = self.parent()
        return P.element_class(P, self.vector + other.vector)

    def _sub_(self, other):
        r"""
        TESTS::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(QQ)
            sage: V((1,1)) - V((2,-1))
            (-1, 2)
        """
        P = self.parent()
        return P.element_class(P, self.vector - other.vector)

    def _neg_(self):
        r"""
        TESTS::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(QQ)
            sage: -V((1,-2))
            (-1, 2)
        """
        P = self.parent()
        return P.element_class(P, -self.vector)

    def _rmul_(self, scalar):
        r"""
        TESTS::

            sage: from pyflatsurf.vector import Vectors

            sage: V = Vectors(ZZ)
            sage: 3 * V((1,2))
            (3, 6)

            sage: V = Vectors(QQ)
            sage: V((2, 5)) / 7
            (2/7, 5/7)

            sage: from pyeantic import RealEmbeddedNumberField
            sage: K = NumberField(x**2 - 2, 'a', embedding=sqrt(AA(2)))
            sage: R = RealEmbeddedNumberField(K)
            sage: V = Vectors(K)
            sage: v = V((1,K.gen() - 3/2))
            sage: v
            (1, (a-3/2 ~ -0.085786438))
            sage: 3 * v
            (3, (3*a-9/2 ~ -0.25735931))
            sage: K.gen() * v
            ((a ~ 1.4142136), (-3/2*a+2 ~ -0.12132034))
            sage: R.gen() * v
            ((a ~ 1.4142136), (-3/2*a+2 ~ -0.12132034))

            sage: v * 3
            (3, (3*a-9/2 ~ -0.25735931))
            sage: v * K.gen()
            ((a ~ 1.4142136), (-3/2*a+2 ~ -0.12132034))
            sage: v * R.gen()
            ((a ~ 1.4142136), (-3/2*a+2 ~ -0.12132034))

            sage: v / 3
            ((1/3 ~ 0.33333333), (1/3*a-1/2 ~ -0.028595479))
            sage: v / K.gen()
            ((1/2*a ~ 0.70710678), (-3/4*a+1 ~ -0.060660172))
            sage: v / R.gen()
            ((1/2*a ~ 0.70710678), (-3/4*a+1 ~ -0.060660172))
        """
        scalar = self.parent()._to_coordinate(scalar)

        return self.parent()(scalar * self.vector.x(), scalar * self.vector.y())

    _lmul_ = _rmul_

    def __bool__(self):
        r"""
        Return whether this is a non-zero vector.

        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(ZZ)
            sage: bool(V(0, 0))
            False
            sage: bool(V(1, 0))
            True

        """
        return bool(self.vector)

    def decomposition(self, base):
        r"""
        Write this vector as a shortest sum `v = \sum a_i v_i` where the `v_i`
        are vectors with entries in ``base`` and the `a_i` are coefficients from our base ring.

        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(ZZ)
            sage: v = V(13, 37)
            sage: v.decomposition(ZZ)
            [(1, (13, 37))]
            sage: v = V(0, 0)
            sage: v.decomposition(ZZ)
            []

        ::

            sage: from pyexactreal import ExactReals
            sage: R = ExactReals()
            sage: V = Vectors(R)
            sage: v = V(R.random_element(), R.random_element())
            sage: v.decomposition(QQ)
            [(ℝ(0.178808…), (1, 0)), (ℝ(0.478968…), (0, 1))]

        ::

            sage: from pyflatsurf.vector import Vectors
            sage: from pyexactreal import ExactReals
            sage: from pyeantic import RealEmbeddedNumberField
            sage: K = NumberField(x^3 - 2, 'a', embedding=AA(2)**(1/3))
            sage: R = ExactReals(K)
            sage: V = Vectors(R)
            sage: v = V(K.random_element() * R.random_element(), R.random_element())
            sage: v.decomposition(K)
            [(ℝ(0.621222…), (0, 1)), (ℝ(0.782515…), (-1/2*a^2 - 4, 0))]

        """
        if not self: return []
        if base is self.parent().base_ring():
            return [(base.one(), self)]
        if base is self.parent()._algebraic_ring():
            if isinstance(self.parent().base_ring(), ExactReals):
                from functools import reduce
                module = type(self.vector.x().module()).span(self.vector.x().module(), self.vector.y().module())

                vector = [entry.promote(module) for entry in [self.vector.x(), self.vector.y()]]
                vector = [entry.coefficients() for entry in vector]

                vector = [[base(Vectors(base).base_ring()(c)) for c in coefficients] for coefficients in vector]

                V = base**2
                return [(self.parent().base_ring()(module.gen(i)), V(coefficients)) for (i, coefficients) in enumerate(zip(*vector)) if any(coefficients)]

        raise NotImplementedError("cannot decompose vector in %s over %s"%(self.parent(), base))


class Vectors(UniqueRepresentation, Parent):
    r"""
    The subspace of the vector space `\mathbb{R}^2` modeled by vectors with
    entries in ``base_ring``. Unlike normal SageMath vector spaces, this space
    is backed by flatsur's ``Vector`` class with entries in ``coordinate``.

    EXAMPLES::

        sage: from pyflatsurf.vector import Vectors
        sage: K = NumberField(x**2 - 2, 'a', embedding=sqrt(AA(2)))
        sage: V = Vectors(K)
        sage: v = V((1,K.gen() - 3/2))
        sage: v
        (1, (a-3/2 ~ -0.085786438))
        sage: VectorSpace(K, 2)(v)
        (1, a - 3/2)

        sage: from pyeantic import RealEmbeddedNumberField
        sage: R = RealEmbeddedNumberField(K)
        sage: assert Vectors(R) is V
        sage: assert Vectors(R.renf) is V

        sage: from gmpxxyy import mpz, mpq
        sage: assert Vectors(QQ) is Vectors(mpq)
        sage: assert Vectors(ZZ) is Vectors(mpz)
    """
    Element = Vector

    @staticmethod
    def __classcall__(cls, base_ring):
        if base_ring in [ZZ, mpz]:
            base_ring = ZZ
            coordinate = mpz
        elif base_ring in [QQ, mpq]:
            base_ring = QQ
            coordinate = mpq
        elif isinstance(base_ring, real_embedded_number_field.RealEmbeddedNumberField):
            coordinate = cppyy.gbl.eantic.renf_elem_class
        elif isinstance(base_ring, eantic.renf_class):
            base_ring = real_embedded_number_field.RealEmbeddedNumberField(base_ring)
            coordinate = cppyy.gbl.eantic.renf_elem_class
        elif isinstance(base_ring, SageNumberField):
            base_ring = real_embedded_number_field.RealEmbeddedNumberField(base_ring)
            coordinate = cppyy.gbl.eantic.renf_elem_class
        elif isinstance(base_ring, ExactReals):
            coordinate = base_ring._element_factory
        else:
            raise NotImplementedError("unsupported base_ring {!r}".format(base_ring))

        return super(Vectors, cls).__classcall__(cls, base_ring, coordinate)

    def __init__(self, base_ring, coordinate):
        self.coordinate = coordinate

        self.Vector = flatsurf.Vector[self.coordinate]

        self._isomorphic_vector_space = FreeModule(base_ring, 2)
        if isinstance(base_ring, real_embedded_number_field.RealEmbeddedNumberField):
            self._isomorphic_vector_space = FreeModule(base_ring.number_field, 2)

        Parent.__init__(self, base_ring, category=FreeModules(base_ring))

        self.register_coercion(self._isomorphic_vector_space)
        self._isomorphic_vector_space.register_conversion(ConversionVectorSpace(self))

    def _algebraic_ring(self):
        r"""
        Return the algebraic ring which underlies our base ring; typically this
        is the base ring itself but for non-algebraic rings such as
        ``ExactReals``, this gives the coefficient ring.

        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: from pyexactreal import ExactReals
            sage: R = ExactReals()
            sage: V = Vectors(R)
            sage: V._algebraic_ring()
            Rational Field

        """
        algebraic_ring = self.base_ring()

        if isinstance(algebraic_ring, ExactReals):
            algebraic_ring = algebraic_ring.base_ring()
            if algebraic_ring not in [ZZ, QQ]: algebraic_ring = algebraic_ring.number_field

        return algebraic_ring

    def _to_coordinate(self, x):
        r"""
        Convert ``x`` to something that the flatsurf backend for this vector type understands.

        EXAMPLES::

            sage: from pyflatsurf.vector import Vectors
            sage: V = Vectors(QQ)
            sage: type(V._to_coordinate(1))
            <class cppyy.gbl.__gmp_expr<__mpz_struct[1],__mpz_struct[1]> at ...>
            sage: type(V._to_coordinate(1/2))
            <class cppyy.gbl.__gmp_expr<__mpq_struct[1],__mpq_struct[1]> at ...>

        """
        if isinstance(x, self.coordinate):
            return x
        if isinstance(x, cppyy.gbl.mpz_class):
            return x
        if isinstance(x, cppyy.gbl.mpq_class):
            return x
        if x in ZZ:
            return cppyy.gbl.mpz_class(str(x))
        if x in QQ:
            return cppyy.gbl.mpq_class(str(x))
        if isinstance(self.base_ring(), real_embedded_number_field.RealEmbeddedNumberField):
            return self.base_ring()(x).renf_elem
        if isinstance(self.base_ring(), ExactReals):
            return self.base_ring()(x)._backend

        raise NotImplementedError("Cannot convert %s to something the flatsurf backend understands yet, i.e., cannot convert a %s into a %s"%(x, type(x), type(self.coordinate)))

    def _repr_(self):
        return repr(self._isomorphic_vector_space)

    def an_element(self):
        return self((1,0))


class ConversionVectorSpace(Morphism):
    r"""
    A conversion from flatsurf ``Vectors`` to SageMath ``VectorSpace``.
    """
    def __init__(self, domain):
        Morphism.__init__(self, Hom(domain, domain._isomorphic_vector_space, SetsWithPartialMaps()))

    def _call_(self, v):
        r"""
        TESTS::

            sage: from pyflatsurf.vector import Vectors

            sage: V = Vectors(ZZ)
            sage: v = V((1,-2))
            sage: FreeModule(ZZ, 2)(v)
            (1, -2)

            sage: V = Vectors(QQ)
            sage: v = V((1, 2/3))
            sage: VectorSpace(QQ, 2)(v)
            (1, 2/3)

            sage: K = NumberField(x**2 - 2, 'a', embedding=sqrt(AA(2)))
            sage: V = Vectors(K)
            sage: v = V((1, K.gen() - 3/2))
            sage: VectorSpace(K, 2)(v)
            (1, a - 3/2)
        """
        x = v.vector.x()
        y = v.vector.y()
        R = self.domain().base_ring()
        return self.codomain()((R(x), R(y)))
