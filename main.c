#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

unsigned int height = 16;
unsigned int width = 16;

uint8_t *last_structure = NULL;
uint8_t *current_structure = NULL;

int load_wbmp(char *input_path){
    //Here you load the wbmp file
    return -1;
}

//region Saving WBMP
size_t int_to_uintvar(unsigned int n, uint8_t *buffer) {
    uint8_t temp_bytes[5];
    uint32_t value = (uint32_t)n;
    size_t count = 0;

    // 1. Break the integer into 7-bit chunks (LSB-first)
    while (value >= 0x80) {
        // Store the 7 LSBs with the continuation bit (0x80) set
        temp_bytes[count++] = (uint8_t)((value & 0x7F) | 0x80);
        value >>= 7;
    }
    // 2. Store the final byte (remaining bits, MSB unset)
    temp_bytes[count++] = (uint8_t)(value & 0x7F);

    // 3. Reverse the order to get MSB-first (WBMP standard)
    // The first byte written (buffer[0]) must be the one we stored last (temp_bytes[count-1])
    for (size_t i = 0; i < count; i++) {
        buffer[i] = temp_bytes[count - 1 - i];
    }

    return count;
}

int save_wbmp(char *file_path){
    FILE *file = fopen(file_path, "wb");
    if (file == NULL){
        perror("Opening file did not work.");
        return 2;
    }

    //Writing the Header
    for (int i = 0; i < 2; i ++) {
        if (fputc(0x00, file) == EOF){
            perror("Could not write header to file!");
            return 1;
        }
    }    

    //Writing the dimensions
    uint8_t width_buffer[5]; //This is the max size that C support for unsigned int in uintvar segments 
    size_t bytes_width = int_to_uintvar(width, width_buffer); //Writing the int data into the buffer

    uint8_t height_buffer[5];
    size_t bytes_height = int_to_uintvar(height, height_buffer);
    
    for (size_t i = 0; i < bytes_width; i++) {
        if(fputc(width_buffer[i], file) == EOF) {
            perror("Could not write width into file!");
            return 1;
        }
    }

    for (size_t i = 0; i < bytes_height; i++) {
        if(fputc(height_buffer[i], file) == EOF) {
            perror("Could not write width into file!");
            return 1;
        }
    }

    //Here you save the actual data
    for (size_t i = 0; i < sizeof(current_structure); i++) {
        if (fputc(current_structure[i], file) == EOF) {
            perror("Could not write data into file!");
            return 1;
        }
    }

    //Here you close the wbmp file
    fclose(file);
    return -1;
}
//endregion

int main(int argc, char *argv[]){
	printf("Hello World!\nWelcome to this magnificent app.\n\n");
   
    // Get the arguments
 
    // Print each argument for verification
    for (int i = 1; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

	return 0;
	
}
