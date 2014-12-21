#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <ctype.h>

#define MAX_LINES           (8)
#define MAX_VALUES          (512)
#define MAX_CHARS_IN_NUMBER (3)
#define MAX_LINE_LENGTH     ((MAX_CHARS_IN_NUMBER + 1) * MAX_VALUES)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define NUM_ROWS(m) (sizeof(m) / sizeof(m[0]))
#define NUM_COLS(m) (sizeof(m[0]) / sizeof(m[0][0]))

typedef enum bool_e 
{
    false = 0,
    true
} bool;

/**
 * Algorithm:
 * Find out which number represents a space, find repeating number sequences 
 * of desired length which are surrounded by spaces (hopefully there's only one)
 * pair, figure out the value with which the sequence is encoded, decode the
 * sequence and print it out.
*/

/* Parsing function */
static void createNumberMatrix(const char *fileName,
                               uint32_t matrix[][MAX_VALUES], uint32_t *columnCnt, uint32_t *rowCnt,
                               uint32_t *seqLen, char *knownChar)
{
    FILE *file = fopen(fileName, "r");
    char line[MAX_LINE_LENGTH];
    char *p;
    *rowCnt = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        /* Find the first value - length of the repeating sequence */
        *seqLen = atoi(line);
        /* Find the second value - a known charachter */
        p = strstr(line, "|");
        uint32_t i = p - &line[0] + 2;
        *knownChar = line[i];
        /* Find the third value - sequence of integers */
        p = strrchr(line, '|');
        i = p - &line[0] + 2;
        char numbBuff[MAX_CHARS_IN_NUMBER] = {0};
        columnCnt[*rowCnt] = 0;
        do {
            uint32_t j = 0;
            while (isdigit(line[i]) ||
                          (line[i]) == '-') {
                numbBuff[j++] = line[i++];
            }
            if (j > 0) {
                matrix[*rowCnt][(columnCnt[*rowCnt])++] = atoi(numbBuff);
                /*
                printf("a[%d][%d]: %d\n", *rowCnt, (columnCnt[*rowCnt]) - 1, 
                    matrix[*rowCnt][(columnCnt[*rowCnt]) - 1]);
                */
                memset(&numbBuff[0], 0x00, MAX_CHARS_IN_NUMBER);
            }
        } while (line[i++ - 1] != '\n');
        (*rowCnt)++;
        //printf("\n");
    }
}

static uint32_t findSpace(const uint32_t array[], const uint32_t arrayLength)
{
    uint32_t frequencyCnt[MAX_LINE_LENGTH] = {0};
    uint32_t temp[5] = {0};
    for (uint32_t i = 0; i < arrayLength; i++) {
        frequencyCnt[(unsigned)array[i]]++;
        //printf("frequencyCnt[%d]: %u\n", array[i], frequencyCnt[(unsigned)array[i]]);
    }
    for (uint32_t i = 0; i < ARRAY_SIZE(temp); i++) {
        uint32_t max = 0;
        for (uint32_t j = 0; j < arrayLength; j++) {
            if (frequencyCnt[j] > max) {
                max = j;
            }
        }
        frequencyCnt[max] = 0;
        temp[i] = max;
    }
    uint32_t minValue = 255;
    for (uint32_t i = 0; i < ARRAY_SIZE(temp); i++) {
        //printf("temp[%u]: %u\n", i, temp[i]);
        if (temp[i] < minValue) {
            minValue = temp[i];
        }
    }
    return minValue;
}

static void matrixInit(uint32_t matrix[][MAX_VALUES], 
                       const uint32_t rowCnt, const uint32_t columnCnt)
{
    for (uint32_t i = 0; i < rowCnt; i++) {
        for (uint32_t j = 0; j < columnCnt; j++) {
            //if (rowCnt == 32) { printf("m[%d][%d]: %d\n", i, j, matrix[i][j]); }
            matrix[i][j] = 0;
        }
    }
}

int32_t main(int32_t argc, const char *argv[]) 
{
    uint32_t matrix[MAX_LINES][MAX_VALUES];
    matrixInit(matrix, NUM_ROWS(matrix), NUM_COLS(matrix));
    uint32_t columnCnt[MAX_LINES] = {0};
    uint32_t rowCnt = 0;
    uint32_t seqLen = 0;
    char knownChar = 0;
    /* Extract the integer sequence into an array */
    createNumberMatrix(argv[1], matrix, &columnCnt[0], &rowCnt,
                       &seqLen, &knownChar);
    //printf("seqLen: %d, knownChar: %c\n", seqLen, knownChar);
    /* Go through every input sequence */
    for (uint32_t i = 0; i < rowCnt; i++) {
        uint32_t spaceCode = findSpace(matrix[i], 255);
        //printf("spaceCode: %d\n", spaceCode);
        uint32_t potentSeqCnt = 0;
        uint32_t potentSeq[16][seqLen];
        matrixInit(potentSeq, NUM_ROWS(potentSeq), NUM_COLS(potentSeq));
        for (uint32_t j = 0; j < columnCnt[i] - seqLen; j++) {
            if ((matrix[i][j] == spaceCode) && 
                (matrix[i][j + seqLen + 1] == spaceCode)) {
                //printf("potentSeq index: %d, seq: ", j + 1);
                /* memcpy doesn't copy the whole sequence for some reason */
                //memcpy(&potentSeq[potentSeqCnt][0], &matrix[i][j + 1], seqLen);
                for (uint32_t k = 0; k < seqLen; k++) {
                    potentSeq[potentSeqCnt][k] = matrix[i][j + 1 + k];
                    //printf("%d ", potentSeq[potentSeqCnt][k]);
                }
                //printf("\n");
                potentSeqCnt++;
            }
        }
        uint32_t codeDiff = 0;
        for (uint32_t j = 0; j < potentSeqCnt - 1; j++) {
            if (!memcmp(&potentSeq[j][0], &potentSeq[j + 1][0], seqLen)) {
                codeDiff = potentSeq[j][seqLen - 1] - knownChar;
                //printf("codeDiff: %d\n", codeDiff);
            }
        }
        for (uint32_t j = 0; j < columnCnt[i]; j++) {
            printf("%c", matrix[i][j] - codeDiff);
        }
        printf("\n");
    }
    return 0;
}
