#ifndef BWT_H
#define BWT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "remap.h"
#include "SA.h"


unsigned char bwt(const struct suffixArray* sa, uint32_t i);

struct BwtTable 
{
    struct StringRemapTable* stringRemapTable;
    struct suffixArray* suffixArray;
    uint32_t* characterTable;
    uint32_t* oTable;
    uint32_t** oIndices;
};

struct Iterator
{
    uint32_t left;
    uint32_t right;
    int64_t current;
    const struct suffixArray* suffixArray;
};

void createBwtTable(struct BwtTable* bwtTable, struct suffixArray* suffixArray, struct StringRemapTable* stringRemapTable);

void createIterator(struct Iterator* iter, struct BwtTable* bwtTable, const uint8_t* remapped_pattern);

bool findNextMatch(struct Iterator* iter, size_t* pos);

#endif // !BWT-SEARCH_H