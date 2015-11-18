SHELL := /bin/bash

.PHONY: all sample clean

all: sample-tpool

sample-tpool: ../include/tpool.hpp
	g++ -std=c++11 -pthread -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -D SAMPLE_TPOOL -x c++ $< -o $@

sample: sample-tpool
	./sample-tpool

clean:
	rm -rf sample-tpool
