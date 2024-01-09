package main

import (
	"fmt"
)

type Calculator interface {
	Execute(int, int) int
}

type Add struct {
}

type Sub struct {
}

func (this *Add) Execute(x int, y int) int {
	return x + y
}

func (this *Sub) Execute(x int, y int) int {
	return x - y
}

func Calculate(c Calculator, x int, y int) int {
	return c.Execute(x, y)
}

func main() {
	a := Add{}
	b := Sub{}
	fmt.Println(Calculate(&a, 3, 2))
	fmt.Println(Calculate(&b, 3, 2))
}
