//#include "graph.cpp"
#include <iostream>
#include <random>
#include <algorithm>


//input: c   = number of vertices in the clique,
//		 ind = number of vertices in the independent set
Splitgraph getSplitgraph(int c, int ind) {
	Graph g;
	g.grow(c + ind);

	std::random_device rseed;
	std::mt19937 rgen(rseed());
	std::uniform_int_distribution<> idist(1,c-1);
	std::uniform_int_distribution<> idist2(0,c);

	for (int i=0; i<c-1; ++i) {
		Vertex & v = g.at(i);
		for (int j=i+1; j<c; ++j) {
			v.addlink(j);
			Vertex & w = g.at(j);
			w.addlink(i);
		}
	}

	for (int i=c; i<g.size(); ++i) {
		Vertex & v = g.at(i);
		int r = idist(rgen);
		int j = 0;
		while(j<r){
			int dest = idist2(rgen);
			const std::vector<int> & adj = v.getAdjlist();
			if(std::find(adj.begin(), adj.end(), dest) != adj.end()) {
				continue;
			}
			else{
				v.addlink(dest);
				++j;
			}
		}

	}

	/*
	for (int i=0; i<g.size(); ++i) {
		Vertex & v = g.at(i);
		int n = v.numlinks();
		std::cout << "Vertex " << i << ": " << n << std::endl;
	}
	*/
	std::for_each(g.begin(), g.end(), [](Vertex & v){v.sortadjlist();});
	Splitgraph s(g, c);
	return s;
}

/*
int main() {
	Graph g = getSplitgraph(10,2);
	//std:: cout << RandU(0, 99) << std::endl;
	return 0;
}
*/
