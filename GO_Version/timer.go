package main

import (
	"fmt"
	"time"
)

func get_time()(time.Time){
	return time.Now();
}

func get_elapsed_ms(start time.Time)(time.Duration){
	return time.Since(start);
}

func print_timestamp(time time.Duration){
	var number float64;
	var unit string;
	if time.Milliseconds() >= 1000 {
		number = time.Seconds();
		unit = "s"
	} else {
		number = float64(time.Milliseconds())
		unit = "ms"
	}

	fmt.Printf("%8.2f%s\n", number, unit)
}