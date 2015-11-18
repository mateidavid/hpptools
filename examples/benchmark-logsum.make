SHELL := /bin/bash

DIR = 

.PHONY: all sample clean

all: benchmark-logsum

benchmark-logsum: ../include/logsum.hpp ${DIR}/logsum.h ${DIR}/logsum.cpp
	g++ -std=c++11 -O2 -Wall -Wextra -pedantic -I ${DIR} -DBENCHMARK_p7LOGSUM -x c++ ../include/logsum.hpp ${DIR}/logsum.cpp -o $@

run: benchmark-logsum
	./benchmark-logsum 42 0
	./benchmark-logsum 42 1

clean:
	rm -rf benchmark-logsum
