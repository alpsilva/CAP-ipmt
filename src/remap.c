#include "remap.h"

void createRemapTable(struct StringRemapTable* stringRemapTable, const uint8_t* string) 
{
	stringRemapTable->alphabetSize = 1; 
	memset(stringRemapTable->charTable, -1, sizeof(stringRemapTable->charTable));
	memset(stringRemapTable->remappedCharTable, -1, sizeof(stringRemapTable->remappedCharTable));

	stringRemapTable->charTable[0] = 0;
	stringRemapTable->remappedCharTable[0] = 0;

	const size_t inputStringLen = strlen((char*)string) + 1;

	for (size_t i = 0; i < inputStringLen; i++)
	{
		if(stringRemapTable->charTable[string[i]] == -1)
		{
			stringRemapTable->alphabetSize++;
			stringRemapTable->charTable[string[i]] = 1;
		}
	}

	uint8_t charIndex = 0;
	for (size_t i = 0; i < 256; i++) 
	{
		if (stringRemapTable->charTable[i] != -1) 
		{
			stringRemapTable->charTable[i] = charIndex;
			stringRemapTable->remappedCharTable[charIndex] = i;
			charIndex++;
		}
	}
}

void remapString(uint8_t* remappedString, const uint8_t* inputString, struct StringRemapTable* stringRemapTable)
{
	const size_t inputStringLen = strlen((char*)inputString) + 1;
	for(int i = 0; i < inputStringLen; i++)
	{
		remappedString[i] = stringRemapTable->charTable[inputString[i]];
	}
}