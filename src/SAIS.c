#include "SAIS.h"
#include "SA.h"

void buildSLTypes(const uint32_t* uintString, bool* SLTypes, uint32_t suffixArrayLen)
{
	SLTypes[suffixArrayLen] = S;
	if (suffixArrayLen == 0) return;
	SLTypes[suffixArrayLen - 1] = L;

	for (size_t i = suffixArrayLen; i > 0; i--) 
	{
		if (uintString[i - 1] > uintString[i])
		{
			SLTypes[i - 1] = L;
		}
		else if (uintString[i - 1] == uintString[i] && SLTypes[i] == L)
		{
			SLTypes[i - 1] = L;
		}
		else 
		{
			SLTypes[i - 1] = S;
		}
	}
}

bool isLeftmostS(bool* LSTypes, uint32_t i) {
	if (i == 0)
	{
		return false;
	}
	else
	{
		return LSTypes[i] == S && LSTypes[i - 1] == L;
	}
}

void buildBuckets(uint32_t* uintString, size_t suffixArrayLen, uint32_t alphabetSize, uint32_t* bucketSizes) 
{
	memset(bucketSizes, 0, alphabetSize* sizeof(uint32_t));
	for (size_t i = 0; i < suffixArrayLen + 1; i++) 
	{
		bucketSizes[uintString[i]]++;
	}
}

void findBucketHeads(uint32_t alphabetSize, uint32_t* bucketSizes, uint32_t* bucketHeads)
{
	bucketHeads[0] = 0;
	for (size_t i = 1; i < alphabetSize; i++) 
	{
		bucketHeads[i] = bucketHeads[i - 1] + bucketSizes[i - 1];
	}
}

void findBucketTails(uint32_t alphabetSize, uint32_t* bucketSizes, uint32_t* bucketTails) 
{
	bucketTails[0] = bucketSizes[0];
	for (size_t i = 1; i < alphabetSize; i++) 
	{
		bucketTails[i] = bucketTails[i - 1] + bucketSizes[i];
	}
}

void placeLeftmostS(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketTails) 
{
	findBucketTails(alphabetSize, bucketSizes, bucketTails);
	for (size_t i = 0; i < suffixArrayLen + 1; i++) 
	{
		if (isLeftmostS(LSTypes, i))
		{
			bucketTails[uintString[i]]--;
			tempSuffixArray[bucketTails[uintString[i]]] = i;
		}
	}
}

void induceL(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketHeads) 
{
	findBucketHeads(alphabetSize, bucketSizes, bucketHeads);
	for (size_t i = 0; i < suffixArrayLen + 1; i++) 
	{
		if (tempSuffixArray[i] == UNDEFINED) continue;
		if (tempSuffixArray[i] == 0) continue;
		uint32_t j = tempSuffixArray[i] - 1;

		if (LSTypes[j] == L) 
		{
			tempSuffixArray[bucketHeads[uintString[j]]] = j;
			bucketHeads[uintString[j]]++;
		}
	}
}

void induceS(uint32_t* uintString, size_t suffixArrayLen, uint32_t alphabetSize, uint32_t* tempSuffixArray, bool* LSTypes, uint32_t* bucketSizes, uint32_t* bucketEnds) 
{
	findBucketTails(alphabetSize, bucketSizes, bucketEnds);
	for (size_t i = suffixArrayLen + 1; i > 0; i--) 
	{
		if (tempSuffixArray[i - 1] == 0) continue;

		uint32_t j = tempSuffixArray[i - 1] - 1;
		if (LSTypes[j] == S) 
		{
			bucketEnds[uintString[j]]--;
			tempSuffixArray[bucketEnds[uintString[j]]] = j;
		}
	}
}

bool equalLeftmostS(uint32_t* uintString, uint32_t suffixArrayLen, bool* LSTypes, uint32_t i, uint32_t j)
{
	if (i == suffixArrayLen + 1 || j == suffixArrayLen + 1) return false;

	uint32_t k = 0;
	while (true) 
	{
		bool iLeftmostS = isLeftmostS(LSTypes, i + k);
		bool jLeftmostS = isLeftmostS(LSTypes, j + k);

		if (k > 0 && iLeftmostS && jLeftmostS) return true;
		if (iLeftmostS != jLeftmostS || uintString[i + k] != uintString[j + k] ) return false;

		k++;
	}
	return true;
};

