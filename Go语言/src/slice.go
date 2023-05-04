package main

import "fmt"

func disp(slice []int) {
	if slice == nil {
		fmt.Println("empty slice")
		return
	}
	// equal with: fmt.Println(slice)
	fmt.Printf("%v\n", slice)
}

func main() {
	slice1 := []int{1, 2, 3, 4} // {1, 2, 3, 4}
	slice2 := make([]int, 4)    // {0, 0, 0, 0}
	var slice3 []int            // slice3 == [] == nil
	slice4 := [][]int{slice1, slice2, slice3}
	copy(slice2, slice1) // 深拷贝
	for _, slice := range slice4 {
		disp(slice)
	}
}
