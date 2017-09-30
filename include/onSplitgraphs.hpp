#ifndef ON_SPLITGRAPHS_INCLUDED
#define ON_SPLITGRAPHS_INCLUDED

#include "graph.hpp"

#include <vector>

using vec = std::vector<int>;

/**
 * takes a splitgraph and position of the burning vertex in graph_
 */
vec onSplitgraphs(Splitgraph & g, int pos);

#endif // ON_SPLITGRAPHS_INCLUDED

