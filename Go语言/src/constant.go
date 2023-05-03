package main

import "fmt"

/**
 * 1. const 关键字的使用与 var 关键字的使用语法是类似的
 * 2. iota 关键字只能配合 const 关键字使用
 */

const (
	UP    = iota // 0
	DOWN         // 1
	LEFT         // 2
	RIGHT        // 3
)

const (
	a, b = iota + 1, iota + 2 // 1, 2
	c, d                      // 3, 4
	e, f                      // 5, 6
	g, h = iota * 2, iota * 3 // 6, 9
	i, j                      // 8, 12
)

func main() {
	const length int = 20
	fmt.Println("hello world")
}
