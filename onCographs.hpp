#ifndef ON_COGRAPHS_H_INCLUDED
#define ON_COGRAPHS_H_INCLUDED

#include <vector>

class Graph;

/**
 * Takes two cographs g1, g2 which g is the complete join of, 
 * burning vertex s from g as position of s in graph_
 */
std::vector<int> onCographs(
    const Graph & g1, /// first cograph
    const Graph & g2, /// second cograph
    const int & pos   /// position of burning vertex
    );

#endif

