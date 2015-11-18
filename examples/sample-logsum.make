SHELL := /bin/bash

.PHONY: all sample clean

all: sample-logsum

sample-logsum: ../include/logsum.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -DSAMPLE_p7LOGSUM -x c++ $< -o $@

sample: sample-logsum
	./sample-logsum .5 .2

clean:
	rm -rf sample-logsum
