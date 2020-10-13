#include "graph.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

Vertex::Vertex(const int & state, const std::vector<int> & adjlist):
  state_(state), adjlist_(adjlist) {}

int Vertex::getState() const {return state_;}

std::vector<int> & Vertex::getAdjlist() {return adjlist_;}
const std::vector<int> & Vertex::getAdjlist() const {return adjlist_;}

void Vertex::protect() {
  if (state_ != 1) {
    state_ = -1;
  }
}
void Vertex::burn() {
  if (state_ == 0) {
    state_ = 1;
  }
}
void Vertex::saved() {
  if (state_ == 0) {
    state_ = -2;
  }
}

const int Vertex::numlinks() const {
  return adjlist_.size();
}

void Vertex::addlink(const int & dest){
  adjlist_.emplace_back(dest);
}
void Vertex::sortadjlist() {
  std::sort(adjlist_.begin(), adjlist_.end());
}

bool Vertex::removelink(const int & dest){
  auto pos = std::find(adjlist_.begin(), adjlist_.end(), dest);
  if(pos == adjlist_.end()){
    return false;
  } else {
    adjlist_.erase(pos);
    return true;
  }
}

void Vertex::sizeAdjlist(int nelem) {
  adjlist_.resize(nelem,0);
}

//graph as adjacency list
Graph::Graph(Graph::storage && graph):
  graph_(graph) {}
Graph::Graph(Graph::storage graph, std::unordered_map<std::string, int> lookup):
  graph_(graph), lookup_(lookup) {}

Graph::storage & Graph::getGraph() {return graph_;}
Vertex & Graph::at(const int & i) {return graph_.at(i);}

int Graph::size() const {
  return graph_.size();
}

Graph::storage::iterator Graph::begin(){
  return graph_.begin();
}
Graph::storage::const_iterator Graph::begin() const {
  return graph_.cbegin();
}

Graph::storage::iterator Graph::end(){
  return graph_.end();
}
Graph::storage::const_iterator Graph::end() const {
  return graph_.cend();
}

Graph::storage::reference Graph::back() {
  return graph_.back();
}
Graph::storage::const_reference Graph::back() const {
  return graph_.back();
}

void Graph::grow(int nelem){
  return graph_.resize(graph_.size()+nelem);
}

void Graph::growAdjlists(int nelem) {
  for (int i=0; i<graph_.size(); ++i) {
    Vertex v = graph_.at(i);
    v.sizeAdjlist(nelem);
  }
}

void Graph::print() const {
  for(int i=0; i<graph_.size(); ++i){
    std::cout << "Node " << i << " Nachbarn: " << std::endl;
    Vertex s(graph_[i]);
    std::vector<int> n = s.getAdjlist();
    for (int j=0; j<graph_[i].numlinks(); ++j) {
      std::cout << n[j] << " ";
    }
    std::cout << std::endl;
  }
}

Cograph::Cograph(const Graph & one, const Graph & two, GRAPH_TYPE type):
  delimiter(one.size()) //position of the first vertex of graph two
{
  graph_.reserve(delimiter + two.size());
  graph_.insert(graph_.end(), one.begin(), one.end());
  graph_.insert(graph_.end(), two.begin(), two.end());

  int c = one.size();
  for(int i=delimiter; i<graph_.size(); ++i) {
    Vertex & v = graph_.at(i);
    std::vector<int> & adj = v.getAdjlist();
    for(int j=0; j<adj.size(); ++j) {
      adj.at(j) += c;
    }
  }

  if(type == GRAPH_TYPE::JOIN){
    // join first part
    std::for_each(graph_.begin(), graph_.begin() + delimiter, [&](Vertex & v){
      for(int i=delimiter; i<graph_.size(); ++i){
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

int Cograph::getDelimiter() const {
  return delimiter;
}

Splitgraph::Splitgraph(const Graph & g, int delimiter):
  Graph(g),
  delimiter_(delimiter) {}

int Splitgraph::getDelimiter() const {
  return delimiter_;
}
