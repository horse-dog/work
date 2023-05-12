package main

import (
	"fmt"
	"time"
)

func main() {
	c := make(chan int)
	go func() {
		defer fmt.Println("goroutine finished...")
		fmt.Println("goroutine running...")
		c <- 666
	}()

	time.Sleep(time.Second)
	num := <-c
	fmt.Println("num = ", num)
	fmt.Println("main goroutine finished...")
	time.Sleep(time.Second)
}
