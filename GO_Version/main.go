package main

import (
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
	"time"
)

// @ {} [] # \ || !=

var start_time time.Time;

var verbose bool = false;

var current_structure []uint8;
var last_structure []uint8;

var current_step uint64 = 0;
var final_step uint64 = 0;
var used_digits uint8 = 0;

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

//region Handle Input
func wait_for_enter() {
    fmt.Print("Press Enter to continue...")
    reader := bufio.NewReader(os.Stdin)
    _, _ = reader.ReadString('\n')
}

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
	switch current_output_type {
		case NONE:
			if strings.HasSuffix(path, ".wbmp") {
				current_output_type = FRAME;
				return prepare_output_file(path);
			} else {
				current_output_type = ANIMATION;
				return prepare_output_folder(path);
			}
		case ANIMATION:
			return prepare_output_folder(path);
		case FRAME:
			return prepare_output_file(path);
		default:
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
	fmt.Println("\t--verbose\tOutputs more info to the console.")
}

func parse_input() {
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
			final_step = uint64(steps);
			used_digits = uint8(math.Floor(math.Log10(float64(final_step))) + 1)
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

		if value == "--verbose" {
			verbose = true;
			continue;
		}

		fmt.Printf("The argument %s is being ignored\n", value);
		wait_for_enter();
	}
}
//endregion
//region GameLogic
func set_pixel(x uint32, y uint32, state bool) {
    //The boundaries are treated as dead, can not be alive
	if (x >= img_width || y >= img_height){
		return;
	}

	var width_bytes uint32 = img_width / 8
	var h_byte_offset uint32 = x / 8;
	var byte_index uint64 = uint64(y * width_bytes + h_byte_offset);

	var byte_content uint8 = current_structure[byte_index];

	var bit_offset uint8 = uint8(7 - (x % 8));
	if state {
		byte_content |= (1 << bit_offset)
	} else {
		byte_content &= ^(1 << bit_offset);
	}

	current_structure[byte_index] = byte_content;
}

func get_pixel(x uint32, y uint32) (state bool){
    //The boundaries are treated as dead, can not be alive
	if (x >= img_width || y >= img_height){
		return;
	}

	var width_bytes uint32 = img_width / 8;
    var h_byte_offset uint32 = x / 8;
    var byte_index uint64 = uint64(y * width_bytes + h_byte_offset);

    var byte_content uint8 = last_structure[byte_index];

    var bit_offset uint8 = uint8(7 - (x % 8));

	return ((byte_content >> bit_offset) & 1) != 0;
}

func get_surrounding_live_pixel_count(x uint32, y uint32)(live_pixels uint8){
	//corners
	if get_pixel(x + 1, y + 1) { live_pixels += 1; }
	if get_pixel(x - 1, y + 1) { live_pixels += 1; }
	if get_pixel(x - 1, y - 1) { live_pixels += 1; }
	if get_pixel(x + 1, y - 1) { live_pixels += 1; }
	//touching
	if get_pixel(x, y + 1) { live_pixels += 1; }
	if get_pixel(x, y - 1) { live_pixels += 1; }
	if get_pixel(x + 1, y) { live_pixels += 1; }
	if get_pixel(x - 1, y) { live_pixels += 1; }

	return live_pixels;
}

func update_pixel(x uint32, y uint32){
	var surrounding_live_pixels uint8 = get_surrounding_live_pixel_count(x, y);
	if get_pixel(x, y) { //If the pixel is alive
		if surrounding_live_pixels < 2 {
			set_pixel(x, y, false);
		} else if surrounding_live_pixels > 3 {
			set_pixel(x, y, false);
		}
	} else {
		if surrounding_live_pixels == 3 { //If the pixel is dead
			set_pixel(x, y, true);
		}
	}
}

func step() {
	last_structure = current_structure;
	for line := 0; line < int(img_width); line++ {
		for column := 0; column < int(img_height); column++ {
			update_pixel(uint32(line), uint32(column));
		}
	}
}

func save_current_frame() {
	var frame_path string = output_path + fmt.Sprintf("/frame_%0[1]*d.wbmp", used_digits, current_step);
	if !save_wbmp(frame_path, img_width, img_height, current_structure) {
		fmt.Println();
		wait_for_enter();
	}
}
//endregion

func main(){
	start_time = get_time()
	fmt.Print("WBMP's game of life!\nVersion Go_1.0\n---------------\n\n");

	parse_input();

	fmt.Printf("Building type=%d with %d steps into %s\n", current_output_type, final_step, output_path)
	
	fmt.Println("----------------");
	print_timestamp(get_elapsed_ms(start_time));
	
	//Main Loop
	for current_step < final_step {
		if current_output_type == ANIMATION {
			save_current_frame();
		}

		fmt.Println("\n----------------")
		print_timestamp(get_elapsed_ms(start_time));

		//Regdering next frame
		fmt.Printf("Rendering frame: %0[1]*d/", used_digits, current_step + 1);
		fmt.Printf("%d\n", final_step);
		
		step()

		//Increment the step counter
		current_step += 1;
	}

	switch current_output_type {
		case ANIMATION:
			save_current_frame();
		case FRAME:
			save_wbmp(output_path, img_width, img_height, current_structure);
	}

	fmt.Println("\n----------------")
	print_timestamp(get_elapsed_ms(start_time))
	fmt.Println("This App quit successfully!");
	os.Exit(0);
}