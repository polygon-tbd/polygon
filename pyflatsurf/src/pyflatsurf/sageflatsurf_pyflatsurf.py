# -*- coding: utf-8 -*-
r"""
Interface with sage-flatsurf

See https://github.com/videlec/sage-flatsurf
"""
#*********************************************************************
#  This file is part of flatsurf.
#
#        Copyright (C) 2019 Vincent Delecroix
#        Copyright (C) 2019 Julian Rüth
#
#  Flatsurf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
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

try:
    from sage.all import QQ
except ImportError:
    raise ImportError("sageflatsurf_pyflatsurf needs SageMath, see http://www.sagemath.org/")

try:
    import flatsurf as sage_flatsurf
except ImportError:
    raise ImportError("sageflatsurf_pyflatsurf needs sage-flatsurf, see https://github.com/videlec/sage-flatsurf")

from .cppyy_flatsurf import flatsurf

def _check_data(vp, fp, vec):
    r"""
    Check consistency of data

    vp - vector permutation
    fp - face permutation
    vec - vectors of the flat structure
    """
    assert isinstance(vp, list)
    assert isinstance(fp, list)
    assert isinstance(vec, list)

    n = len(vp) - 1

    assert n%2 == 0, n
    assert len(fp) == n+1
    assert len(vec) == n//2

    assert vp[0] is None
    assert fp[0] is None

    for i in range(1, n//2+1):
        # check fp/vp consistency
        assert fp[-vp[i]] == i, i

        # check that each face is a triangle and that vectors sum up to zero
        j = fp[i]
        k = fp[j]
        assert i != j and i != k and fp[k] == i, (i, j, k)
        vi = vec[i-1] if i >= 1 else -vec[-i-1]
        vj = vec[j-1] if j >= 1 else -vec[-j-1]
        vk = vec[k-1] if k >= 1 else -vec[-k-1]
        v = vi + vj + vk
        assert v.x() == 0, v.x()
        assert v.y() == 0, v.y()

def _cycle_decomposition(p):
    n = len(p) - 1
    assert n%2 == 0
    cycles = []
    unseen = [True] * (n+1)
    for i in list(range(-n//2+1, 0)) + list(range(1, n//2)):
        if unseen[i]:
            j = i
            cycle = []
            while unseen[j]:
                unseen[j] = False
                cycle.append(j)
                j = p[j]
            cycles.append(cycle)
    return cycles

def flatsurf_to_pyflatsurf(S):
    r"""
    Given S a translation surface from flatsurf builds a flat polygonization

    Examples

    >>> import sage.all
    >>> import flatsurf
    >>> from pyflatsurf.sageflatsurf_pyflatsurf import flatsurf_to_pyflatsurf

    Building the regular 2n-gons for n=5,7 (Veech surfaces)

    >>> S5 = flatsurf.translation_surfaces.veech_double_n_gon(5)
    >>> T5 = flatsurf_to_pyflatsurf(S5)

    >>> S7 = flatsurf.translation_surfaces.veech_double_n_gon(7)
    >>> T7 = flatsurf_to_pyflatsurf(S7)

    Arnoux-Yoccoz surfaces in genus 3 and 4

    >>> A3 = flatsurf.translation_surfaces.arnoux_yoccoz(3)
    >>> B3 = flatsurf_to_pyflatsurf(A3)

    >>> A4 = flatsurf.translation_surfaces.arnoux_yoccoz(4)
    >>> B4 = flatsurf_to_pyflatsurf(A4)

    Ward surfaces (a regular 2n-gon glued to two regular n-gons)

    >>> W3 = flatsurf.translation_surfaces.ward(3)
    >>> X3 = flatsurf_to_pyflatsurf(W3)

    >>> W17 = flatsurf.translation_surfaces.ward(17)
    >>> X17 = flatsurf_to_pyflatsurf(W17)
    """
    if not isinstance(S, sage_flatsurf.geometry.translation_surface.TranslationSurface):
        raise TypeError("S must be a translation surface")
    if not S.is_finite():
        raise ValueError("the surface S must be finite")

    S = S.triangulate()

    # number of half edges
    n = sum(S.polygon(lab).num_edges() for lab in S.label_iterator())

    if S.base_ring() is QQ:
        # see https://github.com/flatsurf/flatsurf/issues/83
        raise NotImplementedError
    else:
        from pyeantic.sage_conversion import sage_nf_to_eantic, sage_nf_elem_to_eantic
        K = sage_nf_to_eantic(S.base_ring())
        make_K_elem = lambda x: sage_nf_elem_to_eantic(K, x)
        V = flatsurf.Vector['eantic::renf_elem_class']

    # populate half edges and vectors
    half_edge_labels = {}   # map: (face lab, edge num) in faltsurf -> integer
    vec = []                # vectors
    k = 1                   # half edge label in {1, ..., n}
    for t0, t1 in S.edge_iterator(gluings=True):
        if t0 in half_edge_labels:
            continue

        half_edge_labels[t0] = k
        half_edge_labels[t1] = -k

        f0, e0 = t0
        p = S.polygon(f0)
        e = p.edge(e0)
        v0 = make_K_elem(e[0])
        v1 = make_K_elem(e[1])
        vec.append(V(v0, v1))

        k += 1

    # compute vertex and face permutations
    vp = [None] * (n+1)  # vertex permutation
    fp = [None] * (n+1)  # face permutation
    for t in S.edge_iterator(gluings=False):
        e = half_edge_labels[t]
        j = (t[1] + 1) % S.polygon(t[0]).num_edges()
        fp[e] = half_edge_labels[(t[0], j)]
        vp[fp[e]] = -e

    _check_data(vp, fp, vec)

    # convert the vp permutation into cycles
    verts = _cycle_decomposition(vp)

    return flatsurf.Surface(verts, vec)