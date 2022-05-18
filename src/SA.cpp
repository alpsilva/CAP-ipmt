#include "SA.h"


struct SuffixArray* initSuffixArray(uint8_t* text, size_t textLen)
{
	struct SuffixArray* sa = (struct SuffixArray*)malloc(sizeof(struct SuffixArray));
	sa->text = text;
	sa->length = textLen;
	sa->suffixArray = (uint32_t*)malloc(textLen * sizeof(uint32_t));
	return sa;
}