void reduceSuffixArray(uint32_t* uintString,size_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, bool* LSTypes, uint32_t* newAlphabetSize, uint32_t* summaryString, 
	uint32_t* summaryOffsets, uint32_t* newStringLen) 
{
	memset(namesBuffer, UNDEFINED, (suffixArrayLen + 1) * sizeof(uint32_t));

	uint32_t name = 0;
	namesBuffer[tempSuffixArray[0]] = name;
	uint32_t lastSuffix = tempSuffixArray[0];
	for (size_t i = 1; i < suffixArrayLen + 1; i++) 
	{
		uint32_t j = tempSuffixArray[i];
		if (!isLeftmostS(LSTypes, j)) continue;

		if (!equalLeftmostS(uintString, suffixArrayLen, LSTypes, lastSuffix, j)) 
		{
			name++;
		}
		lastSuffix= j;
		namesBuffer[j] = name;
	}

	*newAlphabetSize = name + 1;
	uint32_t j = 0;
	for (size_t i = 0; i < suffixArrayLen + 1; i++) 
	{
		name = namesBuffer[i];
		if (name == UNDEFINED) continue;

		summaryOffsets[j] = i;
		summaryString[j] = name;
		j++;
	}

	*newStringLen = j - 1;
}

void sortSuffixArray(uint32_t* uintString, uint32_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, uint32_t* summaryString, uint32_t* summaryOffsets, 
	uint32_t* bucketSizes, uint32_t* bucketEnds, bool* LSTypes, uint32_t alphabetSize) 
{
	if (suffixArrayLen == 0) 
	{
		tempSuffixArray[0] = 0;
		return;
	}

	if (alphabetSize == suffixArrayLen + 1) 
	{
		tempSuffixArray[0] = suffixArrayLen;
		for (size_t i = 0; i < suffixArrayLen; i++) 
		{
			uint32_t j = uintString[i];
			tempSuffixArray[j] = i;
		}
	}
	else 
	{
		recursiveSorting(uintString, suffixArrayLen, tempSuffixArray,namesBuffer, LSTypes, bucketSizes, bucketEnds, summaryString, summaryOffsets, alphabetSize);
	}
}

void remapLeftmostS( uint32_t* uintString, size_t suffixArrayLen, uint32_t* bucketSizes, uint32_t* bucketEnds, uint32_t alphabetSize,
	uint32_t reducedLen, uint32_t* reducedSuffixArray, uint32_t* reducedOffsets, uint32_t* tempSuffixArray) 
{
	findBucketTails(alphabetSize, bucketSizes, bucketEnds);
	for (size_t i = reducedLen + 1; i > 0; i--) 
	{
		uint32_t tempSuffixArrayIndex = reducedOffsets[reducedSuffixArray[i - 1]];
		uint32_t bucketEndsIndex = uintString[tempSuffixArrayIndex];
		bucketEnds[bucketEndsIndex]--;
		tempSuffixArray[bucketEnds[bucketEndsIndex]] = tempSuffixArrayIndex;
	}
	tempSuffixArray[0] = suffixArrayLen;
}

void recursiveSorting(uint32_t* uintString, size_t suffixArrayLen, uint32_t* tempSuffixArray, uint32_t* namesBuffer, bool* SLTypes, uint32_t* bucketSizes,
	uint32_t* bucketEnds, uint32_t* reducedString, uint32_t* reducedOffsets, uint32_t alphabetSize) 
{
	buildSLTypes(uintString, SLTypes, suffixArrayLen);
	buildBuckets(uintString, suffixArrayLen, alphabetSize, bucketSizes);
	memset(tempSuffixArray, UNDEFINED, (suffixArrayLen + 1) * sizeof(uint32_t));
	placeLeftmostS(uintString, suffixArrayLen, alphabetSize, tempSuffixArray, SLTypes, bucketSizes, bucketEnds);
	induceL(uintString, suffixArrayLen, alphabetSize, tempSuffixArray, SLTypes, bucketSizes, bucketEnds);
	induceS(uintString, suffixArrayLen, alphabetSize, tempSuffixArray, SLTypes, bucketSizes, bucketEnds);
	uint32_t newAlphabetSize;
	uint32_t newStringLen;
	reduceSuffixArray(uintString, suffixArrayLen, tempSuffixArray, namesBuffer, SLTypes, &newAlphabetSize, reducedString, reducedOffsets, &newStringLen);

	uint32_t* newTempSuffixArray = tempSuffixArray + suffixArrayLen + 1;
	uint32_t* newNamesBuffer = namesBuffer + suffixArrayLen + 1;
	bool* newSLTypes = SLTypes + suffixArrayLen + 1;
	uint32_t* newSummaryString = reducedString + suffixArrayLen + 1;
	uint32_t* newSummaryOffsets = reducedOffsets + suffixArrayLen + 1;
	uint32_t* newBucketSizes = bucketSizes + alphabetSize;
	uint32_t* newBucketExtremes = bucketEnds + alphabetSize;

	sortSuffixArray(reducedString, newStringLen, newTempSuffixArray, newNamesBuffer, newSummaryString, newSummaryOffsets, newBucketSizes, newBucketExtremes, newSLTypes,
		newAlphabetSize);
	memset(tempSuffixArray, UNDEFINED, (suffixArrayLen + 1) * sizeof(uint32_t));
	remapLeftmostS(uintString, suffixArrayLen, bucketSizes, bucketEnds, alphabetSize,  newStringLen, newTempSuffixArray, reducedOffsets,
		tempSuffixArray);
	induceL(uintString, suffixArrayLen, alphabetSize, tempSuffixArray, SLTypes, bucketSizes, bucketEnds);
	induceS(uintString, suffixArrayLen, alphabetSize, tempSuffixArray, SLTypes, bucketSizes, bucketEnds);
}

