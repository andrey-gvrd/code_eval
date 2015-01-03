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
    ray225 = 0,
    ray45 = 1,
    space = 2,
    column = 3,
    prism = 4,
    wall = 5
} Elements_t;

typedef enum {
    no = 0,
    left = 1,
    top = 2,
    right = 3,
    bottom = 4
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
    printf("\n");
    /* Print top index */
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        printf("%d", i);
    }
    printf("\n\n");
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            printf("%c", array[i][j]);
        }
        printf(" %d", i);   // Print right index
        printf("\n");
    }
}

static Elements_t identify(char c)
{
    Elements_t elementType = space;
    printf("Identifying %c\n", c);
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

static bool isWithinBounds(uint8_t i)
{
    return (i < ROOM_DIMENSIONS);
}

static Point_t nextCoordinatesForEntry(Point_t point, Elements_t rayType,
                                       EntrySide_t entrySide)
{
    Point_t nextPoint = { .x = 0, .y = 0 };
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

static Point_t nextCoordinates(char room[][ROOM_DIMENSIONS], 
                               Point_t point, Elements_t rayType)
{
    printf("Looking for next point for: [%d][%d]\n", point.x, point.y);
    Point_t nextPoint = { .x = 0, .y = 0 };
    EntrySide_t entrySide = getRayEntrySide(point);
    if (entrySide != no) {
        printf("Looking for a next point for entry ray\n");
        printf("Entry side: %d\n", entrySide);
        nextPoint = nextCoordinatesForEntry(point, rayType, entrySide);
    } else {
        if (rayType == ray45) {
            printf("Looking for a next point for 45 degree ray\n");
            bool acceptablePoint  = true;
            Point_t potentPoint;
            potentPoint.x = point.x + 1;
            potentPoint.y = point.y - 1;
            printf("Potential next point: [%d][%d]\n", potentPoint.x,
                                                       potentPoint.y);
            Elements_t potentPointType = identify(room[potentPoint.y]
                                                      [potentPoint.x]);
            printf("Potential next point's type: %d\n", potentPointType);
            if ((isWithinBounds(potentPoint.x)) && 
                (isWithinBounds(potentPoint.y))) {
                printf("Top right index is within bounds\n");
                if (potentPointType == ray45) {
                    printf("Top right point is ray's parent\n");
                    acceptablePoint = false;
                    potentPoint.x = point.x - 1;
                    potentPoint.y = point.y + 1;
                    printf("Potential next point: [%d][%d]\n", potentPoint.x,
                                                               potentPoint.y);
                    potentPointType = identify(room[potentPoint.y]
                                                   [potentPoint.x]);
                    printf("Potential next point's type: %d\n", potentPointType);
                    if (potentPointType == space) {
                        printf("Bottom left point is available\n");
                        nextPoint.x = potentPoint.x;
                        nextPoint.y = potentPoint.y;
                        printf("Next point: [%d][%d]\n", potentPoint.x,
                                                         potentPoint.y);
                    } else {
                        printf("Top right point isn't available\n");
                    }
                } else {
                    printf("Top right point isn't ray's parent\n");
                    if (potentPointType == space) {
                        printf("Top right point is available\n");
                        nextPoint.x = potentPoint.x;
                        nextPoint.y = potentPoint.y;
                        printf("Next point: [%d][%d]\n", potentPoint.x,
                                                         potentPoint.y);
                    } else {
                        printf("Top right point isn't available\n");
                    }
                }
            } else {
                printf("Top right index is out of bounds\n");
                acceptablePoint = false;
                potentPoint.x = point.x - 1;
                potentPoint.y = point.y + 1;
                printf("Potential next point: [%d][%d]\n", potentPoint.x,
                                                           potentPoint.y);
                potentPointType = identify(room[potentPoint.y]
                                               [potentPoint.x]);
                printf("Potential next point's type: %d\n", potentPointType);
                if (potentPointType == space) {
                    printf("Bottom left point is available\n");
                    nextPoint.x = potentPoint.x;
                    nextPoint.y = potentPoint.y;
                    printf("Next point: [%d][%d]\n", potentPoint.x,
                                                     potentPoint.y);
                } else {
                    printf("Bottom left point isn't available\n");
                    printf("Nothing to do\n");
                }
            }
        } else if (rayType == ray225) {
            printf("Looking for a next point for 225 degree ray\n");
        }
    }
    return nextPoint;
}

static char getNextCharacter(Elements_t elementType, Elements_t nextElementType)
{
    char nextChar = 'N';
    switch(nextElementType) {
        case space: {
            if (elementType == ray45) {
                nextChar = '/';
            } else if ((elementType == ray225)) {
                nextChar = '\\';
            }
            break;
        }
    }
    return nextChar;
}

int32_t main(int32_t argc, const char *argv[]) 
{   
    char room[ROOM_DIMENSIONS][ROOM_DIMENSIONS];
    clearArray(room);
    FILE *file = fopen(argv[1], "r");
    char line[ROOM_DIMENSIONS];
    uint8_t lineCnt = 0;
    while (fgets(line, ROOM_DIMENSIONS + 1, file)) {
        if (lineCnt == ROOM_DIMENSIONS) {
            printArray(room);
            /* Find first ray */
            Point_t entryPoint = { .x = 0, .y = 0 };
            Elements_t elementType;
            bool entryFound = false;
            for (uint8_t i = 0; i < ROOM_DIMENSIONS && !entryFound; ++i) {
                for (uint8_t j = 0; j < ROOM_DIMENSIONS && !entryFound; ++j) {
                    elementType = identify(room[i][j]);
                    if ((elementType == ray45) ||
                        (elementType == ray225)) {
                        entryPoint.x = j;
                        entryPoint.y = i;
                        entryFound = true;
                    }
                }
            }
            printf("Entry: [%d][%d]\n", entryPoint.x, entryPoint.y);
            /* Replace cells with appropriate rays */
            uint8_t travelledDistance = 0;
            Point_t currentPoint = { .x = entryPoint.x, .y = entryPoint.y };
            while (travelledDistance < 20) {
                Point_t nextPoint = nextCoordinates(room, 
                                                    currentPoint, elementType);
                printf("Next point: [%d][%d]\n", nextPoint.x, nextPoint.y);
                Elements_t nextPointType = identify(room[nextPoint.y][nextPoint.x]);
                printf("Next point type: %d\n", nextPointType);
                char nextChar = getNextCharacter(elementType, nextPointType);
                room[nextPoint.y][nextPoint.x] = nextChar;
                printArray(room);
                currentPoint.x = nextPoint.x;
                currentPoint.y = nextPoint.y;
                travelledDistance++;
            }
            clearArray(room);
            printf("\n");
            printf("---------------");
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
