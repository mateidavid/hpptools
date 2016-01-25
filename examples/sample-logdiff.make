SHELL := /bin/bash

.PHONY: all sample clean

all: sample-logdiff

sample-logdiff: ../include/logdiff.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -DSAMPLE_LOGDIFF -x c++ $< -o $@

sample: sample-logdiff
	./sample-logdiff .5 .2

clean:
	rm -rf sample-logdiff
