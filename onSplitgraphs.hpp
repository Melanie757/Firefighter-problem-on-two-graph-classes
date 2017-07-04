#ifndef ON_SPLITGRAPHS_H_INCLUDED
#define ON_SPLITGRAPHS_H_INCLUDED

#include "graph.hpp"
#include <vector>

/**
 * splitgraph and position of the burning vertex in graph_
 * \return solution
 */
std::vector<int> onSplitgraphs(
    Splitgraph & g, /// splitgraph to be analyzed
    int pos         /// index of the burning vertex in the graph
);

#endif

