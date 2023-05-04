package main

import "fmt"

func main() {
	var map0 map[int]string
	map0 = make(map[int]string) // 必须有这行，否则段错误
	map0[0] = "c++"
	map0[1] = "go"
	map0[2] = "java"

	map1 := make(map[int]string)
	map1[0] = "c++"
	map1[1] = "go"
	map1[2] = "java"

	map2 := map[int]string{1: "c++", 2: "go", 3: "java"}
	fmt.Println(map0)
	fmt.Println(map1)
	fmt.Println(map2)

	// 增删改查
	map2[4] = "C#"
	delete(map2, 4)
	map2[3] = "C#"
	value, ok := map2[4]
	if ok {
		fmt.Println("map2[4] =", value)
	} else {
		fmt.Println("map2[4] = null")
	}

	for key, val := range map2 {
		fmt.Println(key, val)
	}
}
