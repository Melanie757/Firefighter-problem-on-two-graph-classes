#include "graph.cpp"
#include "read_graph.cpp"


int main() {
	Graph g_1 = read_graph("Graphen/Cograph3_1.graphml");
	Graph g_2 = read_graph("Graphen/Cograph3_2.graphml");

	CoGraph g(g_1, g_2);

	for(int i=0; i<g.size(); ++i) {
		Vertex v = g.at(i);
		std::vector<int> adj = v.getAdjlist();
		std::cout << "Vertex " << i << ": ";
		for (int j=0; j<adj.size(); ++j) {
			std::cout << adj.at(j) << " ";
		}
		std::cout << std::endl;
	}
	return 0;
}