// Simple performance test of Go map get

package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"time"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Fprintf(os.Stderr, "usage: perftest file\n")
		os.Exit(1)
	}

	contents, err := ioutil.ReadFile(os.Args[1])
	if err != nil {
		fmt.Fprintf(os.Stderr, "error reading file: %v\n", err)
		os.Exit(1)
	}

	counts := make(map[string]int)
	for i := 0; i < len(contents); {
		// Skip whitespace.
		for i < len(contents) && contents[i] <= ' ' {
			i++
		}
		start := i

		// Find end of word.
		for i < len(contents) && contents[i] > ' ' {
			i++
		}
		if i > start {
			word := contents[start:i]
			counts[string(word)]++
		}
	}

	// Copy keys to slice
	keys := make([]string, 0, len(counts))
	for k := range counts {
		keys = append(keys, k)
	}

	const runs = 10
	start := time.Now()
	for run := 0; run < runs; run++ {
		for i := 0; i < len(counts); i++ {
			_ = counts[keys[i]]
		}
	}
	end := time.Now()
	elapsed := end.Sub(start)
	fmt.Printf("%d runs getting %d keys: %v\n", runs, len(counts), elapsed)
}
