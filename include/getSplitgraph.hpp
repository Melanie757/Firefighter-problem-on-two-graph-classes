#ifndef GET_SPLITGRAPH_INCLUDED
#define GET_SPLITGRAPH_INCLUDED

#include "graph.hpp"

/**
 * generates a random splitgraph
 */
Splitgraph getSplitgraph(
    int c,   /// number of vertices in the clique
    int ind  /// number of vertices in the independent set
);

#endif // GET_SPLITGRAPH_INCLUDED

