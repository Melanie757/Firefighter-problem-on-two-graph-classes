CXX=clang++
FLAGS=-std=c++11 -stdlib=libc++ -Wall -Wpedantic -Werror
#FILES=onSplitgraphs.cpp
#FILES=onCographs2.cpp
#FILES=main.cpp
#FILES=getSplitgraph.cpp
FILES=time.cpp
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

run: $(FILES)
	make release; ./$(OUTPUT) 

clean:
	rm -f $(OUTPUT)
