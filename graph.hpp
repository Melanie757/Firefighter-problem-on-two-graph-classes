#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

class Vertex {

private:
	int state_=0;
	std::vector<int> adjlist_;
public:
	Vertex() = default;
	Vertex(const int & state, const std::vector<int> & adjlist):
	state_(state), adjlist_(adjlist) {}

	int getState() {return state_;}

	std::vector<int> & getAdjlist() {return adjlist_;}
	const std::vector<int> & getAdjlist() const {return adjlist_;}

	void protect() {
		if (state_ != 1) {
			state_ = -1;
		}
	}
	void burn() {
		if (state_ == 0) {
			state_ = 1;
		}
	}
	void saved() {
		if (state_ == 0) {
			state_ = -2;
		}
	}

	size_t numlinks() const {
		return adjlist_.size();
	}

	void addlink(const int & dest){
		adjlist_.emplace_back(dest);
	}
	void sortadjlist() {
		std::sort(adjlist_.begin(), adjlist_.end());
	}

	bool removelink(const int & dest){
		auto pos = std::find(adjlist_.begin(), adjlist_.end(), dest);
		if(pos == adjlist_.end()){
			return false;
		} else {
			adjlist_.erase(pos);
			return true;
		}
	}

	void sizeAdjlist(int nelem) {
		adjlist_.resize(nelem,0);
	}
};

//graph as adjacency list
class Graph {

protected: 
	using storage = std::vector<Vertex>;
	storage graph_;
	std::unordered_map<std::string, int> lookup_;

public:
	Graph() = default;
	Graph(storage && graph):
	graph_(graph) {}
	Graph(storage graph, std::unordered_map<std::string, int> lookup):
	graph_(graph), lookup_(lookup) {}

	storage & getGraph() {return graph_;}
	Vertex & at(const int & i) {return graph_.at(i);}
	const Vertex & at(const int & i) const {return graph_.at(i);}
	Vertex & operator[](const int & i) {return graph_[i];}
	const Vertex & operator[](const int & i) const {return graph_[i];}

	size_t size() const {
		return graph_.size();
	}

	storage::iterator begin(){
		return graph_.begin();
	}
	storage::const_iterator begin() const {
		return graph_.cbegin();
	}

	storage::iterator end(){
		return graph_.end();
	}
	storage::const_iterator end() const {
		return graph_.cend();
	}

	storage::reference back() {
		return graph_.back();
	}
	storage::const_reference back() const {
		return graph_.back();
	}

	void grow(int nelem = 1){
		return graph_.resize(graph_.size()+nelem);
	}

	void growAdjlists(int nelem) {
		for (size_t i=0; i<graph_.size(); ++i) {
			Vertex v = graph_.at(i);
			v.sizeAdjlist(nelem);
		}
	}

	void print() const {
		for(size_t i=0; i<graph_.size(); ++i){
			std::cout << "Node " << i << " Nachbarn: " << std::endl;
			Vertex s(graph_[i]);
			std::vector<int> n = s.getAdjlist();
			for (size_t j=0; j<graph_[i].numlinks(); ++j) {
				std::cout << n[j] << " ";
			}
			std::cout << std::endl;
		}
	}
};

enum class GRAPH_TYPE {
	JOIN,
	UNION
};

class Cograph : public Graph {
private:
	int delimiter = 0;
public:
	Cograph(const Graph & one, const Graph & two, GRAPH_TYPE type = GRAPH_TYPE::JOIN):
		delimiter(one.size()) //position of the first vertex of graph two
	{
		graph_.reserve(delimiter + two.size());
		graph_.insert(graph_.end(), one.begin(), one.end());
		graph_.insert(graph_.end(), two.begin(), two.end());

		int c = one.size();
		for(size_t i=delimiter; i<graph_.size(); ++i) {
			Vertex & v = graph_.at(i);
			std::vector<int> & adj = v.getAdjlist();
			for(size_t j=0; j<adj.size(); ++j) {
				adj.at(j) += c;
			}
		}

		if(type == GRAPH_TYPE::JOIN){
			// join first part
			std::for_each(graph_.begin(), graph_.begin() + delimiter, [&](Vertex & v){
				for(size_t i=delimiter; i<graph_.size(); ++i){
					v.addlink(i);
				}
			});
			// join last part
			std::for_each(graph_.begin() + delimiter, graph_.end(), [&](Vertex & v){
				for(int i=0; i<delimiter; ++i){
					v.addlink(i);
				}
			});
			std::for_each(graph_.begin(), graph_.end(), [](Vertex & v){v.sortadjlist();});
		}
		
		
		else if(type == GRAPH_TYPE::UNION) {
			delimiter = 0;
		}
		
	}
	int getDelimiter() const {
		return delimiter;
	}
};

class Splitgraph : public Graph {
private:
	int delimiter_ = 0; //indicates which vertices are a clique and which are independent
						//shows where the first vertex of the independent set is
public:
	Splitgraph()             = default;
	Splitgraph(Splitgraph &) = default;
	Splitgraph(const Graph & g, int delimiter):
	Graph(g),
	delimiter_(delimiter) {}

	Splitgraph & operator=(Splitgraph &)  = default;
	Splitgraph & operator=(Splitgraph &&) = default;

	int getDelimiter() const {
		return delimiter_;
	}
};

#endif

