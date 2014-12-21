#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <ctype.h>

#define MAX_LINES           (16)
#define MAX_CHAR_LENGTH     (3)
#define MAX_CHAR_COUNT      (512)
#define MAX_LINE_LENGTH     (MAX_CHAR_COUNT * MAX_CHAR_LENGTH)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

typedef enum bool_e 
{
    false = 0,
    true
} bool;

static void createNumberMatrix(const char *fileName,
                               char matrix[][MAX_LINE_LENGTH], uint32_t *columnCnt, uint32_t *rowCnt)
{
    FILE *file = fopen(fileName, "r");
    char line[MAX_LINE_LENGTH] = {0};
    *rowCnt = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        columnCnt[*rowCnt] = 0;
        uint32_t i = 0;
        do {
            char charBuff[MAX_CHAR_LENGTH] = {0}; // + 1 for \n
            sprintf(charBuff, "%d", line[i]);
            matrix[*rowCnt][(columnCnt[*rowCnt])++] = atoi(charBuff);
            //printf("matrix[%d][%d]: %d\n", *rowCnt, columnCnt[*rowCnt] - 1, matrix[*rowCnt][columnCnt[*rowCnt] - 1]);
            i++;
        } while (line[i - 1] != '\n');
        (*rowCnt)++;
    }
}

static uint32_t mostFrequentNumber(const char array[], const uint32_t arrayLength)
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
        if (temp[i] < minValue) {
            minValue = temp[i];
        }
    }
    return minValue;
}

int32_t main(int32_t argc, const char *argv[]) 
{
    const int32_t codeDifference = 5;
    char matrix[MAX_LINES][MAX_LINE_LENGTH];
    for (uint32_t i = 0; i < MAX_LINES; i++) {
        for (uint32_t j = 0; j < MAX_LINE_LENGTH; j++) {
            matrix[i][j] = 0;
        }
    }
    uint32_t columnCnt[MAX_LINES] = {0};
    uint32_t rowCnt = 0;
    createNumberMatrix(argv[1], matrix, &columnCnt[0], &rowCnt);
    for (uint32_t i = 0; i < rowCnt; i++) {
        //printf("columnCnt: %u\n", columnCnt[i]);
        //printf("most frequent character: %u\n", mostFrequentNumber(matrix[i], columnCnt[i]));
        FILE *file = fopen("enc_text.txt", "w");
        for (uint32_t j = 0; j < columnCnt[i]; j++) {
            matrix[i][j] += codeDifference;
            fprintf(file, "%u ", matrix[i][j]);
            //printf("%c", matrix[i][j]);
        }
        //printf("\n");
    }
    return 0;
}
