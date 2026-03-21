package main

import (
	"fmt"
	"os"
)

func file_exists(path string)(bool){
	info, err := os.Stat(path)
    if os.IsNotExist(err) {
        return false;
    }
    return !info.IsDir()
}

func is_valid_dir(path string)(success bool){
	info, err := os.Stat(path)
    if os.IsNotExist(err) {
        return false;
    }
    return info.IsDir()
}

func create_dir(path string)(success bool){
    err := os.Mkdir(path, 0755)
    if err != nil {
        fmt.Println(err)
        return false;
    }
	return true;
}

func is_dir_empty(path string) (bool) {
    files, err := os.ReadDir(path)
    if err != nil {
        return false
    }
    return len(files) == 0
}