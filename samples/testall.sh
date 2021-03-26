#!/usr/bin/env bash

set -e

gcc -Wall -O2 -o perfget-c samples/perfget.c ht.c
go build -o perfget-go samples/perfget.go
gcc -Wall -O2 -o perfset-c samples/perfset.c ht.c
go build -o perfset-go samples/perfset.go
gcc -O2 -Wall -o perflbh samples/perflbh.c ht.c

gcc -Wall -O2 -o lsearch samples/lsearch.c && ./lsearch >samples/output/lsearch.txt

gcc -Wall -O2 -o bsearch samples/bsearch.c && ./bsearch >samples/output/bsearch.txt

gcc -Wall -O2 -o demo samples/demo.c ht.c
echo 'foo bar the bar bar bar the' | ./demo >samples/output/demo.txt

gcc -Wall -O2 -o dump samples/dump.c ht.c && ./dump >samples/output/dump.txt

python3 samples/gensimilar.py 466550 >samples/similar.txt
gcc -O2 -Wall -o stats samples/stats.c ht.c
./stats <samples/words.txt >samples/output/stats-words.txt
./stats <samples/similar.txt >samples/output/stats-similar.txt

git diff --exit-code samples/output/*
echo 'All good!'
