SHELL := /bin/bash

.PHONY: all test clean

all: test-alg

%: %.cpp
	${DOCKER_CMD} ${CXX} -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../include -o $@ $^ -lz

test: test-alg
	${DOCKER_CMD} ./test-alg

clean:
	rm -rf test-alg
