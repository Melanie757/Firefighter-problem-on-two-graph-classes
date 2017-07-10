CXX=clang++
FLAGS=-std=c++11 -stdlib=libc++ -Wall -Wpedantic 
#FILES=time.cpp
#FILES=average_time.cpp
FILES=fp.cpp
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
