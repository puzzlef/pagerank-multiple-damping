Comparing performance of single vs block multiple damping factor based ranks for PageRank ([pull], [CSR]).

The **damping factor** parameter is the [PageRank algorithm] controls the tendency
of a user to follow one of the links on a given web page. A damping factor (α) of
`1` implies that the user (or a web surfer) will always follow one of the given
links on the given webpage (like a crawler bot). In contrast, a damping factor of
`0` implies that the user will always jump to a random webpage. A common choice for
damping factor is `0.85` which amounts to an average path (link follow) length of
`6.67`. This choice can make certain webpages seem too important, and leave the
remainder of the webpages with negligible importance. A prudent approach could be
to allow the average path length to vary from `2` to `10`, which amounts to the
damping factor parameter ranging from `0.50` to `0.90`.

*When* ranking vertices with **multiple different damping factors** is desireable,
the PageRank computation upon the graph can be peformed one after the other with
each desired damping factor (**single**), or with multiple damping factors in one
go (**block**). With the *block*-based approach, given `D` different damping factors,
each vertex in the graph is mapped to a list of `D` ranks. This is implemented as
a single-dimensional vector with a size of `N*D`, where `N` is the total number of
vertices in the graph. The idea behind such a *block*-based arrangement of ranks is
to allow for better usage of memory bandwidth by combining multiple accesses to the
graph data structure (in case of *single*-based approach) to one, and by increasing
the amount of sequential memory accesses into the rank vector (due to blocks of `D`
ranks per vertex).

Results indicate that there is significant overhead when running the *block*-based
approach with a *block size* of `1` (one damping factor only). This could be due to
the additional logic which for some reason prevents the compiler from being able to
optimize the code. However, with a *block size* of `32` (32 damping factors in
descending order), the *block*-based approach achieves an average speedup of `1.67x`
([GM-RATIO]) / `1.70x` ([AM-RATIO]) with respect to *single*-based approach.

All outputs are saved in [out](out/) and a small part of the output is listed
here. Some [charts] are also included below, generated from [sheets]. The input
data used for this experiment is available at the [SuiteSparse Matrix Collection].

<br>

```bash
$ g++ -std=c++17 -O3 main.cxx
$ ./a.out ~/data/web-Stanford.mtx
$ ./a.out ~/data/web-BerkStan.mtx
$ ...

# Loading graph /home/subhajit/data/web-Stanford.mtx ...
# order: 281903 size: 2312497 [directed] {}
# order: 281903 size: 2312497 [directed] {} (transposeWithDegree)
# [#32; 08834.382 ms; 1403 iters.] [0.0000e+00 err.] pagerankSeqSingle
# [#32; 06651.837 ms; 1403 iters.] [1.2975e-05 err.] pagerankSeqBlock
#
# Loading graph /home/subhajit/data/web-BerkStan.mtx ...
# order: 685230 size: 7600595 [directed] {}
# order: 685230 size: 7600595 [directed] {} (transposeWithDegree)
# [#32; 19374.484 ms; 1431 iters.] [0.0000e+00 err.] pagerankSeqSingle
# [#32; 10376.712 ms; 1431 iters.] [1.4077e-05 err.] pagerankSeqBlock
#
# ...
```

[![](https://i.imgur.com/IbT7PGu.png)][sheetp]
[![](https://i.imgur.com/W51wCwO.png)][sheetp]

<br>
<br>


## References

- [PageRank Algorithm, Mining massive Datasets (CS246), Stanford University][PageRank algorithm]
- [SuiteSparse Matrix Collection]

[PageRank algorithm]: https://www.youtube.com/watch?v=ke9g8hB0MEo
[SuiteSparse Matrix Collection]: https://sparse.tamu.edu
[pull]: https://github.com/puzzlef/pagerank-push-vs-pull
[CSR]: https://github.com/puzzlef/pagerank-class-vs-csr
[GM-RATIO]: https://gist.github.com/wolfram77/6dc740392d2f4e713fafdaea4ec1eba2
[AM-RATIO]: https://gist.github.com/wolfram77/6dc740392d2f4e713fafdaea4ec1eba2
[charts]: https://photos.app.goo.gl/eRnMmdqxtiiDbchFA
[sheets]: https://docs.google.com/spreadsheets/d/1cQr6wLqKFU_F3H8uMuVQk_DzNvl5ogCy-3rJfQdGF3c/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vRf6EsCE_dyU3yQAOcAYf2UOOOQwnYvpXnvOIiMprfULcWNM-52uVhr1qgSYbW0-sV_LKofP75DcbR2/pubhtml
