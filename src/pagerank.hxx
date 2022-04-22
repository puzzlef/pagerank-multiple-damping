#pragma once
#include <vector>
#include <utility>
#include "_main.hxx"

using std::vector;
using std::move;




// PAGERANK-OPTIONS
// ----------------

template <class T>
struct PagerankOptions {
  int  repeat;
  int  toleranceNorm;
  T    damping;
  T    tolerance;
  int  maxIterations;

  PagerankOptions(int repeat=1, int toleranceNorm=1, T damping=0.85, T tolerance=1e-6, int maxIterations=500) :
  repeat(repeat), toleranceNorm(toleranceNorm), damping(damping), tolerance(tolerance), maxIterations(maxIterations) {}
};


template <class T>
struct PagerankMultiOptions {
  int  repeat;
  int  toleranceNorm;
  vector<T> damping;
  T    tolerance;
  int  maxIterations;

  PagerankMultiOptions(int repeat=1, int toleranceNorm=1, const vector<T>& damping={0.85}, T tolerance=1e-6, int maxIterations=500) :
  repeat(repeat), toleranceNorm(toleranceNorm), damping(damping), tolerance(tolerance), maxIterations(maxIterations) {}
};




// PAGERANK-RESULT
// ---------------

template <class T>
struct PagerankResult {
  vector<T> ranks;
  int   iterations;
  float time;

  PagerankResult(vector<T>&& ranks, int iterations=0, float time=0) :
  ranks(ranks), iterations(iterations), time(time) {}

  PagerankResult(vector<T>& ranks, int iterations=0, float time=0) :
  ranks(move(ranks)), iterations(iterations), time(time) {}


  // Get initial ranks (when no vertices affected for dynamic pagerank).
  template <class G>
  static PagerankResult<T> initial(const G& x, const vector<T>* q=nullptr) {
    int  N = x.order();
    auto a = q? *q : createContainer(x, T());
    if (!q) fillValueAtU(a, x.vertexKeys(), T(1)/N);
    return {a, 0, 0};
  }
};


template <class T>
struct PagerankMultiResult {
  vector2d<T> ranks;
  int   iterations;
  float time;

  PagerankMultiResult(vector2d<T>&& ranks, int iterations=0, float time=0) :
  ranks(ranks), iterations(iterations), time(time) {}

  PagerankMultiResult(vector2d<T>& ranks, int iterations=0, float time=0) :
  ranks(move(ranks)), iterations(iterations), time(time) {}


  // Get initial ranks (when no vertices affected for dynamic pagerank).
  template <class G>
  static PagerankMultiResult<T> initial(const G& x, const vector2d<T>* qs=nullptr) {
    int  N = x.order();
    if (qs) return {*qs, 0, 0};
    vector2d<T> as;
    for (const auto& q : qs) {
      auto a = createContainer(x, T());
      fillValueAtU(a, x.vertexKeys(), T(1)/N);
      as.push_back(move(a));
    }
    return {as, 0, 0};
  }
};
