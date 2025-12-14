#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "headers/wbmp_io.h"
#include "headers/file_io.h"
#include "headers/timer.h"

double start_time = 0;

unsigned int height = 0;
unsigned int width = 0;

unsigned long current_step = 0;
unsigned long final_step = 0;
size_t  used_digits = 0;

size_t raw_data_size;

typedef enum {
    NONE,
    FRAME,
    ANIMATION
} save_mode;
save_mode current_save_mode = NONE;

char *output_path = NULL;

uint8_t *last_structure = NULL;
uint8_t *current_structure = NULL;

#ifdef DEBUG
void print_uint8_binary(uint8_t value) {
    char buf[9];               /* 8 bits + '\0' */
    for (int i = 0; i < 8; ++i) {
        uint8_t bit = (value >> (7 - i)) & 1U;
        buf[i] = bit ? '1' : '0';
    }
    buf[8] = '\0';

    printf("%s\n", buf);
}
#endif

void wait_for_enter() {
    printf("Press Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void set_pixel(unsigned int x, unsigned int y, bool state) {
    //The boundaries are treated as dead, can not be alive
    if (x >= width || y >= height) {
        return;
    }

    unsigned int width_bytes = width / 8;
    unsigned int h_byte_offset = x / 8;
    size_t byte_index = y * width_bytes + h_byte_offset;

    uint8_t byte = current_structure[byte_index];

    unsigned int bit_offset = 7 - (x % 8);
    if (state) {
        byte |= (1 << bit_offset); // Set the bit to 1
    } else {
        byte &= ~(1 << bit_offset); // Set the bit to 0
    }

    current_structure[byte_index] = byte;
}

bool get_pixel(unsigned int x, unsigned int y) {
    //The boundaries are treated as dead, can not be alive
    if (x >= width || y >= height) {
        return false;
    }

    unsigned int width_bytes = width / 8;
    unsigned int h_byte_offset = x / 8;
    size_t byte_index = y * width_bytes + h_byte_offset;

    uint8_t byte = current_structure[byte_index];

    unsigned int bit_offset = 7 - (x % 8);

    return (byte >> bit_offset) & 1;
}

unsigned int get_surrounding_live_pixel_count(int x, int y) {
    unsigned int count = 0;
    //corners
    count += get_pixel(x+1, y+1);
    count += get_pixel(x-1, y+1);
    count += get_pixel(x-1, y-1);
    count += get_pixel(x+1, y-1);
    //touching
    count += get_pixel(x, y+1);
    count += get_pixel(x, y-1);
    count += get_pixel(x+1, y);
    count += get_pixel(x-1, y);

    return count;
}
void update_pixel(int x, int y) {
    unsigned int surrounding_live_pixels = get_surrounding_live_pixel_count(x, y);
    if (get_pixel(x, y)) { //If pixel is alive.
        if (surrounding_live_pixels < 2) {
            set_pixel(x, y, false);
        } else if (surrounding_live_pixels > 3) {
            set_pixel(x, y, false);
        }
    } else { //the pixel is dead.
        if (surrounding_live_pixels == 3) {
            set_pixel(x, y, true);
        }
    }
}

bool step() {
    //Push current structure to last structure
    *last_structure = *current_structure;
    for (int line = 0; line < width; line++) {
        for (int column = 0; column < height; column++) {
            update_pixel(line, column);
        }
    }
    return true;
}

bool save_current_frame() {
    //Writing the current frame
    printf("->%s\n", output_path);

    size_t buffer_size = strlen(output_path) + 1 + 6 + used_digits + 5 + 1;
    char frame_path[buffer_size];
    int path_chars_written = snprintf(
        frame_path, 
        sizeof(frame_path), 
        "%s/frame_%0*lu.wbmp",
        output_path,
        (int)used_digits,
        current_step
    );

    if (path_chars_written < 0 || path_chars_written >= sizeof(frame_path)) {
        printf("Error formatting frame path string.\nThis App quit!\n");
        return false;
    }

    int result = save_wbmp(frame_path, current_structure, width, height, raw_data_size);
    if (result != 0) {
        printf("\n");
        wait_for_enter();
    }

    printf("Saved frame to %s\n", frame_path);
    return true;
}

bool endsWith(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return false;
    }
 
    size_t len_str = strlen(str);
    size_t len_suffix = strlen(suffix);

    if (len_suffix > len_str) {
        return false;
    }

    return (strncmp(str + len_str - len_suffix, suffix, len_suffix) == 0);
}

//region Handle Input
bool get_anim_len_from_arg(char *frame_count){
    char *empty;
    long raw_fc = strtol(frame_count, &empty, 0);
    
    if (raw_fc <= 0) {
        printf("You provided an invalid number of iterations. It has to be bigger than 0.\nThis App quit!\n");
        return false;   
    }

    if (raw_fc == 9223372036854775807) {
        printf("WARNING: We have recived the biggest possible input number.\nIf this was intended you may ignore this warning.\nIf your number isn't 9223372036854775807, I will have to inform you: That is the maximum number we can do.\n");
        wait_for_enter();
    }
 
    final_step = (unsigned long)raw_fc;
    used_digits = (size_t)floor(log10((double)final_step)) + 1;
    return true;
}

