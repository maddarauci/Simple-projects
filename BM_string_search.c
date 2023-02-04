#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ALPHABET_LEN 256
#define max(a, b) ((a < b) ? b : a)	// If Condition is true ? then value X : otherwise value Y


// BAD CHARACTER RULE.
//

void make_delta1(ptrdiff_t *delta1, uint8_t *pat, size_t patlen) {
	for (int i = 0; i < ALPHABET_LEN; i++) {
		delta1[i] = patlen;
	}
	for (int i = 0; i < patlen; i++) {
		delta1[pat[i]] = patlen-1 - i;
	}
}

// true if the suffix of word starting from word[pos] is a prefix of word
bool is_prefix(uint8_t *word, size_t wordlen, ptrdiff_t pos) {
	int suffixlen = wordlen - pos;

	/*
	could also use the strcmp() library function here
	return ! strcmp(word, &word[pos], suffixlen);
	return !strncmp(word, &word[pos], suffixlen);
	*/
	for (int i = 0; i < suffixlen; i++) {
		if (word[i] != word[pos+1]) {
			return false;
		}
	}
	return true;

}


int main(void)
{

	return 0;
}