package main

import "fmt"

type Point2 struct {
	x int
	y int
}

type Point3 struct {
	Point2
	z int
}

func (this *Point2) disp() {
	fmt.Println(this.x, this.y)
}

func (this *Point3) disp() {
	fmt.Println(this.x, this.y, this.z)
}

func main() {
	p0 := Point2{0, 1}
	p1 := Point3{p0, 2}
	p0.disp()
	p1.disp()
}
