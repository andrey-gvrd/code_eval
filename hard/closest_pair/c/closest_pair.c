#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // atoi
#include <string.h>
#include <ctype.h>

#define MAX_SETS            (8)
#define MAX_POINTS          (10000)
#define MAX_LINE_LENGTH     (2 * 5 + 2) // two 40000 points, a space and \n

typedef enum bool_e 
{
    false = 0,
    true
} bool;

typedef struct point_s
{
    uint32_t x;
    uint32_t y;
} point_t;

typedef struct pointSet_s
{
    point_t point[MAX_POINTS];
    uint32_t len;
} pointSet_t;

static uint32_t getNum(const char *str, uint32_t *pos)
{
    uint32_t dstPos = 0;
    char numbBuff[5] = {0};
    while (isdigit(str[*pos])) {
        numbBuff[dstPos++] = str[(*pos)++];
    }
    (*pos)++;
    return atoi(numbBuff);
}

int32_t main(int32_t argc, const char *argv[]) 
{
    pointSet_t psArr[MAX_SETS];
    FILE *file = fopen(argv[1], "r");
    char line[MAX_LINE_LENGTH];
    bool readingPoints = false;
    uint32_t psArrCnt = 0;
    uint32_t pointCnt = 0;
    /* Parse input file */
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (readingPoints) {
            if (pointCnt <= psArr[psArrCnt].len - 1) {
                uint32_t pos = 0;
                psArr[psArrCnt].point[pointCnt].x = getNum(&line[0], &pos);
                psArr[psArrCnt].point[pointCnt].y = getNum(&line[0], &pos);
                printf("[%d][%d](%d) - [%d, %d]\n", 
                    psArrCnt, pointCnt, psArr[psArrCnt].len,
                    psArr[psArrCnt].point[pointCnt].x, 
                    psArr[psArrCnt].point[pointCnt].y);
                pointCnt++;
            } else {
                readingPoints = false;
                pointCnt = 0;
                psArrCnt++;
                printf("\n");
            }
        } else {
            uint32_t pos = 0;
            psArr[psArrCnt].len = getNum(&line[0], &pos);
            readingPoints = true;
        }
    }
    /* Work with parsed data */
    for (uint32_t i = 0; i < psArrCnt; i++) {
        printf("%d\n", i);
    }
    return 0;
}
