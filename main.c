#include <stdio.h>

int load_wbmp(char *input_path){
    //Here you load the wbmp file
    return -1;
}

int save_wbmp(char *file_path){
    //Here you save the wbmp file
    return -1;
}

int main(int argc, char *argv[]){
	printf("Hello World!\nWelcome to this magnificent app.\n\n");
    
    // Print each argument for verification
    for (int i = 1; i < argc; i++) {
        printf("Argument %d: %s\n", i, argv[i]);
    }

	return 0;
	
}
