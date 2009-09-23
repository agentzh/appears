#define DDEBUG 0
#include "ddebug.h"

#include <cstdio>
#include <cmath>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <bitset>

/* here we allow overriding on the g++ command line */
#ifndef APPEARS_SEQSIZE
#define APPEARS_SEQSIZE 15L /* sequence string size */
#endif

/* 4^APPEARS_SEQSIZE */
const size_t TABLESIZE = 1L << (2 * APPEARS_SEQSIZE);
const int BITS_PER_CHAR = 2;

typedef std::bitset<APPEARS_SEQSIZE*2> index_t;

enum {
    G = 0, A, T, C
};

std::bitset<TABLESIZE> BitTable;

/* forward declarations */
static bool parse_seq_file (const char* fname);
static void search_missing_combinations ();
void index2str(index_t& index, char* buf);

int main (int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No input file specified.\n");
        return 1;
    }

    if ( ! parse_seq_file(argv[1]) ) {
        return 1;
    }

    printf("INFO: Using a bit array of length %lu"
            " (sequence length: %ld).\n",
            (unsigned long)TABLESIZE,
            APPEARS_SEQSIZE);
    //exit(1);

    printf("INFO: Searching missing combinations of sequences...\n");
    search_missing_combinations();

    //system("sleep 360");

    return 0;
}

/* parse_seq_file: read in the sequence stream and set
 * the global BigTable variable accordingly. */
static bool parse_seq_file (const char* fname) {
    FILE *infile;
    char c;
    unsigned long lineno = 1L;
    index_t index;
    unsigned long chars_seen = 0L;

    if (NULL == (infile = fopen(fname, "r"))) {
        fprintf(stderr,
                "ERROR: Failed to open input file %s for reading: "
                "%s\n",
                fname, strerror(errno));
        return false;
    }

    for (;;) {
        bool found_char = false;

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
                index <<= BITS_PER_CHAR;
                index |= G;
                found_char = true;
                break;
            case 'A':
                index <<= BITS_PER_CHAR;
                index |= A;
                found_char = true;
                break;
            case 'T':
                index <<= BITS_PER_CHAR;
                index |= T;
                found_char = true;
                break;
            case 'C':
                index <<= BITS_PER_CHAR;
                index |= C;
                found_char = true;
                break;
            default:
                fprintf(stderr, "ERROR: File %s: line %llu: "
                        "Invalid character found: '%c' (0x%x)\n",
                        fname, lineno, c, c);
                return false;
                break;
        }
        if (found_char) {
            if (chars_seen < APPEARS_SEQSIZE) {
                chars_seen++;
            }
            if (chars_seen >= APPEARS_SEQSIZE) {
                /* register this subsequence combination */
                DD("setting the %luth bit", index.to_ulong());
                BitTable.set(index.to_ulong());
            }
        }
    }
    fclose(infile);
    return true;
}

void index2str(unsigned long index, char* buf) {
    for (unsigned long i = 0; i < APPEARS_SEQSIZE; i++) {
        int char_val = (index >> (i * 2)) & 3;
        switch (char_val) {
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
            index2str(i, buf);
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

