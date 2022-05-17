#pragma once
#ifndef REMAP_H
#define REMAP_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct StringRemapTable 
{
	char charTable[256];
	char remappedCharTable[128];
	uint32_t alphabetSize;
};

void createRemapTable(struct StringRemapTable* stringRemapTable, const uint8_t* string);
void remapString(uint8_t* remappedString, const uint8_t* inputString, struct StringRemapTable* stringRemapTable);

#endif