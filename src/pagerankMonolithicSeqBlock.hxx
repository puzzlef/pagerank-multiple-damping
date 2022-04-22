#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "transpose.hxx"
#include "dynamic.hxx"
#include "pagerank.hxx"
#include "pagerankSeq.hxx"

using std::vector;
using std::swap;




// PAGERANK-LOOP
// -------------

template <class T>
int pagerankMonolithicSeqBlockLoopU(vector<T>& a, vector<T>& r, vector<T>& c, const vector<T>& f, const vector<int>& vfrom, const vector<int>& efrom, const vector<int>& vdata, int i, int n, int N, const vector<T>& p, T E, int L, int EF) {
  int l = 0;
  int B = p.size(), b = B;
  vector<T> c0(B), el(B);
  while (l<L) {
    pagerankTeleportBlockW(c0, B, b, r, vdata, N, p);
    // T c0 = pagerankTeleport(r, vdata, N, p);
    pagerankCalculateBlockW(a, B, b, c, vfrom, efrom, i, n, c0);  // assume contribtions (c) is precalculated
    pagerankErrorBlockW(el, B, b, a, r, i, n, EF); l += b;        // b iterations complete
    b = findIf(el, [&](auto e) { return e<E || l>=L; });          // check tolerance, iteration limit
    if (b == 0) break;                                            // check tolerance, iteration limit
    multiplyValuesBlockW(c, B, b, a, f, i, n);                    // update partial contributions (c)
    swap(a, r);                                                   // final ranks always in (a)
  }
  return l;
}




// PAGERANK (STATIC / INCREMENTAL)
// -------------------------------

// Find pagerank using a single thread (pull, CSR).
// @param x  original graph
// @param xt transpose graph (with vertex-data=out-degree)
// @param q  initial ranks (optional)
// @param o  options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class H, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqBlock(const G& x, const H& xt, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  int  N  = xt.order();  if (N==0) return PagerankMultiResult<T>::initial(xt, qs);
  auto ks = vertexKeys(xt);
  return pagerankSeqBlock(xt, ks, 0, N, pagerankMonolithicSeqBlockLoopU<T>, qs, os);
}

template <class G, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqBlock(const G& x, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  auto xt = transposeWithDegree(x);
  return pagerankMonolithicSeqBlock(x, xt, qs, os);
}




// PAGERANK (DYNAMIC)
// ------------------

template <class G, class H, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqBlockDynamic(const G& x, const H& xt, const G& y, const H& yt, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  int  N = yt.order();                             if (N==0) return PagerankMultiResult<T>::initial(yt, qs);
  auto [ks, n] = dynamicInVertices(x, xt, y, yt);  if (n==0) return PagerankMultiResult<T>::initial(yt, qs);
  return pagerankSeqBlock(yt, ks, 0, n, pagerankMonolithicSeqBlockLoopU<T>, qs, os);
}

template <class G, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqBlockDynamic(const G& x, const G& y, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  auto xt = transposeWithDegree(x);
  auto yt = transposeWithDegree(y);
  return pagerankMonolithicSeqBlockDynamic(x, xt, y, yt, qs, os);
}
