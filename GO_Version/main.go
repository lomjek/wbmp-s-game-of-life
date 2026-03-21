package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// @ {} [] # \ || !=

var current_structure []uint8;
var last_structure []uint8;

var current_step uint = 0;
var final_step uint = 0;

var img_width uint32 = 0;
var img_height uint32;

type output_type uint;
const (
	NONE output_type = iota
	FRAME
	ANIMATION
)
var current_output_type output_type = NONE;
var output_path string;

func wait_for_enter() {
    fmt.Print("Press Enter to continue...")
    reader := bufio.NewReader(os.Stdin)
    _, _ = reader.ReadString('\n')
}

//region Handle Input
func prepare_output_folder(dir_path string)(success bool){
	if !is_valid_dir(dir_path) {
		fmt.Println("The frames directory does not exist, creating it now...");
		if !create_dir(dir_path) {
			fmt.Println("Could not create the frames directory.");
			return false;
		}
	} else {
		if !is_dir_empty(dir_path) {
			fmt.Println("The output directory isn't empty...");
			wait_for_enter();
		}
	}
	output_path = dir_path;
	return true;
}

func prepare_output_file(file_path string)(success bool){
	if file_exists(file_path) {
		fmt.Println("The output file already exists, continuing will overwrite it.");
		wait_for_enter();
	}
	output_path = file_path;
	return true;
}

func get_output_ready(path string)(success bool){
	if current_output_type == NONE {
		if strings.HasSuffix(path, ".wbmp") {
			current_output_type = FRAME;
			return prepare_output_file(path);
		} else {
			current_output_type = ANIMATION;
			return prepare_output_folder(path);
		}
	} else if (current_output_type == ANIMATION) {
		return prepare_output_folder(path);
	} else if current_output_type == FRAME {
		return prepare_output_file(path);
	} else {
		return false;
	}
}

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
			final_step = uint(steps);
			skip_pass = true;
			continue;
		}

		if value == "-t" { //Retrive the type value
			if (index + 1) >= len(os.Args){
				fmt.Println("Didn't find the type value.");
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
			skip_pass = true;
			continue;
		}

		if value == "-i" { //Retrive the input file
			if (index + 1) >= len(os.Args){
				fmt.Println("Didn't find the input value.");
				print_usage_instructions();
				os.Exit(-1);
			}
			var file_name string = strings.Trim(os.Args[index + 1], " ");
			no_err, r_width, r_height, content := load_wbmp(file_name);
			if (no_err == false) || (r_width == 0) || (r_height == 0) {
				fmt.Println("The image at the provided path is invalid.");
				os.Exit(-1);
			}
			current_structure = content;
			last_structure = content;
			img_width = r_width;
			img_height = r_height;
			skip_pass = true;
			continue;
		}

		if value == "-o" { //Retrive the output
			if (index + 1) >= len(os.Args){
				fmt.Println("Didn't find the output value.");
				print_usage_instructions();
				os.Exit(-1);
			}
			var result bool = get_output_ready(strings.Trim(os.Args[index + 1], " "));
			if !result {
				fmt.Println("The output path is not valid...");
				print_usage_instructions();
				os.Exit(-1);
			}
			skip_pass = true;
			continue;
		}

		fmt.Printf("The argument %s is being ignored\n", value);
		wait_for_enter();
	}

	fmt.Printf("Building type=%d with %d steps into %s\n", current_output_type, final_step, output_path)
	fmt.Println("----------------");
	//print_timestamp
	save_wbmp("./output.wbmp", img_width, img_height, current_structure);
}