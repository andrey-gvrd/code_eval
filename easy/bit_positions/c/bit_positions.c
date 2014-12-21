#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1024
#define NUM_COLS 3

typedef enum bool_e 
{
    false = 0,
    true
} bool;

void createNumberMatrix(FILE *file, uint32_t numMatrix[][NUM_COLS], uint32_t *rowIndex)
{
    char line[MAX_LINES];
    *rowIndex = 0;
    while (fgets(line, MAX_LINES, file)) {
        uint32_t i = 0;
        uint32_t columnIndex = 0;
        char numbBuff[NUM_COLS] = {0};
        for (columnIndex = 0; columnIndex < NUM_COLS; columnIndex++) {
            uint32_t j = 0;
            while (isdigit(line[i])) {
                numbBuff[j++] = line[i++];
            }
            i++;
            numMatrix[*rowIndex][columnIndex] = atoi(numbBuff);
            memset(&numbBuff[0], 0x00, NUM_COLS);
        }
        (*rowIndex)++;
    }
}

int32_t main(int32_t argc, const char *argv[]) 
{
    uint32_t numMatrix[MAX_LINES][NUM_COLS];
    uint32_t numLines = 0;
    uint32_t i, j;
    FILE *file = fopen(argv[1], "r");
    createNumberMatrix(file, numMatrix, &numLines);
    for (i = 1; i <= numLines; i++) {
        uint32_t n = numMatrix[i - 1][0];
        uint32_t p1 = numMatrix[i - 1][1];
        uint32_t p2 = numMatrix[i - 1][2];
        uint32_t p1_mask = (1 << (p1 - 1));
        uint32_t p2_mask = (1 << (p2 - 1));
        /*
        printf("n: %d, p1: %d, p2: %d, p1 mask: %d, p2 mask: %d\n", n, p1, p2, p1_mask, p2_mask);
        printf("n masked p1: %d, n masked p2: %d\n", p1_mask & n, p2_mask & n);
        printf("p1 moved: %d, p2 moved: %d\n",
            (p1_mask & n) >> (p1 - 1), (p2_mask & n) >> (p2 - 1));
        */
        if ((((p1_mask & n) >> (p1 - 1))) == (((p2_mask & n) >> (p2 - 1))))
            printf("true\n");
        else
            printf("false\n");
        //printf("\n");
    }
    return 0;
}
