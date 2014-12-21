#include <stdio.h>  // printf, sprintf
#include <stdint.h>
#include <stdlib.h> // atoi
#include <ctype.h>  // isdigit
#include <math.h>   // sqrt, pow

#define MAX_POINTS          (10000 + 1)
#define MAX_SETS            (128)
#define MAX_NUMBER          (40000)

int32_t main(int32_t argc, const char *argv[]) 
{
    FILE *file = fopen("rnd_in.txt", "w");
    for (uint64_t i = 0; i < MAX_SETS; i++) {
        uint32_t nPoints = rand() % MAX_POINTS;
        fprintf(file, "%u\n", nPoints);
        for (uint32_t j = 0; j < nPoints; j++) {
            uint32_t x, y;
            x = rand() % MAX_NUMBER;
            y = rand() % MAX_NUMBER;
            fprintf(file, "%u %u\n", x, y);
        }
        fprintf(file, "0\n");
    }
    return 0;
}
