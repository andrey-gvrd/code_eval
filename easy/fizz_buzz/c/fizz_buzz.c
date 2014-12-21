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

void main(int32_t argc, const char *argv[]) 
{
    uint32_t numMatrix[MAX_LINES][NUM_COLS];
    uint32_t numLines = 0;
    uint32_t i, j;
    FILE *file = fopen(argv[1], "r");
    createNumberMatrix(file, numMatrix, &numLines);
    for (i = 1; i <= numLines; i++) {
        uint32_t x = numMatrix[i - 1][0];
        uint32_t y = numMatrix[i - 1][1];
        uint32_t n = numMatrix[i - 1][2];
        bool f = false;
        bool b = false;
        for (j = 1; j <= n; j++) {
            if ((j % x) == 0) f = true;
            if ((j % y) == 0) b = true;
            if (f && b) printf("FB ");
            else if (f) printf("F ");
            else if (b) printf("B ");
            else printf("%d ", j);
            f = false;
            b = false;
        }
        printf("\n");
    }
}
