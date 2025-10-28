#include "headers/wbmp_io.h"

size_t load_wbmp(char *file_path, unsigned int *width, unsigned int *height, uint8_t **buffer){
    printf("Loading WBMP file: %s\n", file_path);
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("Could not read the input file!\n");
        *buffer = NULL;
        return 0;
    }
#ifdef DEBUG
    printf("\tFile opened!\n");
#endif

    //Verifying the header
    uint8_t header[2] = {0xFF, 0xFF};
    size_t bytes_read = fread(header, 1, 2, file);
    if (bytes_read != 2) {
        printf("Couldn't retrive header!\n");
        return 0;
    }
    
    if (header[0] != 0x00 || header[1] != 0x00) {
        printf("The header is invalid!\n");
        return 0;
    }
#ifdef DEBUG
    printf("\tVerified header!\n");
#endif

    // Getting image dimensions
    uint8_t img_width[5];
    for (int i = 0; i < 5; i++) {
        uint8_t byte = fgetc(file);
        img_width[i] = byte;
        if (is_uintvar_done(byte)){
            break;
        }
        if (i == 4 && !is_uintvar_done(byte)) {
            printf("WBMP image to large. Width exceeds 32 bits!\n");
            return 0;
        }
    }
    *width = uintvar_to_int(img_width);

    uint8_t img_height[5];
    for (int i = 0; i < 5; i++) {
        uint8_t byte = fgetc(file);
        img_height[i] = byte;

        if (is_uintvar_done(byte)){
            break;
        }
        if (i == 4 && !is_uintvar_done(byte)) {
            printf("WBMP image to large. Height exceeds 32 bits!\n");
            return 0;
        }
    }
    *height = uintvar_to_int(img_height);

#ifdef DEBUG
    printf("\tLoaded image dimesnions!\n");
#endif
    //Getting the image data
    size_t calc_data_size = (size_t)(((*width * *height) + 7) / 8);
    if (calc_data_size == 0) {
        printf("Not understood what to allocate!\n");
        *buffer = 0;
        return 0;
    }
    *buffer = (uint8_t *)malloc(calc_data_size);

    if (*buffer == NULL){
        printf("Could not allocate memory for the image data!\n");
        return 0;
    }

#ifdef DEBUG
    printf("\tPrepared memory for the image!\n"); 
#endif

    size_t data_amount = fread(*buffer, 1, calc_data_size, file);
    if(data_amount != calc_data_size) {
        perror("Not entire image data could be read!\n");
        return 0;
    }

#ifdef DEBUG
    printf("\tLoaded the image data!\n"); 
#endif    

    //Here you close the wbmp file
    fclose(file);
#ifdef DEBUG
    printf("\tNo errors occured during image loading!\n\n");
#endif
    return calc_data_size;
}

int save_wbmp(char *file_path, uint8_t *img_data, unsigned int width, unsigned int height, size_t data_length){
    printf("Saving WBMP file: %s\n", file_path);
    FILE *file = fopen(file_path, "wb");
    if (file == NULL){
        printf("Opening file did not work.");
        return 2;
    }

#ifdef DEBUG
    printf("\tFile opened susesfully!\n");
#endif

    //Writing the Header
    for (int i = 0; i < 2; i ++) {
        if (fputc(0x00, file) == EOF){
            perror("Could not write header to file!");
            return 1;
        }
    }    
    
#ifdef DEBUG
    printf("\tHeader of the file is written!\n");
#endif

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

#ifdef DEBUG
    printf("\tDimensions are stored too!\n");
#endif

    //Here you save the actual data
    for (size_t i = 0; i < data_length; i++) {
        if (fputc(img_data[i], file) == EOF) {
            perror("Could not write data into file!");
            return 1;
        }
    }

#ifdef DEBUG
    printf("\tImage data stored!\n");
#endif

    //Here you close the wbmp file
    fclose(file);
#ifdef DEBUG
    printf("\tWriting image succesful!\n");
#endif
    return 0;
}
