#include "SA.h"


struct suffixArray* initSuffixArray(uint8_t* text, size_t textLen)
{
	struct suffixArray* sa = (struct suffixArray*)malloc(sizeof(struct suffixArray));
	sa->text = text;
	sa->length = textLen;
	sa->suffixArray = (uint32_t*)malloc(textLen * sizeof(uint32_t));
	return sa;
}
