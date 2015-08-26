package main

import "os"
import "fmt"
import "strconv"

func main() {
	argsWOProg := os.Args[1:]

	if len(argsWOProg) == 0 {
		fmt.Println()
		fmt.Println("No input")
		fmt.Println()
		os.Exit(0)
	}

	maxLen := 0

	fmt.Println()
	for _, x := range argsWOProg {
		if len(x) > maxLen {
			maxLen = len(x)
		}
	}

	for _, x := range argsWOProg {
		val, err := strconv.ParseUint(x, 10, 64)
		fmt.Printf("%-*s => ", maxLen, x)
		if err != nil {
			fmt.Println(err)
		} else {
			fmt.Printf("%s%s", "0x", strconv.FormatUint(val, 16))
			fmt.Println()
		}
	}
	fmt.Println()
}
