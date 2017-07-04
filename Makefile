CXX=g++
# profiling flags for gprof
#FLAGS=-std=c++11 -Wall -Wpedantic -pg
FLAGS=-std=c++11 -Wall -Wpedantic
FLAGS+= -fopenmp
#FILES=onSplitgraphs.cpp
#FILES=onCographs2.cpp
FILES=main.cpp read_graph.cpp onCographs.cpp onSplitgraphs.cpp getSplitgraph.cpp
#FILES=getSplitgraph.cpp
#FILES=time.cpp
#FILES=test.cpp
#FILES=test2.cpp
#FILES=fp.cpp
#FILES=average_time.cpp
OUTPUT=program.a

all: release

release: $(FILES)
	$(CXX) $(FLAGS) -O3 $(FILES) -o $(OUTPUT)

debug: $(FILES)
	$(CXX) $(FLAGS) -g $(FILES) -o $(OUTPUT)

profile: $(FILES)
	$(CXX) $(FLAGS) -O3 -pg $(FILES) -o $(OUTPUT)

run: $(FILES)
	make release; ./$(OUTPUT) 

clean:
	rm -f $(OUTPUT)
