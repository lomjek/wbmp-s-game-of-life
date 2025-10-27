#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "headers/wbmp_io.h"
#include "headers/dir_io.h"

unsigned int height = 0;
unsigned int width = 0;

unsigned long current_step = 0;
unsigned long final_step = 0;
size_t  used_digits = 0;

size_t raw_data_size;

uint8_t *last_structure = NULL;
uint8_t *current_structure = NULL;

void update_pixel(int line, int column) {
    // Placeholder for pixel update logic
    // This function should implement the rules of the Game of Life
    // and update the pixel at (line, column) in current_structure
}

bool step() {
    for (int line = 0; line < height; line++) {
        for (int column = 0; column < width; column++) {
            update_pixel(line, column);
        }
    }
    return true;
}

bool save_current_frame() {
    //Writing the current frame
    char *frame_prefix = "frames/frame_%s.wbmp";
    char frame_number[used_digits + 1];
    
    int chars_written = snprintf(frame_number, used_digits + 1, "%0*lu", (int)used_digits, current_step);

    printf("Preparing to save frame: %s\n", frame_number);
    if (chars_written < 0 || chars_written >= used_digits + 1) {
        printf("Error formatting frame number string.\nThis App quit!\n");
        return false;
    }

    char frame_path[19 + used_digits]; // Calculate buffer_size
    int path_chars_written = snprintf(frame_path, sizeof(frame_path), frame_prefix, frame_number);
    if (path_chars_written < 0 || path_chars_written >= sizeof(frame_path)) {
        printf("Error formatting frame path string.\nThis App quit!\n");
        return false;
    }

    save_wbmp(frame_path, current_structure, width, height, raw_data_size);
    printf("Saved frame to %s\n", frame_path);
    return true;
}

int main(int argc, char *argv[]){
	printf("WBMP's game of life!\nVersion Alpha, almost nothing is done yet.\n--------------------\n\n");
   
    // Get the arguments
    if (argc != 3) {
        printf("You did not provide enough ingormation.\n\tTo use this app you must use:\n\texecutable <path_to_original_image> <number_of_iterations>\n\nThis App quit!\n");
        return 1;
    }
   
    char *original_frame = argv[1];
    char *frame_count = argv[2];

    //Handle the animation length input
    char *empty;
    long raw_fc = strtol(frame_count, &empty, 0);
    
    if (raw_fc <= 0) {
        printf("You provided an invalid number of iterations. It has to be bigger than 0.\nThis App quit!\n");
        return 1;   
    }

    if (raw_fc == 9223372036854775807) {
        printf("WARNING: We have recived the biggest possible input number.\nIf this was intended you may ignore this warning.\nIf your number isn't 9223372036854775807, I will have to inform you: That is the maximum number we can do.\n");
    }
 
    final_step = (unsigned long)raw_fc;
    used_digits = (size_t)floor(log10((double)final_step)) + 1;
    
    //Handle the input image
    raw_data_size = load_wbmp(original_frame, &width, &height, &current_structure);
    if (raw_data_size <= 0) {
        printf("The image at the provided path is invalid.\nPlease verify your path.\nThis App quit!\n");
        return 2;
    }
   
    //Verify the directory for the frames
    char *dir_path = "frames";
    if (!is_valid_dir(dir_path)) {
        printf("The frames directory does not exist, creating it now...\n");
        if (!create_dir(dir_path)) {
            printf("Could not create the frames directory.\nPlease create a directory named 'frames' in the executable's folder and try again.\nThis App quit!\n");
            return 3;
        }
        printf("Created the frames directory successfully!\n");
    }

    //Actual logic
    printf("Loading complete, running loop:\n");
    while (current_step < final_step){
        if (!save_current_frame()) {
            printf("Frame %0*lu saved successfully!\n", used_digits, current_step);
        }
        //Rendering the next frame
        printf("Rendering frame: %0*lu/", used_digits, current_step + 1);
        printf("%lu\n", final_step);
        step();

        //Increment the step counter
        current_step += 1;
    }
    
    printf("Rendered all frames, cleaning up!\n");
 
    //Cleanup before exit.
    free(current_structure);
    free(last_structure);
    printf("Cleanup complete, exiting!\n");
    return 0;
}
