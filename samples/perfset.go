// Simple performance test of Go map set

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

	table := make(map[string]int)
	start := time.Now()
	for i := 0; i < len(counts); i++ {
		table[keys[i]] = 1
	}
	end := time.Now()
	elapsed := end.Sub(start)
	fmt.Printf("setting %d keys: %v\n", len(counts), elapsed)
}
