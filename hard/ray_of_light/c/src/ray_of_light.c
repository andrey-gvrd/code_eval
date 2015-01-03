#include <stdio.h>  // printf, sprintf
#include <stdint.h> // uintX_t
#include <stdlib.h> // atoi
#include <ctype.h>  // isdigit

#define ROOM_DIMENSIONS     (10)

typedef enum {
    false = 0,
    true = !false
} bool;

typedef enum {
    ray225,
    ray45,
    space,
    column,
    prism,
    wall
} Elements_t;

typedef enum {
    left,
    top,
    right,
    bottom,
    no
} EntrySide_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point_t;

static void clearArray(char array[][ROOM_DIMENSIONS])
{
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            array[i][j] = 0x00;
        }
    }
}

static void printArray(char array[][ROOM_DIMENSIONS])
{
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            printf("%c", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

static Elements_t identify(char c)
{
    Elements_t elementType = space;
    switch (c) {
        case '#': {
            elementType = wall;
            break;
        }
        case 'o': {
            elementType = column;
            break;
        }
        case '*': {
            elementType = prism;
            break;
        }
        case '\\': {
            elementType = ray225;
            break;
        }
        case '/': {
            elementType = ray45;
            break;
        }
    }
    return elementType;
}

static EntrySide_t getRayEntrySide(Point_t point)
{
    EntrySide_t entrySide = no;
    if (point.x == 0) {
        entrySide = left;
    } else if (point.x == ROOM_DIMENSIONS - 1) {
        entrySide = right;
    } else if (point.y == 0) {
        entrySide = top;
    } else if (point.y == ROOM_DIMENSIONS - 1) {
        entrySide = bottom;
    }
    return entrySide;
}

static Point_t nextCoordinates(Point_t point, Elements_t rayType)
{
    Point_t nextPoint = { .x = 0, .y = 0 };
    EntrySide_t entrySide = getRayEntrySide(point);
    switch (entrySide) {
        case left: {
            if (rayType == ray45) {
                nextPoint.x = point.x + 1;
                nextPoint.y = point.y - 1;
            } else if (rayType == ray225) {
                nextPoint.x = point.x + 1;
                nextPoint.y = point.y + 1;
            }
            break;
        }
        case top: {
            if (rayType == ray45) {
                nextPoint.x = point.x - 1;
                nextPoint.y = point.y + 1;
            } else if (rayType == ray225) {
                nextPoint.x = point.x + 1;
                nextPoint.y = point.y + 1;
            }
            break;
        }
        case right: {
            if (rayType == ray45) {
                nextPoint.x = point.x - 1;
                nextPoint.y = point.y + 1;
            } else if (rayType == ray225) {
                nextPoint.x = point.x - 1;
                nextPoint.y = point.y - 1;
            }
            break;
        }
        case bottom: {
            if (rayType == ray45) {
                nextPoint.x = point.x + 1;
                nextPoint.y = point.y - 1;
            } else if (rayType == ray225) {
                nextPoint.x = point.x - 1;
                nextPoint.y = point.y - 1;
            }
            break;
        }
    }
    return nextPoint;
}

int32_t main(int32_t argc, const char *argv[]) 
{   
    char room[ROOM_DIMENSIONS][ROOM_DIMENSIONS];
    FILE *file = fopen(argv[1], "r");
    char line[ROOM_DIMENSIONS];
    uint8_t lineCnt = 0;
    while (fgets(line, ROOM_DIMENSIONS + 1, file)) {
        if (lineCnt == ROOM_DIMENSIONS) {
            printArray(room);

            /* Find first ray */
            Point_t entryPoint = { .x = 0, .y = 0 };
            for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
                for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
                    if ((identify(room[i][j]) == ray45) ||
                        (identify(room[i][j]) == ray225)) {
                        entryPoint.x = i;
                        entryPoint.y = j;
                    }
                }
            }
            Point_t nextPoint = nextCoordinates(entryPoint, 
                                                identify(room[entryPoint.x][entryPoint.y]));
            printf("Entry coodrinates: [%d][%d]\n", entryPoint.x, entryPoint.y);
            printf("Next  coodrinates: [%d][%d]\n", nextPoint.x, nextPoint.y);
            clearArray(room);
            printf("\n");
            lineCnt = 0;
        } else {
            for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
                room[i][lineCnt] = line[i];
            }
            lineCnt++;
        }
    }
    return 0;
}
