SHELL := /bin/bash

.PHONY: all sample clean

all: sample-pfor

sample-pfor: ../include/pfor.hpp
	g++ -std=c++11 -pthread -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -D SAMPLE_PFOR -x c++ $< -o $@

sample: sample-pfor
	./sample-pfor

clean:
	rm -rf sample-pfor