bool load_input_image(char *original_frame) {
    char *extension = ".wbmp";
    if (!endsWith(original_frame, extension)) {
        printf("The input file does not end with .wbmp\n");
        wait_for_enter();
    }

    raw_data_size = load_wbmp(original_frame, &width, &height, &current_structure);

    last_structure = (uint8_t *)malloc(raw_data_size);

    if (last_structure == NULL){
        printf("Could not allocate memory for the image data!\n");
        return false;
    }

    if (raw_data_size <= 0) {
        printf("The image at the provided path is invalid.");
        return false;
    }

    return true;
}

bool prepare_output_folder(char *dir_path) {
    if (!is_valid_dir(dir_path)) {
        printf("The frames directory does not exist, creating it now...\n");
        if (!create_dir(dir_path)) {
            printf("Could not create the frames directory.");
            return false;
        }
    } 
    else {
        if (!is_dir_empty(dir_path)) {
            printf("The output directory isn't empty...\n");
            wait_for_enter();
        }
    }
    output_path = dir_path;
    return true;
}

bool prepare_output_file(char *file_path) {
    if (file_exists(file_path)) {
        printf("The output file already exists, continuing will overwrite it.\n");
        wait_for_enter();
    }
    output_path = file_path;
    return true;
}

bool get_output_ready(char *path) {
    if (current_save_mode == NONE) {
        const char *wbmp_ext = ".wbmp";
        if (endsWith(path, wbmp_ext)) {
            current_save_mode = FRAME;
            return prepare_output_file(path);
        }
        else {
            return prepare_output_folder(path);
        }
    }
    else if (current_save_mode == ANIMATION) {
        return prepare_output_folder(path);
    }
    else if (current_save_mode == FRAME) {
        return prepare_output_file(path);
    } 
    else {
        return false;
    }
}

bool get_anim_type(char *argument) {
    char *animation = "ANIMATION";
    char *frame = "FRAME";
    if (strcmp(argument, animation) == 0) {
        current_save_mode = ANIMATION;
        return true;
    }
    else if (strcmp(argument, frame) == 0) {
        current_save_mode = FRAME;
        return true;
    }
    else {
        return false;
    }
}

void print_usage_instructions(){
    printf("\n---------------\n");
    printf("Correct usage of this programm goes as following:\n");
    printf("You need to have three things specified at least. Output, Input and Animation Length.\n");
    printf("\t-s\tSteps\tNumber of steps done in animation\n");
    printf("\t-i\tInput\tMust be a valid wbmp file.\n");
    printf("\t-o\tOutput\tWill make a output file, if path ends with .wbmp, else a folder will be created.\n");
    printf("\t-t\tType\tCan be used to manually override type.\n");
}
//endregion

int main(int argc, char *argv[]){
    start_time = get_time();
	printf("WBMP's game of life!\nVersion 1.0\n---------------\n\n");
   
    unsigned int argument_pointer = 1u;

    while (argument_pointer < argc) {
        if (strcmp(argv[argument_pointer], "-s") == 0) {
            if (argument_pointer + 1 >= argc) {
                printf("No valid step amount recived.");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
            bool result =  get_anim_len_from_arg(argv[argument_pointer]);
            if (!result) {
                printf("The step count could not be parsed...");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
        }
        else if (strcmp(argv[argument_pointer], "-i") == 0) {
            if (argument_pointer + 1 >= argc) {
                printf("No valid input path recived.");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
            bool result = load_input_image(argv[argument_pointer]);
            if (!result) {
                printf("The image path is not valid...");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
        }
        else if (strcmp(argv[argument_pointer], "-t") == 0) {
            if (argument_pointer + 1 >= argc) {
                printf("No valid Animation type recived.");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
            bool result = get_anim_type(argv[argument_pointer]);
            if (!result) {
                printf("The Animation Type is not valid...");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
        }
        else if (strcmp(argv[argument_pointer], "-o") == 0) {
            if (argument_pointer + 1 >= argc) {
                printf("No valid output path recived.");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
            bool result = get_output_ready(argv[argument_pointer]);
            if (!result) {
                printf("The output path is not valid...");
                print_usage_instructions();
                return 1;
            }
            argument_pointer ++;
        }
        else {
            printf("The argument %s is being ignored.\n", argv[argument_pointer]);
            wait_for_enter();
            argument_pointer ++;
        }
    }

    printf("Building type=%u with %u steps into %s\n", current_save_mode, final_step, output_path);

    printf("----------------\n");
    print_timestamp(get_elapsed_ms(start_time));

    //Main Loop
    while (current_step < final_step){
        if (current_save_mode == ANIMATION){
            if (!save_current_frame()) {
                printf("Frame %0*lu saved successfully!\n", used_digits, current_step);
            }
        }

        printf("\n----------------\n");
        print_timestamp(get_elapsed_ms(start_time));

        //Rendering the next frame
        printf("Rendering frame: %0*lu/", used_digits, current_step + 1);
        printf("%lu\n", final_step);

        step();

        //Increment the step counter
        current_step += 1;
    }

    // Save the last frame!
    if (current_save_mode == ANIMATION){
        if (!save_current_frame()) {
            printf("Frame %0*lu saved successfully!\n", used_digits, current_step);
        }
    }
    else if (current_save_mode == FRAME){
        save_wbmp(output_path, current_structure, width, height, raw_data_size);
    }

    printf("\n----------------\n");
    print_timestamp(get_elapsed_ms(start_time));
    printf("Rendered all frames, cleaning up!\n");
 
    //Cleanup before exit.
    free(current_structure);
    free(last_structure);
    printf("This App quit successfully!\n");
    return 0;
}
