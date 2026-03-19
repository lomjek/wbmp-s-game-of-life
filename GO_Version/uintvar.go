package main

import "fmt"

func is_uintvar_done(value uint8) (bool) {
	return (value & 0b10000000) == 0;
}

func uintvar_to_uint(number []uint8) (uint32) {
	var result uint32 = 0;
	var data_bytes int = 0;

	for i := 0; i < 5; i++ {
		data_bytes ++;
		if is_uintvar_done(number[i]) {
			break;
		}
		if i == 4{
			fmt.Println("The uintvar is too large for this program. More than 32 bits")
			return 0;
		}
	}

	var shift int = (data_bytes - 1) * 7;
	for i := 0; i < data_bytes; i++ {
		var byte uint8 = number[i];
		result |= (uint32)(byte & 0x7F) << shift
		shift -= 7;
	}
	return result;
}

func uint_to_uintvar(value uint32) ([]uint8){
	if value == 0 {
		return []uint8{0};
	}

	var tmp []uint8;
	var count int = 0;
	for (value != 0) && count < 5 {
		count++;
		tmp = append(tmp, uint8(value & 0x7F));
		value >>= 7;
	}

	for i := count -1; i >= 0; i-- {
		var b uint8 = tmp[i];
		if i != 0 {
			b |= 0b10000000;
		}
		tmp[i] = b;
	}

	//Invert array.
	left, right := 0, len(tmp)-1
    for left < right {
        tmp[left], tmp[right] = tmp[right], tmp[left]
        left++
        right--
    }

	return tmp;
}