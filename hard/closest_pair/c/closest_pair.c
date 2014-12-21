#include <stdio.h>  // printf, sprintf
#include <stdint.h>
#include <stdlib.h> // atoi
#include <ctype.h>  // isdigit
#include <math.h>   // sqrt, pow

#define MAX_POINTS          (10000 + 1)
#define MAX_LINE_LENGTH     (16) // two 40000 points, a space and \n
#define MAX_DIST            (10000.0)

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

static uint64_t getNum(const char *str, uint32_t *pos)
{
    uint32_t dstPos = 0;
    char numbBuff[10] = {0};
    while (isdigit((int32_t)str[*pos])) {
        numbBuff[dstPos++] = str[(*pos)++];
    }
    (*pos)++;
    return atoi(numbBuff);
}

static float getDistance(point_t p1, point_t p2)
{
    point_t delta = { .x = abs(p1.x - p2.x), .y = abs(p1.y - p2.y)};
    return sqrt(pow(delta.x, 2) + pow(delta.y, 2));
}

int32_t main(int32_t argc, const char *argv[]) 
{
    point_t pArr[MAX_POINTS];
    FILE *file = fopen(argv[1], "r");
    char line[MAX_LINE_LENGTH];
    bool readingPoints = false;
    uint32_t pointCnt = 0;
    uint32_t len = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        if (readingPoints) {
            if (pointCnt <= len - 1) {
                uint32_t pos = 0;
                pArr[pointCnt].x = getNum(&line[0], &pos);
                pArr[pointCnt].y = getNum(&line[0], &pos);
                /*
                printf("[%d](%d) - [%d, %d]\n", 
                    pointCnt, len,
                    pArr[pointCnt].x, 
                    pArr[pointCnt].y);
                */
                pointCnt++;
            } else {
                float min = MAX_DIST;
                for (uint32_t j = 0; j < len - 1; j++) {
                    for (uint32_t k = j + 1; k < len; k++) {
                        float dist = getDistance(pArr[j], 
                                                 pArr[k]);
                        //printf("[%d]-[%d]: %.4f\n", j, k, dist);
                        if (dist < min) {
                            min = dist;
                        }
                    }
                }
                if (min < MAX_DIST) {
                    printf("%.4f\n", min);
                } else {
                    printf("INFINITY\n");
                }
                readingPoints = false;
                pointCnt = 0;
            }
        } else {
            for (uint32_t i = 0; i < MAX_POINTS; i++) {
                pArr[i].x = 0;
                pArr[i].y = 0;
            }
            uint32_t pos = 0;
            len = getNum(&line[0], &pos);
            readingPoints = true;
        }
    }
    return 0;
}
