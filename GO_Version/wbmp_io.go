package main

import (
	"fmt"
	"os"
)

func load_wbmp(file_path string)(finished bool, width uint32, height uint32, file_content []uint8){
	finished = false;
	fmt.Println("Loading WBMP file:", file_path);
	var file *os.File;
	var err error;
	file, err = os.Open(file_path);
	if err != nil {
		fmt.Println("Could not read the input file!");
		return;
	}

	fmt.Println("\tFile opened!");

	//Verifying the header
	var header [2]uint8 = [2]uint8{0xFF, 0xFF};
	var bytes_read int;
	bytes_read, err = file.Read(header[:]);
	if (bytes_read != 2) || (err != nil) {
		fmt.Println("Could not retrive header!");
		return;
	}

	if header != [2]uint8{0x00, 0x00} {
		fmt.Println("The header is invalid!");
		return;
	}

	fmt.Println("\tVerified header!");

	// Getting image dimensions
	var img_width []uint8;
	for i := 0; i < 5; i++ {
		byte_r := make([]uint8, 1);
		bytes_read, err = file.Read(byte_r);
		img_width = append(img_width, byte_r[0]);
		if is_uintvar_done(byte_r[0]) {
			break;
		}
		if (i == 4) && (!is_uintvar_done(byte_r[0])) {
			fmt.Println("WBMP image to large. Width exceeds 32 bits!");
			return;
		}
	}
	width = uintvar_to_uint(img_width);

	var img_height []uint8;
	for i := 0; i < 5; i++ {
		byte_r := make([]uint8, 1);
		bytes_read, err = file.Read(byte_r);
		img_height = append(img_height, byte_r[0]);
		if is_uintvar_done(byte_r[0]) {
			break;
		}
		if (i == 4) && (!is_uintvar_done(byte_r[0])) {
			fmt.Println("WBMP image to large. Height exceeds 32 bits!");
			return;
		}
	}
	height = uintvar_to_uint(img_height);

	fmt.Println("\tLoaded image dimensions!");

	// Getting the image data
	var calc_data_size uint64 = ((uint64(width) *  uint64(height)) + 7) / 8;
	if calc_data_size == 0 {
		fmt.Println("Not understood image dimesnions!");
		return;
	}
	file_content = make([]uint8, calc_data_size);

	bytes_read, err = file.Read(file_content);
	if bytes_read != int(calc_data_size) {
		fmt.Println("Not entire image data could be read!");
		return;
	}

	fmt.Println("\tLoaded image data!");

	file.Close();
	finished = true;
	return;
}

func save_wbmp()(){
	//TODO
}
