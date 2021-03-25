#!/usr/bin/env bash

# perfget - C version
# 10 runs getting 466550 keys: 472.597ms
# 10 runs getting 466550 keys: 481.157ms
# 10 runs getting 466550 keys: 470.343ms
# 10 runs getting 466550 keys: 464.349ms
# 10 runs getting 466550 keys: 465.120ms
# 10 runs getting 466550 keys: 465.255ms
# 10 runs getting 466550 keys: 463.070ms
# MINIMUM TIME: 463.1ms (51% slower than Go)

# perfget - Go version
# 10 runs getting 466550 keys: 308.543747ms
# 10 runs getting 466550 keys: 349.240019ms
# 10 runs getting 466550 keys: 306.092176ms
# 10 runs getting 466550 keys: 308.505962ms
# 10 runs getting 466550 keys: 334.631387ms
# 10 runs getting 466550 keys: 313.506291ms
# 10 runs getting 466550 keys: 337.60812ms
# MINIMUM TIME: 306.1ms

# perfset - C version
# setting 466550 keys: 120.224000000s
# setting 466550 keys: 121.672000000s
# setting 466550 keys: 119.682000000s
# setting 466550 keys: 124.031000000s
# setting 466550 keys: 114.645000000s
# setting 466550 keys: 116.240000000s
# setting 466550 keys: 118.942000000s
# MINIMUM TIME: 114.6ms (40% faster than Go)

# perfset - Go version
# setting 466550 keys: 193.470927ms
# setting 466550 keys: 211.229354ms
# setting 466550 keys: 194.889048ms
# setting 466550 keys: 191.71137ms
# setting 466550 keys: 200.308761ms
# setting 466550 keys: 199.717919ms
# setting 466550 keys: 207.069321ms
# MINIMUM TIME: 191.7ms

# NOTE - words.txt is from here (public domain):
# https://github.com/dwyl/english-words/

set -e

echo 'perfget - C version'
gcc -Wall -O2 -o perfget-c samples/perfget.c ht.c
./perfget-c samples/words.txt
./perfget-c samples/words.txt
./perfget-c samples/words.txt
./perfget-c samples/words.txt
./perfget-c samples/words.txt
./perfget-c samples/words.txt
./perfget-c samples/words.txt

echo 'perfget - Go version'
go build -o perfget-go samples/perfget.go
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 
./perfget-go samples/words.txt 

echo

echo 'perfset - C version'
gcc -Wall -O2 -o perfset-c samples/perfset.c ht.c
./perfset-c samples/words.txt
./perfset-c samples/words.txt
./perfset-c samples/words.txt
./perfset-c samples/words.txt
./perfset-c samples/words.txt
./perfset-c samples/words.txt
./perfset-c samples/words.txt

echo 'perfset - Go version'
go build -o perfset-go samples/perfset.go
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
./perfset-go samples/words.txt 
