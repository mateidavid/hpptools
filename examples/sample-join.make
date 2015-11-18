SHELL := /bin/bash

.PHONY: all sample clean

all: sample-join

sample-join: ../include/join.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -D SAMPLE_JOIN -x c++ $< -o $@

sample: sample-join
	./sample-join

clean:
	rm -rf sample-join
