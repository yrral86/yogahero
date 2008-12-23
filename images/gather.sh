#!/bin/bash

# from test.c
TESTS="\
    ALIGN \
    ALIGNSCALE \
    BASE \
    ALL"

SCALED_TESTS="\
    ANGLE \
    FLOOR"

rm *.data

for i in $TESTS; do
    ./gather_one.sh $i;
done

for i in $SCALED_TESTS; do
    for j in 1 10 100 1000 10000; do
	./gather_one.sh $i$j;
    done;
done