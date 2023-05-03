package main

import (
	"fmt"
)

// global varibales
var g_A = 20

// g_B := 20 // error

func main() {

	var a int
	var b int = 10
	var c = 10
	d := 10
	var (
		e = 10
		f = "sb"
	)

	fmt.Printf("a = %2d, type = %T\n", a, a)
	fmt.Printf("b = %2d, type = %T\n", b, b)
	fmt.Printf("c = %2d, type = %T\n", c, c)
	fmt.Printf("d = %2d, type = %T\n", d, d)
	fmt.Printf("e = %2d, type = %T\n", e, e)
	fmt.Printf("f = %2s, type = %T\n", f, f)

}
