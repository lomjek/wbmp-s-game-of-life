package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"
)

var final_steps uint = 0;

type output_type uint;
const (
	NONE output_type = iota
	FRAME
	ANIMATION
)
var current_output_type output_type = NONE;

func print_usage_instructions(){
	fmt.Println("---------------");
    fmt.Println("Correct usage of this programm goes as following:");
    fmt.Println("You need to have three things specified at least. Output, Input and Animation Length.");
    fmt.Println("\t-s\tSteps\tNumber of steps done in animation");
    fmt.Println("\t-i\tInput\tMust be a valid wbmp file.");
    fmt.Println("\t-o\tOutput\tWill make a output file, if path ends with .wbmp, else a folder will be created.");
    fmt.Println("\t-t\tType\tCan be used to manually override type.");
}

func main(){
	fmt.Print("WBMP's game of life!\nVersion Go_1.0\n---------------\n\n");

	var skip_pass bool = false;
	for index, value := range os.Args {
		if index == 0 {
			continue;
		}

		if skip_pass {
			skip_pass = false;
			continue;
		}

		if value == "-s" { //Retrive the step value
			if (index + 1) >= len(os.Args){
				fmt.Println("Didn't find the step value.");
				print_usage_instructions();
				os.Exit(-1);
			}
			var next string = os.Args[index + 1];
			var steps int;
			var errc error;
			steps, errc = strconv.Atoi(next);
			if errc != nil {
				fmt.Println("Cannot read the step count properly.")
				print_usage_instructions();
				os.Exit(-1);
			} 
			if steps <= 0 {
				fmt.Println("Current step value is inappropriate, ", steps);
				print_usage_instructions();
				os.Exit(-1);
			}
			final_steps = uint(steps);
		}

		if value == "-t" { //Retrive the type value
			if (index + 1) >= len(os.Args){
				fmt.Println("Didn't find the step value.");
				print_usage_instructions();
				os.Exit(-1);
			}
			var next string = strings.Trim(os.Args[index + 1], " ");
			switch next {
				case "FRAME":
					current_output_type = FRAME;
				case "ANIMATION":
					current_output_type = ANIMATION;
				default:
					fmt.Println("The output type found isn't valid. It has to be in [FRAME, ANIMATION].")
					print_usage_instructions();
					os.Exit(-1);
			}
		}

		if value == "-i" { //Retrive the input file
			//TODO
		}
	}
}