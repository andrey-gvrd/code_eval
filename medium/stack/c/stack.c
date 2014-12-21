#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES           (256)
#define MAX_VALUES          (256)
#define MAX_CHARS_IN_NUMBER (16)
#define MAX_LINE_LENGTH     ((MAX_CHARS_IN_NUMBER + 1) * MAX_VALUES)

typedef enum bool_e 
{
    false = 0,
    true
} bool;

static void createNumberMatrix(const char *fileName,
                               int32_t matrix[][MAX_VALUES], uint32_t *columnCnt, uint32_t *rowCnt)
{
    FILE *file = fopen(fileName, "r");
    char line[MAX_LINE_LENGTH];
    *rowCnt = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        uint32_t i = 0;
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

int32_t main(int32_t argc, const char *argv[]) 
{
    int32_t matrix[MAX_LINES][MAX_VALUES] = {0};
    uint32_t columnCnt[MAX_LINES] = {0};
    uint32_t rowCnt = 0;
    bool canPrint = true;
    createNumberMatrix(argv[1], matrix, &columnCnt[0], &rowCnt);
    for (uint32_t i = 0; i < rowCnt; i++) {
        for (int32_t j = columnCnt[i] - 1; j >= 0; j--) {
            if (canPrint) {
                printf("%d ", matrix[i][j - 1]);
                canPrint = false;
            } else {
                canPrint = true;
            }
            //printf("a[%d][%d]: %d\n", i, j - 1, matrix[i][j - 1]);
        }
        printf("\n");
        canPrint = true;
    }
    return 0;
}
