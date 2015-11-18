SHELL := /bin/bash

.PHONY: all test clean

all: test-algorithm_extra

%: %.cpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../include -o $@ $^ -lz

test: test-algorithm_extra
	./test-algorithm_extra

clean:
	rm -rf test-algorithm_extra
