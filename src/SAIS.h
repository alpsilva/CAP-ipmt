#ifndef SAIS_H
#define SAIS_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "SA.h"
#include <string.h>


#define S true
#define L false
#define UNDEFINED ~0




void recursiveSorting(uint32_t* uintString, size_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, bool* SLTypes, uint32_t* bucketSizes,
	uint32_t* bucketEnds, uint32_t* reducedString, uint32_t* reducedOffsets, uint32_t alphabetSize);
void buildSLTypes(const uint32_t* uintString, bool* SLTypes, uint32_t suffixArrayLen);
bool isLeftmostS(bool* LSTypes, uint32_t i);
void buildBuckets(uint32_t* uintString, size_t suffixArrayLen, uint32_t alphabetSize, uint32_t* bucketSizes);
void findBucketHeads(uint32_t alphabetSize, uint32_t* bucketSizes, uint32_t* bucketHeads);
void findBucketTails(uint32_t alphabetSize, uint32_t* bucketSizes, uint32_t* bucketTails);
void placeLeftmostS(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketTails);
void induceL(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketHeads);
void induceS(uint32_t* uintString, size_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketEnds);
bool equalLeftmostS(uint32_t* uintString, uint32_t suffixArrayLen, bool* LSTypes, uint32_t i, uint32_t j);
void reduceSuffixArray(uint32_t* uintString, size_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, bool* LSTypes, uint32_t* newAlphabetSize, uint32_t* summaryString,
	uint32_t* summaryOffsets, uint32_t* newStringLen);
void sortSuffixArray(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, uint32_t* summaryString, uint32_t* summaryOffsets, uint32_t* bucketSizes,
	uint32_t* bucketEnds, bool* LSTypes, uint32_t alphabetSize);
void remapLeftmostS(uint32_t* uintString, size_t suffixArrayLen, uint32_t* bucketSizes, uint32_t* bucketEnds, uint32_t alphabetSize,
	uint32_t reducedLen, uint32_t* reducedSuffixArray, uint32_t* reducedOffsets, uint32_t* tempSuffixArray);
void recursiveSorting(uint32_t* uintString, size_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, bool* SLTypes, uint32_t* bucketSizes,
	uint32_t* bucketEnds, uint32_t* reducedString, uint32_t* reducedOffsets, uint32_t alphabetSize);
struct SuffixArray* buildSuffixArray(uint8_t* remappedString, size_t remappedStringSize, uint32_t alphabetSize);




#endif // !SAIS_H