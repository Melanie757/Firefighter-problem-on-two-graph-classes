#include "graph.hpp"
#include "read_graph.hpp"
#include "onCographs.hpp"
#include "onSplitgraphs.hpp"
#include "getSplitgraph.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	Graph g_file;
	std::string program, file;
	int delimiter = 0;

	if(argc != 4) {
		std::cout << "Falsche Anzahl an Argumenten." << std::endl;
	}

	program = argv[1];

	if(program == "Splitgraph") {
	
		file = argv[2];

		std::string s(argv[2]);
		int len = s.length();
		std::string t = s.substr(len-2, len-1);

		if(t == "ml") {
			delimiter = std::atoi(argv[3]);
	
			std::cout << "Processing " << file 
        	  	  	<< ", " << delimiter << std::endl;

			g_file = read_graph(file);
		} 

		else {
			int c   = std::atoi(argv[2]);
			int ind = std::atoi(argv[3]);
			if(c == 0 || ind == 0){
				std::cerr << "Eingabe ist keine Zahl oder .graphml Datei"
				          << std::endl;
				return EXIT_FAILURE;
			}
			delimiter = c;
			g_file = getSplitgraph(c, ind);
		}

		for (int i=0; i<g_file.size(); ++i) {
			
			Splitgraph sol(g_file, delimiter);

			start = std::chrono::system_clock::now();
			auto solution = onSplitgraphs(sol,i);
			end   = std::chrono::system_clock::now();
			auto elapsed_time = end - start;
			auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
			
			std::cout << "Burning Vertex:" << i << std::endl;
			for (int i=0; i<solution.size(); ++i) {
				std::cout << solution.at(i) << " ";
			}
			std::cout << std::endl;
			std::cout << "time: " << time_in_us.count() << "us" << std::endl;	
		}

	}

	else if(program == "Cograph") {
		Graph g1_file, g2_file;
		std::string filename_g1("Cograph1_1.graphml");
		std::string filename_g2("Cograph1_2.graphml");

		filename_g1  = argv[2];
		filename_g2  = argv[3];
		
		std::cout << "Processing (" << filename_g1 << ","
	          	<< filename_g2 << ")" << std::endl;
	
		g1_file = read_graph(filename_g1);
		g2_file = read_graph(filename_g2);

		const int size_total = g1_file.size() + g2_file.size();
		//start = std::chrono::system_clock::now();
		for (int i=0; i<size_total; ++i) {
			start = std::chrono::system_clock::now();
			auto solution = onCographs(g1_file, g2_file, i);
			end   = std::chrono::system_clock::now();
			auto elapsed_time = end - start;
			auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
			
			std::cout << "Burning Vertex: " << i << std::endl;
			for (int i=0; i<solution.size(); ++i) {
				std::cout << solution[i] << " ";
			}
			std::cout << std::endl;
			std::cout << "time: " << time_in_us.count() << "us" << std::endl;
		}
	}
	return EXIT_SUCCESS;
}
