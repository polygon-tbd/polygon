#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################################
#  This file is part of flatsurf.
#
#        Copyright (C) 2019 Julian Rüth
#
#  flatsurf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  flatsurf is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with flatsurf. If not, see <https://www.gnu.org/licenses/>.
######################################################################

import sys
import pytest
import ctypes

from pyflatsurf import flatsurf, Surface
import flatsurf as sage_flatsurf
from pyeantic.sage_conversion import sage_nf_to_eantic, sage_nf_elem_to_eantic

def test_unfold_triangle():
    # Unfold the (5, 6, 7) triangle with sage-flatsurf
    T = sage_flatsurf.polygons.triangle(5, 6, 7)
    S = sage_flatsurf.similarity_surfaces.billiard(T)
    S = S.minimal_cover(cover_type="translation")

    # TODO: Make this a method in sage_flatsurf_conversion.py

    # Map sage-flatsurf's (face, id) to flatsurf.HalfEdge 
    edges = {}
    # Map flatsurf.HalfEdge to flatsurf.Vector
    vectors = {}

    K = None

    for face in S.label_iterator():
        for edge in [0, 1, 2]:
            label = (face, edge)

            if label in edges: continue

            edges[label] = len(edges) // 2 + 1
            edges[S.opposite_edge(*label)] = -edges[label]

            vector = S.polygon(face).edge(edge)

            if K is None:
                K = sage_nf_to_eantic(vector[0].parent())

            vector = flatsurf.Vector['eantic::renf_elem_class'](
                sage_nf_elem_to_eantic(K, vector[0]),
                sage_nf_elem_to_eantic(K, vector[1]))

            vectors[edges[label]] = vector
            vectors[edges[S.opposite_edge(*label)]] = -vector

    # The vectors for the HalfEdge 1 … n
    vectors = [vectors[i + 1] for i in range(len(vectors) // 2)]

    # Construct half edge permutation around vertices
    vertices = {}

    for face in S.label_iterator():
        for edge in [0, 1, 2]:
            label = (face, edge)
            nextLabel = S.opposite_edge(face, (edge + 2) % 3)
            vertices[edges[label]] = edges[nextLabel]

    cycles = []
    while vertices:
        key = next(iter(vertices.keys()))
        cycle = []
        while True:
            cycle.append(key)
            successor = vertices[key]
            del vertices[key]
            key = successor 
            if key == cycle[0]: break
        cycles.append(cycle)

    vertices = cycles

    surface = Surface(vertices, vectors)
    assert(str(surface) == "FlatTriangulationCombinatorial(vertices = (1, -3, -9, -21, -33, -45, -54, 47, 36, 24, 14, 6, -7, -17, -29, -38, 41, -42, -50, 48, 39, 27, -28, 25, 12, -13, 18, -19, -31, -43, -52, 46, 34, 22, 10, 4)(-1, -5, -12, 16, 7, 2)(-2, -6, -15, -27, -40, -46, 53, 45, -44, 38, 26, 13, 5, -4, -11, -24, -37, -48, 51, 43, 32, 21, -20, 17, -16, -25, 23, -22, -35, -47, 49, 42, 30, 19, 8, 3)(-8, -18, -26, 29, 20, 9)(-10, -23, 28, 15, -14, 11)(-30, -41, 44, 33, -32, 31)(-34, 40, -39, 37, -36, 35)(-49, 54, -53, 52, -51, 50), faces = (1, 2, 3)(-1, 4, 5)(-2, 7, 6)(-3, 8, 9)(-4, 10, 11)(-5, 13, 12)(-6, 14, 15)(-7, 16, 17)(-8, 19, 18)(-9, 20, 21)(-10, 22, 23)(-11, -14, 24)(-12, 25, -16)(-13, 26, -18)(-15, 28, 27)(-17, -20, 29)(-19, 30, 31)(-21, 32, 33)(-22, 34, 35)(-23, -25, -28)(-24, 36, 37)(-26, 38, -29)(-27, 39, 40)(-30, 42, 41)(-31, -32, 43)(-33, 44, 45)(-34, 46, -40)(-35, -36, 47)(-37, -39, 48)(-38, -44, -41)(-42, 49, 50)(-43, 51, 52)(-45, 53, 54)(-46, -52, -53)(-47, -54, -49)(-48, -50, -51)) with vectors 1: (1, 0), 2: ((a^4 - 9/2*a^2 + 2 ~ -0.407604), (3/2*a^5 - 13/2*a^3 + 3*a ~ 0.705990)), 3: ((-a^4 + 9/2*a^2 - 3 ~ -0.592396), (-3/2*a^5 + 13/2*a^3 - 3*a ~ -0.705990)), 4: ((a^4 - 9/2*a^2 + 3 ~ 0.592396), (-3/2*a^5 + 13/2*a^3 - 3*a ~ -0.705990)), 5: ((-a^4 + 9/2*a^2 - 2 ~ 0.407604), (3/2*a^5 - 13/2*a^3 + 3*a ~ 0.705990)), 6: ((a^4 - 9/2*a^2 + 3/2 ~ -0.907604), (3/2*a^5 - 7*a^3 + 9/2*a ~ -0.160035)), 7: ((1/2 ~ 0.500000), (1/2*a^3 - 3/2*a ~ 0.866025)), 8: ((-1/2*a^4 + 2*a^2 - 1 ~ -0.766044), (-3/2*a^5 + 13/2*a^3 - 5/2*a ~ 0.278817)), 9: ((-1/2*a^4 + 5/2*a^2 - 2 ~ 0.173648), (-1/2*a ~ -0.984808)), 10: ((1/2*a^4 - 5/2*a^2 + 2 ~ -0.173648), (-1/2*a ~ -0.984808)), 11: ((1/2*a^4 - 2*a^2 + 1 ~ 0.766044), (-3/2*a^5 + 13/2*a^3 - 5/2*a ~ 0.278817)), 12: ((-1/2 ~ -0.500000), (1/2*a^3 - 3/2*a ~ 0.866025)), 13: ((-a^4 + 9/2*a^2 - 3/2 ~ 0.907604), (3/2*a^5 - 7*a^3 + 9/2*a ~ -0.160035)), 14: ((-1/2*a^4 + 2*a^2 - 1 ~ -0.766044), (1/2*a^5 - 5/2*a^3 + 5/2*a ~ 0.642788)), 15: ((3/2*a^4 - 13/2*a^2 + 5/2 ~ -0.141559), (a^5 - 9/2*a^3 + 2*a ~ -0.802823)), 16: ((-2*a^4 + 9*a^2 - 4 ~ 0.815207), 0), 17: ((2*a^4 - 9*a^2 + 9/2 ~ -0.315207), (1/2*a^3 - 3/2*a ~ 0.866025)), 18: ((-1/2*a^4 + 2*a^2 - 1 ~ -0.766044), (-1/2*a^5 + 5/2*a^3 - 5/2*a ~ -0.642788)), 19: (0, (-a^5 + 4*a^3 ~ 0.921605)), 20: ((-2*a^4 + 17/2*a^2 - 7/2 ~ -0.624485), (-1/2*a^5 + 2*a^3 - 1/2*a ~ -0.524005)), 21: ((3/2*a^4 - 6*a^2 + 3/2 ~ 0.798133), (1/2*a^5 - 2*a^3 ~ -0.460802)), 22: ((-3/2*a^4 + 6*a^2 - 3/2 ~ -0.798133), (1/2*a^5 - 2*a^3 ~ -0.460802)), 23: ((2*a^4 - 17/2*a^2 + 7/2 ~ 0.624485), (-1/2*a^5 + 2*a^3 - 1/2*a ~ -0.524005)), 24: (0, (-a^5 + 4*a^3 ~ 0.921605)), 25: ((-2*a^4 + 9*a^2 - 9/2 ~ 0.315207), (1/2*a^3 - 3/2*a ~ 0.866025)), 26: ((-3/2*a^4 + 13/2*a^2 - 5/2 ~ 0.141559), (a^5 - 9/2*a^3 + 2*a ~ -0.802823)), 27: ((3/2*a^4 - 6*a^2 + 3/2 ~ 0.798133), (1/2*a^5 - 2*a^3 ~ -0.460802)), 28: ((-1/2*a^2 + 1 ~ -0.939693), (1/2*a^5 - 5/2*a^3 + 2*a ~ -0.342020)), 29: ((-1/2*a^2 + 1 ~ -0.939693), (-1/2*a^5 + 5/2*a^3 - 2*a ~ 0.342020)), 30: ((1/2*a^4 - 2*a^2 + 1 ~ 0.766044), (-3/2*a^5 + 13/2*a^3 - 5/2*a ~ 0.278817)), 31: ((-1/2*a^4 + 2*a^2 - 1 ~ -0.766044), (1/2*a^5 - 5/2*a^3 + 5/2*a ~ 0.642788)), 32: ((3/2*a^4 - 13/2*a^2 + 5/2 ~ -0.141559), (a^5 - 9/2*a^3 + 2*a ~ -0.802823)), 33: ((1/2*a^2 - 1 ~ 0.939693), (-1/2*a^5 + 5/2*a^3 - 2*a ~ 0.342020)), 34: ((-1/2*a^2 + 1 ~ -0.939693), (-1/2*a^5 + 5/2*a^3 - 2*a ~ 0.342020)), 35: ((-3/2*a^4 + 13/2*a^2 - 5/2 ~ 0.141559), (a^5 - 9/2*a^3 + 2*a ~ -0.802823)), 36: ((1/2*a^4 - 2*a^2 + 1 ~ 0.766044), (1/2*a^5 - 5/2*a^3 + 5/2*a ~ 0.642788)), 37: ((-1/2*a^4 + 2*a^2 - 1 ~ -0.766044), (-3/2*a^5 + 13/2*a^3 - 5/2*a ~ 0.278817)), 38: ((-3/2*a^4 + 6*a^2 - 3/2 ~ -0.798133), (1/2*a^5 - 2*a^3 ~ -0.460802)), 39: ((-1/2*a^4 + 5/2*a^2 - 2 ~ 0.173648), (-1/2*a ~ -0.984808)), 40: ((2*a^4 - 17/2*a^2 + 7/2 ~ 0.624485), (1/2*a^5 - 2*a^3 + 1/2*a ~ 0.524005)), 41: ((-1/2*a^4 + 5/2*a^2 - 2 ~ 0.173648), (1/2*a ~ 0.984808)), 42: ((a^4 - 9/2*a^2 + 3 ~ 0.592396), (-3/2*a^5 + 13/2*a^3 - 3*a ~ -0.705990)), 43: ((a^4 - 9/2*a^2 + 3/2 ~ -0.907604), (3/2*a^5 - 7*a^3 + 9/2*a ~ -0.160035)), 44: ((2*a^4 - 17/2*a^2 + 7/2 ~ 0.624485), (-1/2*a^5 + 2*a^3 - 1/2*a ~ -0.524005)), 45: ((-2*a^4 + 9*a^2 - 9/2 ~ 0.315207), (1/2*a^3 - 3/2*a ~ 0.866025)), 46: ((2*a^4 - 9*a^2 + 9/2 ~ -0.315207), (1/2*a^3 - 3/2*a ~ 0.866025)), 47: ((-a^4 + 9/2*a^2 - 3/2 ~ 0.907604), (3/2*a^5 - 7*a^3 + 9/2*a ~ -0.160035)), 48: ((-a^4 + 9/2*a^2 - 3 ~ -0.592396), (-3/2*a^5 + 13/2*a^3 - 3*a ~ -0.705990)), 49: ((a^4 - 9/2*a^2 + 2 ~ -0.407604), (-3/2*a^5 + 13/2*a^3 - 3*a ~ -0.705990)), 50: (1, 0), 51: ((a^4 - 9/2*a^2 + 2 ~ -0.407604), (3/2*a^5 - 13/2*a^3 + 3*a ~ 0.705990)), 52: ((-1/2 ~ -0.500000), (-1/2*a^3 + 3/2*a ~ -0.866025)), 53: ((-2*a^4 + 9*a^2 - 4 ~ 0.815207), 0), 54: ((-1/2 ~ -0.500000), (1/2*a^3 - 3/2*a ~ 0.866025))")

    return surface

if __name__ == '__main__': sys.exit(pytest.main(sys.argv))