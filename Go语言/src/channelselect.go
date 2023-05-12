package main

import "fmt"

func fibnacii(c, quit chan int) {
	x, y := 1, 1

	for {
		select {
		case c <- x:
			tmp := x
			x = y
			y = tmp + y
		case <-quit:
			fmt.Println("quit")
			return
		}

	}
}

func main() {
	c := make(chan int)
	quit := make(chan int)

	go func() {
		for i := 0; i < 10; i++ {
			fmt.Println(<-c)
		}
		quit <- 0
	}()

	fibnacii(c, quit)
}
