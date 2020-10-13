#ifndef GRAPH_INCLUDED
#define GRAPH_INCLUDED

#include <vector>
#include <string>
#include <unordered_map>

class Vertex {

private:
	int state_=0;
	std::vector<int> adjlist_;
public:
	Vertex() = default;
	Vertex(const int & state, const std::vector<int> & adjlist);
	int getState() const;

	std::vector<int> & getAdjlist();
	const std::vector<int> & getAdjlist() const;

	void protect();
  void burn();
	void saved();
	const int numlinks() const;

	void addlink(const int & dest);
  void sortadjlist();
	bool removelink(const int & dest);
	void sizeAdjlist(int nelem);
};

//graph as adjacency list
class Graph {

protected: 
	using storage = std::vector<Vertex>;
	storage graph_;
	std::unordered_map<std::string, int> lookup_;

public:
	Graph() = default;
	Graph(storage && graph);
  Graph(storage graph, std::unordered_map<std::string, int> lookup);

	storage & getGraph();
  Vertex & at(const int & i);

	int size() const;
	storage::iterator begin();
  storage::const_iterator begin() const;

	storage::iterator end();
  storage::const_iterator end() const;
	storage::reference back();
  storage::const_reference back() const;
	void grow(int nelem = 1);
	void growAdjlists(int nelem);
	void print() const;
};

enum class GRAPH_TYPE {
	JOIN,
	UNION
};

class Cograph : public Graph {
private:
	int delimiter = 0;
public:
	Cograph(const Graph & one, const Graph & two, GRAPH_TYPE type = GRAPH_TYPE::JOIN);
	int getDelimiter() const;
};

class Splitgraph : public Graph {
private:
	int delimiter_ = 0; //indicates which vertices are a clique and which are independent
						//shows where the first vertex of the independent set is
public:
	Splitgraph()             = default;
	Splitgraph(Splitgraph &) = default;
	Splitgraph(const Graph & g, int delimiter);

	Splitgraph & operator=(Splitgraph &)  = default;
	Splitgraph & operator=(Splitgraph &&) = default;

	int getDelimiter() const;
};
#endif // GRAPH_INCLUDED

