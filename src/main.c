#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "headers/wbmp_io.h"

unsigned int height = 0;
unsigned int width = 0;

unsigned long current_step = 0;
unsigned long final_step = 0;
size_t  used_digits = 0;

size_t raw_data_size;

uint8_t *last_structure = NULL;
uint8_t *current_structure = NULL;

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
    if (!raw_data_size > 0) {
        printf("The image at the provided path is invalid.\nPlease verify your path.\nThis App quit!\n");
        return 2;
    }
   
    //Actual logic
    printf("Loading complete, running loop:\n");
    while (current_step < final_step){
        printf("Rendering frame: %0*lu/", used_digits, current_step + 1);
        printf("%lu\n", final_step);
        //THINGS
        //Tim da muesch du koche
        current_step += 1;
    }
    printf("Rendered all frames, cleaning up!\n");
 
    //Cleanup before exit.
    free(current_structure);
    free(last_structure);
    return 0;
}