struct suffixArray* buildSuffixArray(uint8_t* remappedString, size_t remappedStringSize, uint32_t alphabetSize)
{
	struct suffixArray* suffixArray = initSuffixArray(remappedString, remappedStringSize);

	size_t suffixArrayLen = suffixArray->length - 1;
	uint32_t* uintString = (uint32_t*)malloc((suffixArrayLen + 1) * sizeof(uint32_t));
	if (uintString == NULL)
	{
		printf("couldn't allocate uintString, exiting\n");
		exit(1);
	}
	for (size_t i = 0; i < suffixArrayLen; i++) 
	{
		uintString[i] = remappedString[i];
	}
	uintString[suffixArrayLen] = 0;
	
	uint32_t* tempSuffixArray = (uint32_t*)malloc(2 * (suffixArrayLen + 1) * sizeof(uint32_t));
	if (tempSuffixArray == NULL)
	{
		printf("couldn't allocate tempSuffixArray, exiting\n");
		exit(1);
	}
	uint32_t* namesBuffer = (uint32_t*)malloc(2 * (suffixArrayLen + 1) * sizeof(uint32_t));
	if (namesBuffer == NULL)
	{
		printf("couldn't allocate namesBuffer, exiting\n");
		exit(1);
	}
	uint32_t* summaryString = (uint32_t*)malloc(2 * (suffixArrayLen + 1) * sizeof(uint32_t));
	if (summaryString == NULL)
	{
		printf("couldn't allocate summaryString, exiting\n");
		exit(1);
	}
	uint32_t* summaryOffsets = (uint32_t*)malloc(2 * (suffixArrayLen + 1) * sizeof(uint32_t));
	if (summaryOffsets == NULL)
	{
		printf("couldn't allocate summaryOffsets, exiting\n");
		exit(1);
	}
	bool* SLTypes = (bool*)malloc(2 * (suffixArrayLen + 1) * sizeof(bool));
	if (SLTypes == NULL)
	{
		printf("couldn't allocate SLTypes, exiting\n");
		exit(1);
	}

	uint32_t maxAlphabetSize;
	if (alphabetSize > suffixArrayLen)
	{
		maxAlphabetSize = alphabetSize;
	}
	else
	{
		maxAlphabetSize = suffixArrayLen + 1;
	}
	uint32_t* bucketSizes = (uint32_t*)malloc(2 * maxAlphabetSize * sizeof(uint32_t));
	if (bucketSizes == NULL)
	{
		printf("couldn't allocate bucketSizes, exiting\n");
		exit(1);
	}
	uint32_t* bucketEnds = (uint32_t*)malloc(2 * maxAlphabetSize * sizeof(uint32_t));
	if (bucketEnds == NULL)
	{
		printf("couldn't allocate bucketEnds, exiting\n");
		exit(1);
	}

	sortSuffixArray(uintString, suffixArrayLen, tempSuffixArray, namesBuffer, summaryString, summaryOffsets, bucketSizes, bucketEnds, SLTypes, alphabetSize);
	memcpy(suffixArray->suffixArray, tempSuffixArray, (suffixArrayLen + 1) * sizeof(uint32_t));

	free(bucketEnds);
	free(bucketSizes);
	free(SLTypes);
	free(summaryOffsets);
	free(summaryString);
	free(tempSuffixArray);
	free(uintString);

	return suffixArray;
}



