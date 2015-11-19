SHELL := /bin/bash

.PHONY: all test clean

all: test-alg

%: %.cpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../include -o $@ $^ -lz

test: test-alg
	./test-alg

clean:
	rm -rf test-alg
