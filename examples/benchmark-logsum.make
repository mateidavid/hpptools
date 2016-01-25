SHELL := /bin/bash

.PHONY: all sample clean

all: benchmark-logsum

benchmark-logsum: ../include/logsum.hpp
	g++ -std=c++11 -O2 -Wall -Wextra -pedantic -DBENCHMARK_p7LOGSUM -x c++ ../include/logsum.hpp -o $@

run: benchmark-logsum
	./benchmark-logsum 42 0
	./benchmark-logsum 42 1

clean:
	rm -rf benchmark-logsum
