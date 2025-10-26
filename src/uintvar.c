#include "headers/uintvar.h"

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


