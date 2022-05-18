#ifndef BWT_H
#define BWT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "remap.h"
#include "SA.h"




struct BwtTable 
{
    struct StringRemapTable* stringRemapTable;
    struct SuffixArray* suffixArray;
    uint32_t* characterTable;
    size_t characterTableSize;
    uint32_t* oTable;
    size_t oTableSize;
    uint32_t** oIndices;
    size_t oIndicesSize;
};

struct Iterator
{
    uint32_t left;
    uint32_t right;
    int64_t current;
    const struct SuffixArray* suffixArray;
};

unsigned char bwt(const struct SuffixArray* suffixArray, uint32_t i);


void createBwtTable(struct BwtTable* bwtTable, struct SuffixArray* suffixArray, struct StringRemapTable* stringRemapTable);

void createIterator(struct Iterator* iter, struct BwtTable* bwtTable, const uint8_t* remapped_pattern);

bool findNextMatch(struct Iterator* iter, size_t* pos);

void recreateOIndices(struct BwtTable* bwtTable);



#endif // !BWT-SEARCH_H