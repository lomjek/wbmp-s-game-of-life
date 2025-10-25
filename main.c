#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

unsigned int height = 0;
unsigned int width = 0;

unsigned long long raw_data_size;

uint8_t *last_structure = NULL;
uint8_t *current_structure = NULL;

//region Loading WBMP
bool is_uintvar_done(uint8_t byte){
    return (byte & 0x80) == 0;
}

unsigned int uintvar_to_int(const uint8_t *number) {
    unsigned int result = 0;
    int shift = 0;
    int data_bytes = 0; // To count how many bytes were successfully read

    for (int i = 0; i < 5; i++) { // Get the length of the uintvar
        data_bytes++;
        if (number[i] < 0x80) { // If the number is to large
            break;
        }
        if (i == 4) {
            perror("The uintvar is too large for this program. More than 32 bits");
            break; 
        }
    }

    shift = (data_bytes - 1) * 7; 
    for (int i = 0; i < data_bytes; i++) {
        uint8_t byte = number[i];
        result |= (unsigned int)(byte & 0x7F) << shift; //Append result
        shift -= 7; //Shift to the next byte
    }
    return result;
}

int load_wbmp(char *file_path){
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Could not read the input file");
        return 2;
    }
    //Verifying the header
    uint8_t header[2];
    size_t bytes_read = fread(header, 1, 2, file);
    if (bytes_read != 2) {
        perror("Couldn't retrive header!");
        return 1;
    }
    
    if (header[0] != 0x00 || header[1] != 0x00) {
        perror("The header is invalid!");
        return 1;
    }
    
    //Getting image dimensions
    uint8_t img_width[5];
    for (int i = 0; i < 5; i++) {
        uint8_t byte = fgetc(file);
        img_width[i] = byte;
        if (is_uintvar_done(byte)){
            break;
        }
        if (i == 4 && !is_uintvar_done(byte)) {
            perror("WBMP image to large. Width exceeds 32 bits");
            return 1;
        }
    }
    width = uintvar_to_int(img_width);

    uint8_t img_height[5];
    for (int i = 0; i < 5; i++) {
        uint8_t byte = fgetc(file);
        img_height[i] = byte;
        if (is_uintvar_done(byte)){
            break;
        }
        if (i == 4 && !is_uintvar_done(byte)) {
            perror("WBMP image to large. Width exceeds 32 bits");
            return 1;
        }
    }
    height = uintvar_to_int(img_height);

    printf("%u:", width);
    printf("%u\n", height);

    //Getting the image data
    raw_data_size = (unsigned long long)(((width * height) + 7) / 8);
    printf("%u", raw_data_size);
    current_structure = (uint8_t *)malloc(raw_data_size);
    last_structure = (uint8_t *)malloc(raw_data_size);

    if (current_structure == NULL || last_structure == NULL){
        perror("Could not allocate memory for the image data");
        return 3;
    }
    
    size_t data_amount = fread(current_structure, 1, raw_data_size, file);
    if(data_amount != raw_data_size) {
        perror("Not entire image data could be read!");
        return 1;
    }
    
    //Here you close the wbmp file
    fclose(file);
    return 0;
}
//endregion
//region Saving WBMP
size_t int_to_uintvar(unsigned int value, uint8_t *number) {
    // handle zero explicitly
    if (value == 0) {
        number[0] = 0x00;
        return 1;
    }

    // collect 7-bit chunks from LSB to MSB into a small buffer
    uint8_t tmp[5];
    int count = 0;
    while (value != 0 && count < 5) {
        tmp[count++] = (uint8_t)(value & 0x7F);
        value >>= 7;
    }

    // if value still non-zero here, it exceeded 35 bits — but for 32-bit unsigned this won't happen
    // now write bytes in big-endian order: most-significant chunk first
    for (int i = count - 1; i >= 0; --i) {
        uint8_t b = tmp[i];
        if (i != 0) { // more bytes follow -> set continuation bit
            b |= 0x80;
        }
        *number++ = b;
    }

    return (size_t)count;
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
    for (size_t i = 0; i < raw_data_size; i++) {
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
    for (int i = 0; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

    //testing
    char *fpath = "./file.wbmp";
    load_wbmp(fpath);
    
    char *opath = "./output.wbmp";
    save_wbmp(opath);

    //Cleanup before exit.
    free(current_structure);
    free(last_structure);
    return 0;
}
