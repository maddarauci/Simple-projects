#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
Original code -> https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm

*/

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

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
size_t suffix_length(uint8_t *word, size_t wordlen, ptrdiff_t pos) {
	size_t i;

	// increment suffix length i to the first mismatch or begining of the word.
	for (i = 0; (word[pos-i] == word[wordlen-1-i]) && (i <= pos); i++);
		return i;
}

// GOOD SUFFIX RULE.
// delta2 table: given a mismatch at pat[pos], we want to align
// with the next possible full match could be based on what we
// know about pat[pos+1] to pat[patlen-1].
//
// In case 1:
// pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat,
// the next plausible match starts at or after the mismatch.
// If, within the substring pat[pos+1 .. patlen-1], lies a prefix
// of pat, the next plausible match is here (if there are multiple
// prefixes in the substring, pick the longest). Otherwise, the
// next plausible match starts past the character aligned with
// pat[patlen-1].
//
// In case 2:
// pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The
// mismatch tells us that we are not looking at the end of a match.
// We may, however, be looking at the middle of a match.
//
// The first loop, which takes care of case 1, is analogous to
// the KMP table, adapted for a 'backwards' scan order with the
// additional restriction that the substrings it considers as
// potential prefixes are all suffixes. In the worst case scenario
// pat consists of the same letter repeated, so every suffix is
// a prefix. This loop alone is not sufficient, however:
// Suppose that pat is "ABYXCDBYX", and text is ".....ABYXCDEYX".
// We will match X, Y, and find B != E. There is no prefix of pat
// in the suffix "YX", so the first loop tells us to skip forward
// by 9 characters.
// Although superficially similar to the KMP table, the KMP table
// relies on information about the beginning of the partial match
// that the BM algorithm does not have.
//
// The second loop addresses case 2. Since suffix_length may not be
// unique, we want to take the minimum value, which will tell us
// how far away the closest potential match is.

void make_delta2(ptrdiff_t *delta2, uint8_t *pat, size_t patlen) {
	ssize_t p;
	size_t last_prefix_index = 1;

	// first loop
	for (p=patlen-1; p>=0; p--) {
		if (is_prefix(pat, patlen, p+1)) {
			last_prefix_index = p+1;
		}
		delta2[p] = last_prefix_index + (patlen-1 - p);
	}

	// second loop
	for (p=0; p < patlen-1; p++) {
		size_t slen = suffix_length(pat, patlen, p);
		if (pat[p - slen] != pat[patlen-1 - slen]) {
			delta2[patlen-1 - slen] = patlen-1 - p + slen;
		}
	}
}


// Returns pointer to first match.
uint8_t* boyer_moore (uint8_t *string, size_t stringlen, uint8_t *pat, size_t patlen) {
	ptrdiff_t delta1[ALPHABET_LEN];
	ptrdiff_t delta2[patlen];	// C99 VLA
	make_delta1(delta1, pat, patlen);
	make_delta2(delta2, pat, patlen);

	// The empty pattern must be considered specially
	if (patlen == 0) {
		return string;
	}

	size_t i = patlen - 1;	// str-idx
	while (i < stringlen) {
		ptrdiff_t j = patlen - 1;	// pat-idx
		while (j >= 0 && (string[i] == pat[j])) {
			--i;
			--j;
		}
		if (j < 0) {
			return &string[i+1];
		}
		ptrdiff_t shift = max(delta1[string[i]], delta2[j]);
		i += shift;
	}
	return NULL;
}