SHELL := /bin/bash

.PHONY: all sample clean

all: sample-alg

sample-alg: ../include/alg.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -D SAMPLE_ALG -x c++ $< -o $@

sample: sample-alg
	./sample-alg

clean:
	rm -rf sample-alg
