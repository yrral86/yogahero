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

for i in 5000 15000 20000 25000 50000; do
    ./gather_one.sh ANGLE$i;
done

for i in 500 1500 2000 2500 3000 3500 4000; do
    ./gather_one.sh FLOOR$i;
done

for i in 10 15 20 25; do
    for j in 15 20 25 30 35; do
	./gather_one.sh ALL$i$j;
    done;
done

./gather_one.sh ALL205
./gather_one.sh ALL205S
./gather_one.sh ALL1025S
