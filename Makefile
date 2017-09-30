CXX=clang++
FLAGS=-std=c++11 -Wall -Wpedantic 
FILES=fp.cpp graph.cpp onSplitgraphs.cpp onCographs.cpp getSplitgraph.cpp part_solution.cpp read_graph.cpp
HEADERS=graph.hpp onSplitgraphs.hpp onCographs.hpp getSplitgraph.hpp part_solution.hpp read_graph.hpp
OUTPUT=program.a

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
