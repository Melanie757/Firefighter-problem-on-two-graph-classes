#include "read_graph.hpp"
#include "graph.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>

Graph read_graph(const std::string & filename) {

	//check if the file is a graphml-file
	int len = filename.length();

	if (len < 7) {
		std::cerr << "Es wird eine graphml-Datei benötigt." << std::endl;
		return {};
	}

	std::string newstring = filename.substr(len-7, len-1);
	
	if (newstring != "graphml") {
		std::cerr << "Es wird eine graphml-Datei benötigt." << std::endl;
		return {};
	}

	// lookup table for mapping string key to position in graph
	std::unordered_map<std::string, int> lookup; 

	Graph graph = {};

	std::ifstream input(filename);

	if (!input) {
		std::cerr << "Fehler beim Öffnen der Datei " << filename << std::endl;
		return {};
	}

	std::string line;
	while (getline(input,line)) {
		//std::cout << line << std::endl;

		//find nodes and edges
		std::string::size_type n,m;
		n = line.find("<node");
		if (n != std::string::npos) {
			auto id_begin = line.find("id=\"",n)+4;
			auto id_end   = line.find("\"",id_begin);
			//std::cout << "ID Begin " << id_begin << " ID End " << id_end << std::endl;
			std::string result;
			result.resize(id_end-id_begin);
			std::copy(line.begin() + id_begin,
				      line.begin() + id_end,
				      result.begin());
			if(lookup.find(result) == lookup.end()){
				lookup.insert(std::make_pair(result, lookup.size()));
				graph.grow();
			}

			//std::cout << "ID: " << result << std::endl;
			continue;
		}
		m = line.find("<edge");
		if (m != std::string::npos) {
			auto source_begin = line.find("source=\"")+8;
			auto source_end   = line.find("\"",source_begin);
			auto target_begin = line.find("target=\"", source_end)+8;
			auto target_end   = line.find("\"", target_begin);

			std::string source;
			std::string target;

			source.resize(source_end - source_begin);
			target.resize(target_end - target_begin);

			std::copy(line.begin() + source_begin,
				      line.begin() + source_end,
				      source.begin());
			std::copy(line.begin() + target_begin,
				      line.begin() + target_end,
				      target.begin());

 
			if(lookup.find(source) == lookup.end()){
				lookup.insert(std::make_pair(source, lookup.size()));
				graph.grow();
			}
			if(lookup.find(target) == lookup.end()){
				lookup.insert(std::make_pair(target, lookup.size()));
				graph.grow();
			}
			// Kante einfügen
			const auto & source_id = lookup[source];
			const auto & target_id = lookup[target];

			graph.at(source_id).addlink(target_id);
			graph.at(target_id).addlink(source_id);

		}

	}
	std::for_each(graph.begin(), graph.end(), [](Vertex & v){v.sortadjlist();});

	std::vector<Vertex> g = graph.getGraph();
	Graph result(g, lookup);
	
	return result;
}

