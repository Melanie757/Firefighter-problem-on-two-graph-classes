#include <vector>

using vec = std::vector<int>;

//Sonderfälle!

//input: two cographs g1, g2 which g is the complete join of, 
//burning vertex s from g as position of s in graph_
vec onCographs(const Graph & g1, const Graph & g2, const int & pos){
	int saved = 0; //number of saved vertices
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
		//find one neighbour of s which was from g2 and protect it
		for (int i=0; i<adj_s.size(); ++i) {
			const int n = adj_s.at(i);
			if (n >= delimiter) {
				solution.push_back(n);
				Vertex & v = g.at(n);
				v.protect();
				saved += 1;
				break;
			}
		}
	}
	//second case: burning vertex is in g2
	else {
		//find one neighbour of s which was from g1 and protect it
		for (int i=0; i<adj_s.size(); ++i) {
			const int n = adj_s.at(i);
			if(n < delimiter) {
				solution.push_back(n);
				Vertex & v = g.at(n);
				v.protect();
				saved += 1;
				break;
			}
		}
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
			saved += 1;
			break;
		}
	}


	/*
	//burn vertices in the next time step
	for (int i=0; i<g.size(); ++i) {
		Vertex & v = g.at(i);
		if (v.getState() == 1) {
			const vec & adj = v.getAdjlist();

		}
	}
	*/

	return solution;
}