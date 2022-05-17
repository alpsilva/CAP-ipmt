#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct suffixArray 
{
	uint8_t* text;
	uint32_t length;
	uint32_t* suffixArray;
};

struct suffixArray* initSuffixArray(uint8_t* text, size_t textLen);

