#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "headers/wbmp_io.h"

unsigned int height = 0;
unsigned int width = 0;

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
    printf("Loading first frame!\n"); 
    char *original_frame = argv[1];
    raw_data_size = load_wbmp(original_frame, &width, &height, &current_structure);
    if (!raw_data_size > 0) {
        printf("The image at the provided path is invalid.\nPlease verify your path.\nThis App quit!\n");
        return 2;
    }
    
    printf("%s", current_structure);    
    //testing
    char *opath = "output.wbmp";
    save_wbmp(opath, current_structure, width, height, raw_data_size);

    //Cleanup before exit.
    free(current_structure);
    free(last_structure);
    return 0;
}
