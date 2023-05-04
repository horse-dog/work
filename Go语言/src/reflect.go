package main

import (
	"fmt"
	"reflect"
)

type Foo struct {
	Name string
	Age  int
}

func (this Foo) Disp() {
	fmt.Println(this)
}

func reflectType(arg interface{}) {
	typeinfo := reflect.TypeOf(arg)
	reflectValue := reflect.ValueOf(arg)

	fmt.Println("arg's type is:", typeinfo.Name())
	for i := 0; i < typeinfo.NumField(); i++ {
		fieldinfo := typeinfo.Field(i)
		fmt.Printf("field: %v(%v) = %v\n", fieldinfo.Name, fieldinfo.Type, reflectValue.Field(i))
	}

	fmt.Println(typeinfo.NumMethod())

	for i := 0; i < typeinfo.NumMethod(); i++ {
		methodinfo := typeinfo.Method(i)
		fmt.Printf("method: %v(%v)\n", methodinfo.Name, methodinfo.Type)
	}
}

func main() {
	x := Foo{"xxx", 111}
	reflectType(x)
}
