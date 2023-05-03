#### Hello, World

```go
package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println("hello world!")
	time.Sleep(time.Second)
}
```

#### 变量声明

```go
package main

import (
	"fmt"
)

// global varibales
var g_A = 20

// g_B := 20 // error

func main() {

	var a int
	var b int = 10
	var c = 10
	d := 10
	var (
		e = 10
		f = "sb"
	)

	fmt.Printf("a = %2d, type = %T\n", a, a)
	fmt.Printf("b = %2d, type = %T\n", b, b)
	fmt.Printf("c = %2d, type = %T\n", c, c)
	fmt.Printf("d = %2d, type = %T\n", d, d)
	fmt.Printf("e = %2d, type = %T\n", e, e)
	fmt.Printf("f = %2s, type = %T\n", f, f)

}

```

#### 常量声明

```go
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

```

#### 函数

```go
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

```

#### 包

<img src="img/1.png" style="zoom:100%" />

- main包导入pkg1，将先导入pkg1中依赖的pkg2，然后初始化全局常量和全局变量，然后调用init函数。pkg1导入pkg2的方式是类似的，是一种递归调用

- main包中的init函数将再main函数前运行，可以在init函数中进行一些初始化操作

- 一个包中的函数名称的头字母为大写，代表这个函数对外公开，为小写则代表这个函数仅该包内部可见

```go
package main

import (
  lib0        // 导入lib0
  _ lib1      // 将lib1作为匿名包导入，此后即使不使用lib1提供的接口编译也不会报错，可以通过这种方法调用lib1的init方法而不调用其他api
  mylib2 lib2 // 导入lib2，并为lib2取别名为mylib2
  . lib3      // 导入lib3内部的所有变量和函数，使用它们时不再需要 "lib3.func(...)"，慎用
)
```