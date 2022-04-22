#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include "src/main.hxx"

using namespace std;




template <class G, class H>
void runPagerank(const G& x, const H& xt, int repeat) {
  using T = double;
  enum NormFunction { L0=0, L1=1, L2=2, Li=3 };
  vector<T> dampings = rangeVector(0.9, 0.6, -0.3001/32);
  vector2d<T> *inits = nullptr;
  int B = dampings.size();

  // Find pagerank using a single thread (L1-norm, tolerance=1E-6, maxIterations=500).
  auto a0 = pagerankMonolithicSeqSingle(x, xt, inits, {repeat, L1, dampings});
  T e0 = T(); for (int i=0; i<a0.ranks.size(); i++) e0 += l1Norm(a0.ranks[i], a0.ranks[i]);
  printf("[#%d; %09.3f ms; %03d iters.] [%.4e err.] pagerankSeqSingle\n", B, a0.time, a0.iterations, e0);

  // Find pagerank using a single thread (L1-norm, tolerance=1E-6, maxIterations=500).
  auto a1 = pagerankMonolithicSeqBlock(x, xt, inits, {repeat, L1, dampings});
  T e1 = T(); for (int i=0; i<a1.ranks.size(); i++) e1 += l1Norm(a1.ranks[i], a0.ranks[i]);
  printf("[#%d; %09.3f ms; %03d iters.] [%.4e err.] pagerankSeqBlock\n", B, a1.time, a1.iterations, e1);
}


int main(int argc, char **argv) {
  char *file = argv[1];
  int repeat = argc>2? stoi(argv[2]) : 5;
  printf("Loading graph %s ...\n", file);
  auto x  = readMtxOutDiGraph(file); println(x);
  auto xt = transposeWithDegree(x);  print(xt); printf(" (transposeWithDegree)\n");
  runPagerank(x, xt, repeat);
  printf("\n");
  return 0;
}
