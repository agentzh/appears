#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static char tokens[] = {'G', 'A', 'T', 'C'};

int main (int argc, char* argv[]) {
    float size_in_giga = 0;
    size_t size, i;
    int index;

    if (argc >= 2) {
        size_in_giga = atof(argv[1]);
    } else {
        size_in_giga = 4.8;
    }
    if (size_in_giga <= 0) {
        fprintf(stderr, "ERROR: The input size argument should be "
                "positive.");
        return 1;
    }
    size = size_in_giga * (1024 * 1024 * 1024);

    for (i = 0; i < size; i++) {
        index = rand() % sizeof(tokens);
        putc(tokens[index], stdout);
    }

    return 0;
}

