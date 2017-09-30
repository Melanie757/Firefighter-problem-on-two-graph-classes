#ifndef ON_COGRAPHS_INCLUDED
#define ON_COGRAPHS_INCLUDED

#include "graph.hpp"
#include <vector>

using vec = std::vector<int>;

/**
 * takes two cographs g1, g2 which g is the complete join of, 
 * burning vertex s from g as position of s in graph_
 */
vec onCographs(const Graph & g1, const Graph & g2, const int & pos);

#endif // ON_COGRAPHS_INCLUDED

