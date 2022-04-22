#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "pagerank.hxx"

using std::vector;
using std::swap;




// PAGERANK-FACTOR
// ---------------
// For contribution factors of vertices (unchanging).

template <class T>
void pagerankFactorW(vector<T>& a, const vector<int>& vdata, int i, int n, T p) {
  for (int u=i; u<i+n; u++) {
    int d = vdata[u];
    a[u] = d>0? p/d : 0;
  }
}


template <class T>
void pagerankFactorBlockW(vector<T>& a, int B, int b, const vector<int>& vdata, int i, int n, const vector<T>& p) {
  for (int u=i; u<i+n; u++) {
    int d = vdata[u];
    for (int o=0; o<b; o++)
      a[u*B+o] = d>0? p[o]/d : 0;
  }
}




// PAGERANK-TELEPORT
// -----------------
// For teleport contribution from vertices (inc. dead ends).

template <class T>
T pagerankTeleport(const vector<T>& r, const vector<int>& vdata, int N, T p) {
  T a = (1-p)/N;
  for (int u=0; u<N; u++)
    if (vdata[u] == 0) a += p*r[u]/N;
  return a;
}


template <class T>
void pagerankTeleportBlockW(vector<T>& a, int B, int b, const vector<T>& r, const vector<int>& vdata, int N, const vector<T>& p) {
  for (int o=0; o<b; o++)
    a[o] = (1-p[o])/N;
  for (int u=0; u<N; u++) {
    if (vdata[u] != 0) continue;
    for (int o=0; o<b; o++)
      a[o] += p[o]*r[u*B+o]/N;
  }
}




// PAGERANK-CALCULATE
// ------------------
// For rank calculation from in-edges.

template <class T>
void pagerankCalculateW(vector<T>& a, const vector<T>& c, const vector<int>& vfrom, const vector<int>& efrom, int i, int n, T c0) {
  for (int v=i; v<i+n; v++)
    a[v] = c0 + sumValuesAt(c, sliceIterable(efrom, vfrom[v], vfrom[v+1]));
}


template <class T>
void pagerankCalculateBlockW(vector<T>& a, int B, int b, const vector<T>& c, const vector<int>& vfrom, const vector<int>& efrom, int i, int n, const vector<T>& c0) {
  for (int v=i; v<i+n; v++) {
    copyValues(c0, a, v*B, b);
    sumValuesAtBlockU(a, v, B, b, c, 0, sliceIterable(efrom, vfrom[v], vfrom[v+1]));
  }
}




// PAGERANK-ERROR
// --------------
// For convergence check.

template <class T>
T pagerankError(const vector<T>& x, const vector<T>& y, int i, int N, int EF) {
  switch (EF) {
    case 1:  return l1Norm(x, y, i, N);
    case 2:  return l2Norm(x, y, i, N);
    default: return liNorm(x, y, i, N);
  }
}


template <class T>
void pagerankErrorBlockW(vector<T>& a, int B, int b, const vector<T>& x, const vector<T>& y, int i, int N, int EF) {
  fillValueU(a, 0, b, T());
  switch (EF) {
    case 1:  l1NormBlockU(a, B, b, x, y, i, N); break;
    case 2:  l2NormBlockU(a, B, b, x, y, i, N); break;
    default: liNormBlockU(a, B, b, x, y, i, N); break;
  }
}




// PAGERANK
// --------
// For Monolithic / Componentwise PageRank.

template <class H, class J, class M, class FL, class T=float>
PagerankResult<T> pagerankSeq(const H& xt, const J& ks, int i, const M& ns, FL fl, const vector<T> *q, const PagerankOptions<T>& o) {
  int  N  = xt.order();
  T    p  = o.damping;
  T    E  = o.tolerance;
  int  L  = o.maxIterations, l = 0;
  int  EF = o.toleranceNorm;
  auto vfrom = sourceOffsetsAs(xt, ks, int());
  auto efrom = destinationIndicesAs(xt, ks, int());
  auto vdata = vertexData(xt, ks);
  vector<T> a(N), r(N), c(N), f(N), qc;
  if (q) qc = compressContainer(xt, *q, ks);
  float t = measureDurationMarked([&](auto mark) {
    if (q) copyValuesW(r, qc);   // copy old ranks (qc), if given
    else fillValueU(r, T(1)/N);
    copyValuesW(a, r);
    mark([&] { pagerankFactorW(f, vdata, 0, N, p); multiplyValuesW(c, a, f, 0, N); });  // calculate factors (f) and contributions (c)
    mark([&] { l = fl(a, r, c, f, vfrom, efrom, vdata, i, ns, N, p, E, L, EF); });      // calculate ranks of vertices
  }, o.repeat);
  return {decompressContainer(xt, a, ks), l, t};
}


template <class H, class J, class M, class FL, class T=float>
PagerankMultiResult<T> pagerankSeqBlock(const H& xt, const J& ks, int i, const M& ns, FL fl, const vector2d<T> *qs, const PagerankMultiOptions<T>& os) {
  int  N  = xt.order();
  int  B  = os.damping.size();
  auto p  = os.damping;
  T    E  = os.tolerance;
  int  L  = os.maxIterations, l = 0;
  int  EF = os.toleranceNorm;
  auto vfrom = sourceOffsetsAs(xt, ks, int());
  auto efrom = destinationIndicesAs(xt, ks, int());
  auto vdata = vertexData(xt, ks);
  vector<T>   a(N*B), r(N*B), c(N*B), f(N*B), qc;
  vector2d<T> as, qcs;
  if (qs) {
    for (const auto& q : *qs)
      qcs.push_back(compressContainer(xt, q, ks));
    qc = joinBlocks(B, B, qcs);
  }
  float t = measureDurationMarked([&](auto mark) {
    if (q) copyValuesW(r, qc);   // copy old ranks (qc), if given
    else fillValueU(r, T(1)/N);
    copyValuesW(a, r);
    mark([&] { pagerankFactorBlockW(f, B, B, vdata, 0, N, p); multiplyValuesW(c, a, f, 0, N*B); });  // calculate factors (f) and contributions (c)
    mark([&] { l = fl(a, r, c, f, vfrom, efrom, vdata, i, ns, N, p, E, L, EF); });      // calculate ranks of vertices
  }, os.repeat);
  as = splitBlocks(B, B, a);
  for (auto& a : as)
    a = decompressContainer(xt, a, ks);
  return {as, l, t};
}
