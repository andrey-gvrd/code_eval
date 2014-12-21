#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <ctype.h>

#define MAX_LINES           (16)
#define MAX_VALUES          (512)
#define MAX_CHARS_IN_NUMBER (3)
#define MAX_LINE_LENGTH     ((MAX_CHARS_IN_NUMBER + 1) * MAX_VALUES)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

typedef enum bool_e 
{
    false = 0,
    true
} bool;

static void createNumberMatrix(const char *fileName,
                               uint32_t matrix[][MAX_VALUES], uint32_t *columnCnt, uint32_t *rowCnt,
                               uint32_t *sequenceLength, char *knownChar)
{
    FILE *file = fopen(fileName, "r");
    char line[MAX_LINE_LENGTH];
    char *p;
    *rowCnt = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        /* Find the first value - length of the repeating sequence */
        *sequenceLength = atoi(line);
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

static uint32_t mostFrequentNumber(const uint32_t array[], const uint32_t arrayLength)
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

int32_t main(int32_t argc, const char *argv[]) 
{
    uint32_t matrix[MAX_LINES][MAX_VALUES];
    for (uint32_t i = 0; i < MAX_LINES; i++) {
        for (uint32_t j = 0; j < MAX_VALUES; j++) {
            matrix[i][j] = 0;
        }
    }
    uint32_t columnCnt[MAX_LINES] = {0};
    uint32_t rowCnt = 0;
    uint32_t sequenceLength = 0;
    char knownChar = 0;
    /* Extract the integer sequence into an array */
    createNumberMatrix(argv[1], matrix, &columnCnt[0], &rowCnt,
                       &sequenceLength, &knownChar);
    //printf("length: %d, charachter: %c\n", sequenceLength, knownChar);
    /* Go through every input sequence */
    for (uint32_t i = 0; i < rowCnt; i++) {
        int32_t codeDifference;
        /* Find the most freqent number in the number sequence*/
        uint32_t spaceCode = mostFrequentNumber(matrix[i], 255);
        //printf("space is encoded as: %d\n", spaceCode);
        /* Through every number sequence */
        for (uint32_t j = 0; j < columnCnt[i] - 2 * sequenceLength + 1; j++) {
            bool spaceFound = false;
            uint32_t sequence[sequenceLength];
            //printf("sequence: ");
            /* Through every sequence of constant length */
            for (uint32_t k = 0; k < sequenceLength; k++) {
                if (matrix[i][j + k] == spaceCode) {
                    spaceFound = true;
                }
                sequence[k] = matrix[i][j + k];
                //printf("%d ", sequence[k]);
            }
            //printf("\n");
            if (spaceFound) {
                continue;
            }
            /* Try to find that sequence in the rest of the number sequence */
            for (uint32_t m = j + sequenceLength; m < columnCnt[i] - sequenceLength + 1; m++) {
                uint32_t n = 0;
                uint32_t overlap = 0;
                while (matrix[i][m++] == sequence[n++]) {
                    if (matrix[i][m - 1] == spaceCode) {
                        //printf("space found, breaking\n");
                        break;
                    }
                    //printf("%d == %d\n", matrix[i][m - 1], sequence[n - 1]);
                    //printf("overlap: %d\n", overlap);
                    overlap++;
                }
                if (overlap >= sequenceLength) {
                    printf("sequence originally starting at %d found again at: %d\n", j, m - sequenceLength);
                    codeDifference = sequence[sequenceLength - 1] - knownChar;
                    //printf("codeDifference: %d\n", codeDifference);
                } else {
                    m--;
                    n--;
                    //printf("%d != %d\n", matrix[i][m], sequence[n]);
                    m -= overlap;   // reset the index to previous state
                }
            }
        }
        for (uint32_t j = 0; j < columnCnt[i]; j++) {
            matrix[i][j] -= codeDifference;
            printf("%c", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}
