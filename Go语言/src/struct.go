package main

import (
	"fmt"
)

type vector []int // c++: using vector = std::vector<int>;

type book struct {
	name  string /*> 书名 */
	price int    /*> 价格 */
}

func changePrice(x *book) {
	x.price = 30
}

func (this *book) getPrice() int {
	return this.price
}

// 操作是在一个右值副本里进行的，无法更改对象的属性
func (this book) setPriceErr(price int) {
	this.price = price
}

// 正确的实现
func (this *book) setPrice(price int) {
	this.price = price
}

func main() {
	var vc vector = vector{1, 2, 3}
	fmt.Println(vc)

	x := book{"go", 20}
	fmt.Println(x)
	changePrice(&x)
	fmt.Println(x)
	x.setPrice(40)
	x.setPriceErr(50)
	fmt.Println(x)
}
