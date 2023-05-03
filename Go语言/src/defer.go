package main

import "fmt"

// defer 关键字类似于 unix 的 atexit 库函数，并且作用在函数调用

func defer_callback() {
	fmt.Println("defer...")
}

func return_callback() int {
	fmt.Println("return...")
	return 0
}

func foo() int {
	defer defer_callback()
	return return_callback()
}

func main() {
	foo()
}
