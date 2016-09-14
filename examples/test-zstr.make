SHELL := /bin/bash

.PHONY: all test clean

all: test-strict_fstream ztxtpipe zpipe zc

%: %.cpp
	${DOCKER_CMD} ${CXX} -std=c++11 -O0 -g3 -ggdb -fno-eliminate-unused-debug-types -Wall -Wextra -pedantic -I../include -o $@ $^ -lz

test: ztxtpipe zpipe zc
	cat ztxtpipe.cpp | ${DOCKER_CMD} ./ztxtpipe | diff -q - ztxtpipe.cpp
	cat ztxtpipe.cpp | gzip | ${DOCKER_CMD} ./ztxtpipe | diff -q - ztxtpipe.cpp
	cat ztxtpipe.cpp ztxtpipe.cpp | gzip | ${DOCKER_CMD} ./ztxtpipe | diff -q - <(cat ztxtpipe.cpp ztxtpipe.cpp)
	{ gzip <ztxtpipe.cpp; gzip <ztxtpipe.cpp; } | ${DOCKER_CMD} ./ztxtpipe | diff -q - <(cat ztxtpipe.cpp ztxtpipe.cpp)

	cat zpipe.cpp | ${DOCKER_CMD} ./zpipe | diff -q - zpipe.cpp
	cat zpipe.cpp | gzip | ${DOCKER_CMD} ./zpipe | diff -q - zpipe.cpp
	cat zpipe.cpp zpipe.cpp | gzip | ${DOCKER_CMD} ./zpipe | diff -q - <(cat zpipe.cpp zpipe.cpp)
	{ gzip <zpipe.cpp; gzip <zpipe.cpp; } | ${DOCKER_CMD} ./zpipe | diff -q - <(cat zpipe.cpp zpipe.cpp)
	cat zpipe.cpp | gzip | gzip | ${DOCKER_CMD} ./zpipe | diff -q - <(cat zpipe.cpp | gzip)

	cat zc.cpp | ${DOCKER_CMD} ./zc | diff -q - zc.cpp
	cat zc.cpp | ${DOCKER_CMD} ./zc - | diff -q - zc.cpp
	cat zc.cpp | ${DOCKER_CMD} ./zc - - | diff -q - zc.cpp
	${DOCKER_CMD} ./zc zc.cpp | diff -q - zc.cpp
	cat zc.cpp | gzip | ${DOCKER_CMD} ./zc | diff -q - zc.cpp
	cat zc.cpp | gzip | ${DOCKER_CMD} ./zc - | diff -q - zc.cpp
	cat zc.cpp | gzip | ${DOCKER_CMD} ./zc - - | diff -q - zc.cpp

	cat zc.cpp | ${DOCKER_CMD} ./zc -c | zcat | diff -q - zc.cpp
	cat zc.cpp | ${DOCKER_CMD} ./zc -c - | zcat | diff -q - zc.cpp
	cat zc.cpp | ${DOCKER_CMD} ./zc -c - - | zcat | diff -q - zc.cpp
	${DOCKER_CMD} ./zc -c zc.cpp | zcat | diff -q - zc.cpp

	cat zc.cpp | ${DOCKER_CMD} ./zc -c zc.cpp - | zcat | diff -q - <(cat zc.cpp zc.cpp)
	cat zc.cpp | ${DOCKER_CMD} ./zc -c - zc.cpp | zcat | diff -q - <(cat zc.cpp zc.cpp)
	${DOCKER_CMD} ./zc -c zc.cpp zc.cpp | zcat | diff -q - <(cat zc.cpp zc.cpp)
	{ ${DOCKER_CMD} ./zc -c zc.cpp; ${DOCKER_CMD} ./zc -c zc.cpp; } | zcat | diff -q - <(cat zc.cpp zc.cpp)
	{ ${DOCKER_CMD} ./zc -c zc.cpp; gzip <zc.cpp; } | zcat | diff -q - <(cat zc.cpp zc.cpp)
	{ gzip <zc.cpp; ${DOCKER_CMD} ./zc -c zc.cpp; } | zcat | diff -q - <(cat zc.cpp zc.cpp)
	@echo "all passed"

clean:
	rm -rf test-strict_fstream ztxtpipe zpipe zc
