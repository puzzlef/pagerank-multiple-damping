#pragma once
#include <vector>
#include "_main.hxx"
#include "transpose.hxx"
#include "pagerank.hxx"
#include "pagerankMonolithicSeq.hxx"

using std::vector;




// PAGERANK (STATIC / INCREMENTAL)
// -------------------------------

// Find pagerank using a single thread (pull, CSR).
// @param x  original graph
// @param xt transpose graph (with vertex-data=out-degree)
// @param qs vector initial ranks (optional)
// @param os vector options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns [{ranks, iterations, time}]
template <class G, class H, class T=float>
vector<PagerankResult<T>> pagerankMonolithicSeqScalar(const G& x, const H& xt, const vector2d<T> *qs=nullptr, const vector<PagerankOptions<T>>& os={}) {
  vector<PagerankResult<T>> a;
  for (int i=0; i<os.size(); i++)
    a.push_back(pagerankMonolithicSeq(x, xt, qs[i], os[i]));
  return a;
}

template <class G, class T=float>
vector<PagerankResult<T>> pagerankMonolithicSeqScalar(const G& x, const vector2d<T> *qs=nullptr, const vector<PagerankOptions<T>>& os={}) {
  auto xt = transposeWithDegree(x);
  return pagerankMonolithicSeqScalar(x, xt, qs, os);
}




// PAGERANK (DYNAMIC)
// ------------------

template <class G, class H, class T=float>
vector<PagerankResult<T>> pagerankMonolithicSeqScalarDynamic(const G& x, const H& xt, const G& y, const H& yt, const vector2d<T> *qs=nullptr, const vector<PagerankOptions<T>>& os={}) {
  vector<PagerankResult<T>> a;
  for (int i=0; i<os.size(); i++)
    a.push_back(pagerankMonolithicSeqDynamic(x, xt, y, yt, qs[i], os[i]));
  return a;
}

template <class G, class T=float>
vector<PagerankResult<T>> pagerankMonolithicSeqScalarDynamic(const G& x, const G& y, const vector2d<T> *qs=nullptr, const vector<PagerankOptions<T>>& os={}) {
  auto xt = transposeWithDegree(x);
  auto yt = transposeWithDegree(y);
  return pagerankMonolithicSeqScalarDynamic(x, xt, y, yt, qs, os);
}
