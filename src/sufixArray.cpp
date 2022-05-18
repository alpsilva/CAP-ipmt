#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <string_view>
#include "utils.h"
#include <assert.h>
#include <chrono>

#include <fcntl.h>

#include "SA.h"
#include "BWT.h"
#include "SAIS.h"    


using namespace std;

void saveIndexFile(string fileName) {

    FILE* textFile;
    if ((textFile = fopen(fileName.data(), "r")) == NULL) {
        cout << "couldn't open textFile\n" << endl;
        exit(1);
    }
    fseek(textFile, 0, SEEK_END);
    const size_t fileSize = ftell(textFile);
    rewind(textFile);
    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * fileSize);
    fread(str, sizeof(uint8_t), fileSize, textFile);
    fclose(textFile);

    struct StringRemapTable rt;
    createRemapTable(&rt, str);
    uint8_t* remapped = (uint8_t *)malloc(sizeof(uint8_t) * fileSize);

    remapString(remapped, str, &rt);
    size_t remappedStringSize = strlen((char*)remapped) + 1;


    struct SuffixArray* newSa = buildSuffixArray(remapped, remappedStringSize, rt.alphabetSize);
    struct BwtTable bwtTable;
    createBwtTable(&bwtTable, newSa, &rt);

    FILE* saveFile;
    if ((saveFile = fopen((fileName + ".idx").data(), "wb")) == NULL)
    {
        cout << "failed to create textFile" << endl;
    }

    fwrite(&fileSize, sizeof(size_t), 1, saveFile);
    fwrite(&newSa->length, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.characterTableSize, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.oTableSize, sizeof(size_t), 1, saveFile);
    fwrite(&bwtTable.oIndicesSize, sizeof(size_t), 1, saveFile);

    fwrite(&bwtTable.stringRemapTable->alphabetSize, sizeof(uint8_t), 1, saveFile);
    fwrite(bwtTable.stringRemapTable->charTable, sizeof(char), 256, saveFile);
    fwrite(bwtTable.stringRemapTable->remappedCharTable, sizeof(char), 192, saveFile);

    fwrite(str, sizeof(uint8_t), fileSize, saveFile);
    fwrite(newSa->suffixArray, sizeof(uint32_t), bwtTable.suffixArray->length, saveFile);
    fwrite(bwtTable.characterTable, sizeof(uint32_t), bwtTable.characterTableSize, saveFile);
    fwrite(bwtTable.oTable, sizeof(uint32_t), bwtTable.oTableSize, saveFile);

    fclose(saveFile);

    free(newSa->text);
    free(newSa->suffixArray);
    free(bwtTable.characterTable);
    free(bwtTable.oTable);
    free(bwtTable.oIndices);

    return;
}

void searchPattern(string fileName, vector<string> patterns, bool isCount) 
{
    cout << (fileName).data() << endl;
    FILE* indexFile;
    if ((indexFile = fopen((fileName).data(), "rb")) == NULL) {
        cout << "couldn't open textFile\n" << endl;
        exit(1);
    }

    size_t strLen;
    size_t suffixArrayLen;
    size_t bwtCharacterTableSize;
    size_t bwtOTableSize;
    size_t bwtOIndicesSize;
    fread(&strLen, sizeof(size_t), 1, indexFile);
    fread(&suffixArrayLen, sizeof(size_t), 1, indexFile);
    fread(&bwtCharacterTableSize, sizeof(size_t), 1, indexFile);
    fread(&bwtOTableSize, sizeof(size_t), 1, indexFile);
    fread(&bwtOIndicesSize, sizeof(size_t), 1, indexFile);

    uint8_t* str = (uint8_t*)malloc(sizeof(uint8_t) * strLen);
    BwtTable bwtTable;
    bwtTable.characterTable = (uint32_t*)malloc(sizeof(uint32_t) * bwtCharacterTableSize);
    bwtTable.oTable = (uint32_t*)malloc(sizeof(uint32_t) * bwtOTableSize);
    bwtTable.oIndices = (uint32_t**)malloc(sizeof(uint32_t*) * bwtOIndicesSize);
    StringRemapTable rt;
    bwtTable.stringRemapTable = &rt;
    struct SuffixArray* suffixArray = (struct SuffixArray*)malloc(sizeof(struct SuffixArray));
    suffixArray->suffixArray = (uint32_t*)malloc(sizeof(uint32_t) * suffixArrayLen);
    suffixArray->length = suffixArrayLen;

    fread(&bwtTable.stringRemapTable->alphabetSize, sizeof(uint8_t), 1, indexFile);
    fread(bwtTable.stringRemapTable->charTable, sizeof(char), 256, indexFile);
    fread(bwtTable.stringRemapTable->remappedCharTable, sizeof(char), 192, indexFile);

    fread(str, sizeof(uint8_t), strLen, indexFile);
    fread(suffixArray->suffixArray, sizeof(uint32_t), suffixArray->length, indexFile);
    fread(bwtTable.characterTable, sizeof(uint32_t), bwtCharacterTableSize, indexFile);
    fread(bwtTable.oTable, sizeof(uint32_t), bwtOTableSize, indexFile);
    
    fclose(indexFile);

    bwtTable.suffixArray = suffixArray;
    recreateOIndices(&bwtTable);

    for(string pattern : patterns)
    {
        if(pattern.size() == 0) continue;
        //auto start = std::chrono::high_resolution_clock::now();

        uint8_t* remmapedPattern = (uint8_t*)malloc(sizeof(uint8_t) * pattern.length());
        remapString(remmapedPattern, (const uint8_t*) pattern.data(), &rt);

        struct Iterator iter;
        size_t pos;

        createIterator(&iter, &bwtTable, remmapedPattern);
        int count = 0;
        while (findNextMatch(&iter, &pos))
        {
            uint32_t startPos = pos;
            int64_t finishPos = pos;
            if(!isCount)
            {
                while (str[startPos] != '\n' || startPos == 0) startPos--;
                while (str[finishPos] != '\n' || finishPos == (strLen - 1)) finishPos++;
                fwrite(str + (startPos * sizeof(char)), sizeof(char), finishPos - startPos, stdout);
                printf("\n");
            }
            else
            {
                count++;
            }            
        }
        if(isCount) cout << count << endl;

        //auto end = std::chrono::high_resolution_clock::now();
        //auto int_s = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        //std::cout << "search elapsed time is " << int_s.count() << " microseconds )" << std::endl;
        free(remmapedPattern);
    }
    
    free(suffixArray->suffixArray);
    free(bwtTable.characterTable);
    free(bwtTable.oTable);
    free(bwtTable.oIndices);

    return;
}

