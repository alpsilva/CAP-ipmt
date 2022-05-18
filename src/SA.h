#pragma once
#ifndef REMAP_H
#define REMAP_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>




struct SuffixArray 
{
	uint8_t* text;
	size_t length;
	uint32_t* suffixArray;
};

struct SuffixArray* initSuffixArray(uint8_t* text, size_t textLen);



#endif
