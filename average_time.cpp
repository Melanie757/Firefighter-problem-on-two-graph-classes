#include "graph.cpp"
#include "read_graph.cpp"
#include "onCographs.cpp"
#include "onSplitgraphs.cpp"
#include "getSplitgraph.cpp"
#include <iostream>
#include <iomanip>
#include <chrono>

int main(int argc, char** argv) {
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	Splitgraph g_file;
	Graph g;
	std::string program, file;
	int delimiter = 0;
	program = argv[1];
	int count = std::atoi(argv[2]);
	int s = std::atoi(argv[3]);

	if(program == "splitgraph") {
		file = argv[4];

		std::string name(argv[4]);
		int len = name.length();
		std::string t = name.substr(len-2, len-1);

		if(t == "ml") {
			delimiter = std::atoi(argv[5]);
	
			std::cout << "Processing " << file 
        	  	  	<< ", " << delimiter << std::endl;

			g = read_graph(file);
			g_file = Splitgraph(g, delimiter);
		} 

		else {
			int c   = std::atoi(argv[4]);
			int ind = std::atoi(argv[5]);
			if(c == 0 || ind == 0){
				std::cerr << "Input is not a number or .graphml file."
				          << std::endl;
				return EXIT_FAILURE;
			}
			//delimiter = c;
			//backup graph
			g_file = getSplitgraph(c, ind);
		}
		
		std::vector<int> av;
		
		auto sum = std::chrono::nanoseconds::zero();
		for(int j=0; j<count; ++j) {
			Splitgraph sol(g_file);
			start        = std::chrono::high_resolution_clock::now();
			vec solution = onSplitgraphs(sol,s);
			end          = std::chrono::high_resolution_clock::now();
			auto elapsed_time = end - start;
			sum += elapsed_time;
		}
		auto average = sum / count;
		std::cout << "time: " << average.count() << "ns" << std::endl;
	}

	else if(program == "cograph") {
		Graph g1_file, g2_file;
		std::string filename_g1("Cograph1_1.graphml");
		std::string filename_g2("Cograph1_2.graphml");

		filename_g1  = argv[4];
		filename_g2  = argv[5];
	
		g1_file = read_graph(filename_g1);
		g2_file = read_graph(filename_g2);


		auto sum = std::chrono::nanoseconds::zero();
		for(int j=0; j<count; ++j) {
			start = std::chrono::high_resolution_clock::now();
			vec solution = onCographs(g1_file, g2_file, s);
			end   = std::chrono::high_resolution_clock::now();
			auto elapsed_time = end - start;
			sum += elapsed_time;
		}
		auto average = sum / count;
		auto avg_in_us = std::chrono::duration_cast<std::chrono::microseconds>(average);
		std::cout << "time: " << avg_in_us.count() << "us" << std::endl;

	}

	return 0;
}