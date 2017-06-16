#include "graph.cpp"
#include "read_graph.cpp"
#include "onCographs.cpp"
#include "onSplitgraphs.cpp"
#include "getSplitgraph.cpp"
#include <iostream>
#include <chrono>

int main(int argc, char** argv) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	Splitgraph g_file;
	Graph g;
	std::string program, file;
	int delimiter = 0;
	program = argv[1];
	int count = std::atoi(argv[2]);

	if(program == "Splitgraph") {
		file = argv[3];

		std::string s(argv[3]);
		int len = s.length();
		std::string t = s.substr(len-2, len-1);

		if(t == "ml") {
			delimiter = std::atoi(argv[4]);
	
			std::cout << "Processing " << file 
        	  	  	<< ", " << delimiter << std::endl;

			g = read_graph(file);
			g_file = Splitgraph(g, delimiter);
		} 

		else {
			int c   = std::atoi(argv[3]);
			int ind = std::atoi(argv[4]);
			if(c == 0 || ind == 0){
				std::cerr << "Eingabe ist keine Zahl oder .graphml Datei"
				          << std::endl;
				return EXIT_FAILURE;
			}
			//delimiter = c;
			g_file = getSplitgraph(c, ind);
		}
		// backup graph

		for(int i=0; i<g_file.size(); ++i) {
			auto sum = std::chrono::microseconds::zero();
			for(int j=0; j<count; ++j) {
				Splitgraph sol(g_file);
				start        = std::chrono::system_clock::now();
				vec solution = onSplitgraphs(sol,i);
				end          = std::chrono::system_clock::now();
				auto elapsed_time = end - start;
				sum += elapsed_time;
			}
			auto average = sum / count;
			//auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(average);
			std::cout << "Vertex " << i << ": " << average.count() << "us" << std::endl;	
		}
	}

	else if(program == "Cograph") {
		Graph g1_file, g2_file;
		std::string filename_g1("Cograph1_1.graphml");
		std::string filename_g2("Cograph1_2.graphml");

		filename_g1  = argv[3];
		filename_g2  = argv[4];
	
		g1_file = read_graph(filename_g1);
		g2_file = read_graph(filename_g2);

		const int size_total = g1_file.size() + g2_file.size();

		for(int i=0; i<size_total; ++i) {
			auto sum = std::chrono::microseconds::zero();
			for(int j=0; j<count; ++j) {
				start = std::chrono::system_clock::now();
				vec solution = onCographs(g1_file, g2_file, i);
				end   = std::chrono::system_clock::now();
				auto elapsed_time = end - start;
				sum += elapsed_time;
			}
			auto average = sum / count;
			//auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(average);
			std::cout << "Vertex " << i << ": " << average.count() << "us" << std::endl;
		}
	}

	return 0;
}