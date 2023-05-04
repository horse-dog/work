package main

import "fmt"

// universal reference
func disp(obj interface{}) {
	fmt.Println(obj)
	value, ok := obj.([]int) // type assert: is obj slice<int> type?
	if ok {
		value[0] = 123
	}
}

func main() {
	arr0 := [3]int{1, 2, 3}
	arr1 := []int{1, 2, 3}
	disp(123)
	disp("hello")
	disp(arr0)
	disp(arr1)
	fmt.Println(arr0)
	fmt.Println(arr1)
}
