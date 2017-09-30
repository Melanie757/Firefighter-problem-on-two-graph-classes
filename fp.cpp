#include "graph.hpp"
#include "read_graph.cpp"
#include "onCographs.cpp"
//#include "onSplitgraphs.cpp"
#include "getSplitgraph.cpp"
#include "onSplitgraphs_new.cpp"
#include <iostream>
#include <chrono>

void printUsage(const char * prg){
	std::cout << "Usage:" << std::endl
	          << prg << " splitgraph <burning_idx> <graph_file> <delimiter>"     << std::endl
	          << prg << " splitgraph <burning_idx> <#clique> <#independent set>" << std::endl
		      << prg << " cograph    <burning_idx> <graph_file1> <graph_file2>"  << std::endl;
}

int main(int argc, char** argv) {
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	Graph g_file;
	std::string program, file;
	int delimiter = 0;

	if(argc != 5) {
		std::cout << "False number of arguments." << std::endl;
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	program = argv[1];

	if(program == "splitgraph") {

		int s = std::atoi(argv[2]);
	
		file = argv[3];

		std::string name(argv[3]);
		int len = name.length();
		std::string t = name.substr(len-2, len-1);

		if(t == "ml") {
			delimiter = std::atoi(argv[4]);
	
			std::cout << "Processing " << file 
        	  	  	<< ", " << delimiter << std::endl;

			g_file = read_graph(file);
		} 

		else {
			int c   = std::atoi(argv[3]);
			int ind = std::atoi(argv[4]);
			if(c == 0 || ind == 0){
				std::cerr << "Input is not a number or .graphml file."
				          << std::endl;
				printUsage(argv[0]);
				return EXIT_FAILURE;
			}
			delimiter = c;
			g_file = getSplitgraph(c, ind);
			//std::cout << "Graph generated." << sdt::endl;
		}

		Splitgraph sol(g_file, delimiter);
		start = std::chrono::high_resolution_clock::now();
		vec solution = onSplitgraphs(sol,s);
		end   = std::chrono::high_resolution_clock::now();
		auto elapsed_time = end - start;
		auto time_in_us = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
		
		std::cout << "Burning Vertex:" << s << std::endl;
		for (int i=0; i<solution.size(); ++i) {
			std::cout << solution.at(i) << " ";
		}
		std::cout << std::endl;
		std::cout << "time: " << time_in_us.count() << "ns" << std::endl;	
	}


	else if(program == "cograph") {
		Graph g1_file, g2_file;
		std::string filename_g1("Cograph1_1.graphml");
		std::string filename_g2("Cograph1_2.graphml");

		int s = std::atoi(argv[2]);

		filename_g1  = argv[3];
		filename_g2  = argv[4];

		if(filename_g1.size() == 0
                   || filename_g2.size() == 0){
			printUsage(argv[0]);
			return EXIT_FAILURE;
		}
		
		std::cout << "Processing (" << filename_g1 << ","
	          	<< filename_g2 << ")" << std::endl;
	
		g1_file = read_graph(filename_g1);
		g2_file = read_graph(filename_g2);

		//const int size_total = g1_file.size() + g2_file.size();
		//start = std::chrono::system_clock::now();
		start = std::chrono::high_resolution_clock::now();
		vec solution = onCographs(g1_file, g2_file, s);
		end   = std::chrono::high_resolution_clock::now();
		auto elapsed_time = end - start;
		auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
			
		std::cout << "Burning Vertex: " << s << std::endl;
		for (int i=0; i<solution.size(); ++i) {
			std::cout << solution[i] << " ";
		}
		std::cout << std::endl;
		std::cout << "time: " << time_in_us.count() << "us" << std::endl;
	}

	return EXIT_SUCCESS;
}
