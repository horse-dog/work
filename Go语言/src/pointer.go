package main

import "fmt"

func swap(pa *int, pb *int) {
	tmp := *pa
	*pa = *pb
	*pb = tmp
}

func main() {
	a, b := 1, 2
	swap(&a, &b)
	fmt.Println(a, b)
}
