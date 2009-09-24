#define DDEBUG 0

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include "ddebug.h"

#include <cstdio>
#include <cmath>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <bitset>

/* here we allow overriding on the g++/cl command line */
#ifndef APPEARS_SEQSIZE
#define APPEARS_SEQSIZE 15L /* sequence string size */
#endif

/* 4^APPEARS_SEQSIZE */
const size_t TABLESIZE = 1L << (2 * APPEARS_SEQSIZE);

/* We make this naming convention that a "token" is a nucleotide */
const int BITS_PER_TOKEN = 2;
const int TOKEN_MASK = 3; /* "11" in binary */

typedef std::bitset<APPEARS_SEQSIZE * BITS_PER_TOKEN> index_t;

enum {
    G = 0, A, T, C
};

std::bitset<TABLESIZE> BitTable;

/* forward declarations */
static bool parse_seq_file (const char* fname);
static void search_missing_combinations ();
static void index_to_tokens (unsigned long index, char* buf);

int main (int argc, char* argv[]) {
    const char* fname;
    if (argc < 2) {
        fprintf(stderr, "ERROR: No input file specified.\n");
        return 1;
    }
    fname = argv[1];

    fprintf(stderr, "INFO: Reading file %s...\n", fname);

    if ( ! parse_seq_file(fname) ) {
        return 1;
    }

    fprintf(stderr,
            "INFO: Using a bit array of length %lu"
            " (sequence length: %ld).\n",
            (unsigned long)TABLESIZE,
            APPEARS_SEQSIZE);

    fprintf(stderr,
            "INFO: Searching missing combinations of sequences...\n");
    search_missing_combinations();

    return 0;
}

/* parse_seq_file: read in the sequence stream and set
 * the global BigTable variable accordingly. */
static bool parse_seq_file (const char* fname) {
    FILE* infile;
    char c;
    unsigned long lineno = 1L;
    index_t index;
    unsigned long tokens_seen = 0L;

    if (NULL == (infile = fopen(fname, "r"))) {
        fprintf(stderr,
                "ERROR: Failed to open input file %s for reading: "
                "%s\n",
                fname, strerror(errno));
        return false;
    }

    for (;;) {
        bool found_token = false;

        c = fgetc(infile);
        if (c == EOF) {
            if (ferror(infile)) {
                fprintf(stderr, "ERROR: Failed to read from file %s:"
                        "%s\n",
                        fname, strerror(errno));
                return false;
            }
            break;
        }

        switch (c) {
            case '\n':
                lineno++;
            case ' ':
            case '\t':
            case '\r':
                /* ignored spaces */
                break;
            case 'G':
                index <<= BITS_PER_TOKEN;
                index |= G;
                found_token = true;
                break;
            case 'A':
                index <<= BITS_PER_TOKEN;
                index |= A;
                found_token = true;
                break;
            case 'T':
                index <<= BITS_PER_TOKEN;
                index |= T;
                found_token = true;
                break;
            case 'C':
                index <<= BITS_PER_TOKEN;
                index |= C;
                found_token = true;
                break;
            default:
                fprintf(stderr, "ERROR: File %s: line %lu: "
                        "Invalid character found: '%c' (0x%x)\n",
                        fname, lineno, c, c);
                return false;
                break;
        }
        if (found_token) {
            if (tokens_seen < APPEARS_SEQSIZE) {
                tokens_seen++;
            }
            if (tokens_seen >= APPEARS_SEQSIZE) {
                /* register this subsequence combination */
                unsigned long index_ulong = index.to_ulong();
                DD("setting the %luth bit", index_ulong);
                BitTable.set(index_ulong, 1);
            }
        }
    }
    fclose(infile);
    return true;
}

/* index_to_tokens: decode the binary index numerals to G/A/T/C
 * token sequences */
void index_to_tokens (unsigned long index, char* buf) {
    for (unsigned long i = 0; i < APPEARS_SEQSIZE; i++) {
        int encoded_token =
            (index >> (i * BITS_PER_TOKEN)) & TOKEN_MASK;
        switch (encoded_token) {
            case G:
                buf[APPEARS_SEQSIZE - i - 1] = 'G';
                break;
            case A:
                buf[APPEARS_SEQSIZE - i - 1] = 'A';
                break;
            case T:
                buf[APPEARS_SEQSIZE - i - 1] = 'T';
                break;
            case C:
                buf[APPEARS_SEQSIZE - i - 1] = 'C';
                break;
            default:
                break;
        }
    }
    buf[APPEARS_SEQSIZE] = '\0';
}

/* search_missing_combinations: traverse the BitTable
 * to find 0 bits and print out the corresponding
 * character sequences */
void search_missing_combinations () {
    unsigned long missing_combs = 0;
    char buf[APPEARS_SEQSIZE + 1];
    for (unsigned long i = 0; i < TABLESIZE; i++) {
        if (!BitTable.test(i)) {
            index_to_tokens(i, buf);
            printf("  %s (index %lu)\n", buf, i);
            missing_combs++;
        }
    }
    if (missing_combs) {
        printf("For total %lu missing combinations found.\n",
                missing_combs);
    } else {
        printf("All combinations have appeared.\n");
    }
}

