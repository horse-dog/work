package main

import "fmt"

func add(a int, b int) int {
	return a + b
}

func sub(a int, b int) int {
	return a - b
}

func foo(a int, b int) (int, int) {
	return add(a, b), sub(a, b)
}

func bar(a int, b int) (r1 int, r2 int) {
	r1 = add(a, b)
	r2 = sub(a, b)
	return
}

func main() {
	fmt.Println(add(3, 2), sub(3, 2))
	fmt.Println(foo(3, 2))
	fmt.Println(bar(3, 2))
}
