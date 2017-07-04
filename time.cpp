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

	if(program == "splitgraph") {
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
				std::cerr << "Input is not a number or .graphml file."
				          << std::endl;
				return EXIT_FAILURE;
			}
			delimiter = c;
			//backup graph
			g_file = getSplitgraph(c, ind);
		}
		
		std::vector<int> av;
		for(int i=0; i<g_file.size(); ++i) {
			auto sum = std::chrono::nanoseconds::zero();
			for(int j=0; j<count; ++j) {
				Splitgraph sol(g_file);
				start        = std::chrono::high_resolution_clock::now();
				vec solution = onSplitgraphs(sol,i);
				end          = std::chrono::high_resolution_clock::now();
				auto elapsed_time = end - start;
				sum += elapsed_time;
			}
			auto average = sum / count;
			av.push_back(average.count());
			//auto time_in_us = std::chrono::duration_cast<std::chrono::microseconds>(average);
			//std::cout << "Vertex " << i << ": " << std::setw(6) << average.count() << "ns" << std::endl;	
			if(i%100 == 0) {
				std::cout << i << std::endl;
			}
		}
		
		int sum2 = 0;
		for(int i=0; i<delimiter; ++i) {
			sum2 += av.at(i);
		}
		auto av2 = sum2/delimiter;
		std::cout << "Clique: " << std::setw(6) << av2 << "ns" << std::endl;
		int sum3 = 0;
		for(int i=delimiter; i<av.size(); ++i) {
			sum3 += av.at(i);
		}
		auto av3 = sum3/(av.size()-delimiter);
		std::cout << "Ind.: " << std::setw(6) << av3 << "ns" << std::endl;
	}

	else if(program == "cograph") {
		Graph g1_file, g2_file;
		std::string filename_g1("Cograph1_1.graphml");
		std::string filename_g2("Cograph1_2.graphml");

		filename_g1  = argv[3];
		filename_g2  = argv[4];
	
		g1_file = read_graph(filename_g1);
		g2_file = read_graph(filename_g2);

		const int size_total = g1_file.size() + g2_file.size();

		for(int i=0; i<size_total; ++i) {
			auto sum = std::chrono::nanoseconds::zero();
			for(int j=0; j<count; ++j) {
				start = std::chrono::high_resolution_clock::now();
				vec solution = onCographs(g1_file, g2_file, i);
				end   = std::chrono::high_resolution_clock::now();
				auto elapsed_time = end - start;
				sum += elapsed_time;
			}
			auto average = sum / count;
			auto avg_in_us = std::chrono::duration_cast<std::chrono::microseconds>(average);
			std::cout << "Vertex " << i << ": " << avg_in_us.count() << "us" << std::endl;
		}
	}

	return 0;
}