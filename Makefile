CXX=clang++
FLAGS=-std=c++11 -Wall -Wpedantic -Iinclude 
CPPFILES=fp.cpp graph.cpp onSplitgraphs.cpp onCographs.cpp getSplitgraph.cpp part_solution.cpp read_graph.cpp
CPPHEADERS=graph.hpp onSplitgraphs.hpp onCographs.hpp getSplitgraph.hpp part_solution.hpp read_graph.hpp
OUTPUT=ffp.a

FILES=$(addprefix src/, $(CPPFILES))
HEADERS=$(addprefix include/, $(CPPHEADERS))

# os-specific flags
ifeq ($(detected_OS),Darwin)
	FLAGS+= -stdlib=libc++
endif

all: release

release: $(FILES) $(HEADERS)
	$(CXX) $(FLAGS) -O3 $(FILES) -o $(OUTPUT)

debug: $(FILES) $(HEADERS)
	$(CXX) $(FLAGS) -g $(FILES) -o $(OUTPUT)

run: $(FILES) $(HEADERS)
	make release; ./$(OUTPUT) 

clean:
	rm -f $(OUTPUT)
