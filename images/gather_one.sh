#!/bin/bash

echo "Gathering $1"

files=`find . | egrep '*.data'| grep $1`

echo -e \
    "file\tinitial error\talign error\tmatch error\talign \
time\tmatch time\ttotal time" \
    > $1.data

for i in $files; do
    # Gather data
    echo -e -n "$i" | perl -p -i -e 's#\./output/(.*)/.*#\1\t#' \
	| cat - $i >> $1.data;
    echo >> $1.data;
    # Reorder filenames so pose comes first
    perl -p -i -e 's/^([a-z]*)-([a-z\-0-9]*)/$2-$1/' $1.data
done
