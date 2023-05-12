package main

import (
	"fmt"
	"reflect"
)

type Foo struct {
	Name string
	Age  int
}

func (this *Foo) Disp() {
	fmt.Println(*this)
}

func reflectType(arg interface{}) {
	typeinfo := reflect.TypeOf(arg)
	reflectValue := reflect.ValueOf(arg)

	if typeinfo.Kind() == reflect.Ptr {
		typeinfo = typeinfo.Elem()
		reflectValue = reflectValue.Elem()
	}

	fmt.Println("arg's type is:", typeinfo.Name())
	for i := 0; i < typeinfo.NumField(); i++ {
		fieldinfo := typeinfo.Field(i)
		var v = reflectValue.Field(i).Interface()
		fmt.Printf("field: %v(%v) = %v\n", fieldinfo.Name, fieldinfo.Type, v)
	}

	for i := 0; i < reflect.TypeOf(arg).NumMethod(); i++ {
		method := reflect.TypeOf(arg).Method(i)
		fmt.Printf("method: %v(%v)\n", method.Name, method.Type)
		reflect.ValueOf(arg).Method(i).Call(nil)
	}
}

func main() {
	x := Foo{"xxx", 111}
	reflectType(&x)
}
