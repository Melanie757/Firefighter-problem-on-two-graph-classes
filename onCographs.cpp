#include "onCographs.hpp"
#include "graph.hpp"
#include <vector>

using vec = std::vector<int>;

//input: two cographs g1, g2 which g is the complete join of, 
//burning vertex s from g as position of s in graph_
vec onCographs(const Graph & g1, const Graph & g2, const int & pos){
	vec solution;
	//create g as the complete join of g1 and g2
	Cograph g(g1, g2);

	const int & delimiter = g.getDelimiter();

	//burn s and get the adjacencylist
	Vertex & s = g.at(pos);
	s.burn();
	const vec & adj_s = s.getAdjlist();
	
	 //first case: burning vertex is in g1
	if (pos < delimiter) {
		//protect the first vertex of g2
		Vertex & v = g.at(delimiter);
		v.protect();
		solution.push_back(delimiter);
	}
	//second case: burning vertex is in g2
	else {
		//protect the first vertex of g1
		Vertex & v = g.at(0);
		v.protect();
		solution.push_back(0);
	}

	//burn unprotected neighbours of s
	for (int i=0; i<adj_s.size(); ++i) {
		Vertex & next = g.at(adj_s.at(i));
		if (next.getState() == 0) {
			next.burn();
		}
	}

	//find unburned vertex in g and protect it if it exists
	for (int i=0; i<g.size(); ++i) {
		Vertex & v = g.at(i);
		if (v.getState() == 0) {
			solution.push_back(i);
			v.protect();
			break;
		}
	}

	return solution;
}
