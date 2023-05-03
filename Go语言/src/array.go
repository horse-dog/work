package main

import "fmt"

func main() {
	var arr0 [10]int
	arr1 := [10]int{1, 2, 3, 4}
	arr2 := []int{1, 2, 3, 4}

	for i := 0; i < len(arr0); i++ {
		fmt.Println(arr0[i])
	}

	for idx, val := range arr1 {
		fmt.Println(idx, val)
	}

	for idx, val := range arr2 {
		fmt.Println(idx, val)
	}

	fmt.Printf("%T\n", arr0)
	fmt.Printf("%T\n", arr1)
	fmt.Printf("%T\n", arr2)

}
