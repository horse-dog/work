package main

import (
	"encoding/json"
	"fmt"
)

type Movie struct {
	Title  string   `json:"title"`
	Year   int      `json:"year"`
	Price  int      `json:"rmb"`
	Actors []string `json:"actors"`
}

func main() {
	movie := Movie{"JokeKing", 2000, 10, []string{"zyc, zbz"}}
	json_str, _ := json.Marshal(movie)
	fmt.Println((string)(json_str))
	movie = Movie{}
	json.Unmarshal(json_str, &movie)
	fmt.Println(movie)
}
