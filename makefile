all: make

make: main.cpp Counter.cpp Timer.cpp
	g++ -std=c++14 main.cpp Counter.cpp Timer.cpp -o counter -lboost_system -lboost_filesystem -lboost_system -Og -pthread 
.PHONY: clean

clean: 
	rm counter
