#include "BWT.h"

unsigned char bwt(const struct suffixArray* suffixArray, uint32_t i)
{
    uint32_t suffix = suffixArray->suffixArray[i];
    if (suffix == 0)
    {
        return '\0';
    }
    else
    {
        return suffixArray->text[suffix - 1];
    }
}

void createBwtTable(struct BwtTable* bwtTable, struct suffixArray* suffixArray, struct StringRemapTable* stringRemapTable) 
{
    bwtTable->stringRemapTable = stringRemapTable;
    bwtTable->suffixArray = suffixArray;

    uint32_t* charCounts = (uint32_t*)calloc(stringRemapTable->alphabetSize, sizeof(uint32_t));
    if (charCounts == NULL)
    {
        printf("couldn't allocate charCounts, exiting\n");
        exit(1);
    }

    for (uint32_t i = 0; i < suffixArray->length; i++) 
    {
        charCounts[suffixArray->text[i]]++;
    }
    bwtTable->characterTable = (uint32_t*)calloc(stringRemapTable->alphabetSize, sizeof(*bwtTable->characterTable));

    for (uint32_t i = 1; i < stringRemapTable->alphabetSize; i++) 
    {
        bwtTable->characterTable[i] = bwtTable->characterTable[i - 1] + charCounts[i - 1];
    }

    free(charCounts);

    bwtTable->oTable = (uint32_t*)malloc((stringRemapTable->alphabetSize) * (suffixArray->length + 1) * sizeof(uint32_t));
    bwtTable->oIndices = (uint32_t**) malloc((suffixArray->length + 1) * sizeof(uint32_t*));
    if (bwtTable->oTable == NULL)
    {
       printf("couldn't allocate oTable, ram size too small");
       exit(1);
    }

    if (bwtTable->oIndices == NULL)
    {
        printf("couldn't allocate oIndices, ram size too small");
        exit(1);
    }

    for (size_t i = 0; i < suffixArray->length + 1; i++) 
    {
        uint32_t* ptr = bwtTable->oTable + stringRemapTable->alphabetSize * i;
        bwtTable->oIndices[i] = ptr;
    }

    for (uint8_t a = 0; a < stringRemapTable->alphabetSize; a++) 
    {
        bwtTable->oIndices[0][a] = 0;
    }

    for (uint8_t a = 0; a < stringRemapTable->alphabetSize; a++) 
    {
        for (uint32_t i = 1; i <= suffixArray->length; i++) 
        {
            bwtTable->oIndices[i][a] = bwtTable->oIndices[i - 1][a] + (bwt(suffixArray, i - 1) == a);
        }
    }
}

void createIterator(struct Iterator* iter, struct BwtTable* bwtTable, const uint8_t* remapped_pattern) 
{
    const struct suffixArray* suffixArray = iter->suffixArray = bwtTable->suffixArray;
    uint32_t suffixArrayLen = suffixArray->length;
    uint32_t m = (uint32_t)strlen((char*)remapped_pattern);
    iter->left = 0;
    iter->right = suffixArrayLen;

    if (m > suffixArrayLen) 
    {
        iter->right = 0; iter->left = 1;
    }

    int64_t i = m - 1;
    while (i >= 0 && iter->left < iter->right) {
        uint8_t a = remapped_pattern[i];
        iter->left = bwtTable->characterTable[a] +  bwtTable->oIndices[iter->left][a];
        iter->right = bwtTable->characterTable[a] + bwtTable->oIndices[iter->right][a];
        i--;
    }

    iter->current = iter->left;
}

bool findNextMatch(struct Iterator* iter, size_t* pos) 
{
    if (iter->current < 0) return false;
    if (iter->current >= iter->right) return false;

    *pos = iter->suffixArray->suffixArray[iter->current];
    iter->current++;
    return true;
}
