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
PagerankMultiResult<T> pagerankMonolithicSeqSingle(const G& x, const H& xt, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  PagerankMultiResult<T> as;
  for (int i=0; i<o.damping.size(); i++) {
    auto a = pagerankMonolithicSeq(x, xt, qs? *qs[i] : nullptr, os.option(i));
    as.ranks.push_back(move(a.ranks));
    as.time += a.time;
    as.iterations += a.iterations;
  }
  return as;
}

template <class G, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqSingle(const G& x, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  auto xt = transposeWithDegree(x);
  return pagerankMonolithicSeqSingle(x, xt, qs, os);
}




// PAGERANK (DYNAMIC)
// ------------------

template <class G, class H, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqSingleDynamic(const G& x, const H& xt, const G& y, const H& yt, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  PagerankMultiResult<T> as;
  for (int i=0; i<o.damping.size(); i++) {
    auto a = pagerankMonolithicSeqDynamic(x, xt, y, yt, qs? *qs[i] : nullptr, os.option(i))
    as.ranks.push_back(move(a.ranks));
    as.time += a.time;
    as.iterations += a.iterations;
  }
  return as;
}

template <class G, class T=float>
PagerankMultiResult<T> pagerankMonolithicSeqSingleDynamic(const G& x, const G& y, const vector2d<T> *qs=nullptr, const PagerankMultiOptions<T>& os={}) {
  auto xt = transposeWithDegree(x);
  auto yt = transposeWithDegree(y);
  return pagerankMonolithicSeqSingleDynamic(x, xt, y, yt, qs, os);
}
