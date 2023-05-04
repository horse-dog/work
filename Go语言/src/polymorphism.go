package main

import "fmt"

type Calculator interface {
	Execute(int, int) int
}

type Add struct {
	Calculator // 实现Calculator接口
}

type Sub struct {
	Calculator // 实现Calculator接口
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
