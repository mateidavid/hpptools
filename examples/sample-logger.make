SHELL := /bin/bash

.PHONY: all sample clean

all: sample-logger

sample-logger: ../include/logger.hpp
	g++ -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -D SAMPLE_LOGGER -x c++ $< -o $@

sample: sample-logger
	./sample-logger info
	./sample-logger info alt:debug1

clean:
	rm -rf sample-logger
