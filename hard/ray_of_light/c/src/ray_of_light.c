#include <stdio.h>  // printf, sprintf
#include <stdint.h> // uintX_t
#include <stdlib.h> // atoi
#include <ctype.h>  // isdigit

#define ROOM_DIMENSIONS     (10)
#define MAX_DISTANCE        (9)

typedef enum {
    false = 0,
    true = 1
} bool;

static const char *BOOL_STR[] = {
    "false", "true"
};

typedef enum { 
    ray225 = 0, ray45 = 1, space = 2, column = 3, prism = 4, wall = 5 
} Elements_t;

static const char *ELEMENTS_STR[] = {
    "ray225", "ray45", "space", "column", "prism", "wall"
};

typedef enum {
    no = 0, left = 1, top = 2, right = 3, bottom = 4
} EntrySide_t;

static const char *ENTRY_SIDE_STR[] = {
    "no", "left", "top", "right", "bottom"
};

typedef struct {
    uint8_t x;
    uint8_t y;
    Elements_t type;
} Point_t;

static void clearArray(char array[][ROOM_DIMENSIONS]);
static void printArray(char array[][ROOM_DIMENSIONS]);

static Elements_t identify(char c)
{
    Elements_t type = space;
    if (c == '\\') type = ray225;
    if (c == '/')  type = ray45;
    if (c == ' ')  type = space;
    if (c == 'o')  type = column;
    if (c == '*')  type = prism;
    if (c == '#')  type = wall;
    return elementType;
}

static char typeToChar(Elements_t type)
{
    char c = 'N';
    if (type == ray225) c = '\\';
    if (type == ray45)  c = '/';
    if (type == space)  c = ' ';
    if (type == column) c = 'o';
    if (type == prism)  c = '*';
    if (type == wall)   c = '#';
    return c;
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

static bool isWithinBounds(Point_t point)
{
    return ((point.x < ROOM_DIMENSIONS) && (point.y < ROOM_DIMENSIONS));
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

static Point_t getActivePoint(char room[][ROOM_DIMENSIONS], Point_t point)
{
    /* Find direction by looking at both potential point */
    /* Active point is a point with which the ray can interact:
       different ray, column, prism, wall */
    Point_t activePoint = { .x = 0, .y = 0 };
    if (point.type = ray45) {
        Point_t topRightPoint = { .x = point.x + 1, 
                                  .y = point.y - 1 };
        Point_t bottomLeftPoint = { .x = point.x - 1, 
                                    .y = point.y + 1 };
        printf("TR: [%d][%d]; BL: [%d][%d]\n", 
            topRightPoint.x, topRightPoint.y, bottomLeftPoint.x, bottomLeftPoint.y);
        /* Checking the top right point */
        if (isWithinBounds(topRightPoint)) {
            printf("Top right point is within bounds\n");
            topRightPoint.type = identify(room[topRightPoint.y][topRightPoint.x]);
            printf("TR: %s\n", ELEMENTS_STR[topRightPoint.type]);
            if (topRightPoint.type != ray45) {
                printf("Top right point is an active point\n");
                activePoint.x = topRightPoint.x;
                activePoint.y = topRightPoint.y;
                activePoint.type = topRightPoint.type;
            }
        }
        /* Checking the bottom left point */
        if (isWithinBounds(bottomLeftPoint)) {
            printf("Bottom left point is within bounds\n");
            bottomLeftPoint.type = identify(room[bottomLeftPoint.y][bottomLeftPoint.x]);
            printf("BL: %s\n", ELEMENTS_STR[bottomLeftPoint.type]);
            if (bottomLeftPoint.type != ray45) {
                printf("Bottom left point is an active point\n");
                activePoint.x = bottomLeftPoint.x;
                activePoint.y = bottomLeftPoint.y;
                activePoint.type = bottomLeftPoint.type;
            }
        }
        printf("Active point: [%d][%d], %s\n", 
            activePoint.x, activePoint.y, ELEMENTS_STR[activePoint.type]);
    } else if (point.type = ray225) {
        /* */
    }
    return activePoint;
}

static Point_t getNextCharacter(Elements_t currentElement, Elements_t nextElement)
{
    char nextChar = 'N';
    printf("Current: %s, Next: %s\n", 
        ELEMENTS_STR[currentElement], ELEMENTS_STR[nextElement]);
    switch(nextElement) {
        case space: {
            if (currentElement == ray45) {
                nextChar = '/';
            } else if ((currentElement == ray225)) {
                nextChar = '\\';
            }
            break;
        }
        case wall: {
            if (currentElement == ray45) {
                nextChar = '\\';
            } else if ((currentElement == ray225)) {
                nextChar = '/';
            }           
            break;
        }
    }
    return nextChar;
}

static Point_t getEntryPoint(char room[][ROOM_DIMENSIONS])
{
    Point_t entryPoint = { .x = 0, .y = 0 };
    bool entryFound = false;
    for (uint8_t i = 0; i < ROOM_DIMENSIONS && !entryFound; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS && !entryFound; ++j) {
            entryPoint.type = identify(room[i][j]);
            if ((entryPoint.type == ray45) ||
                (entryPoint.type == ray225)) {
                entryPoint.x = j;
                entryPoint.y = i;
                entryFound = true;
            }
        }
    }
    printf("Entry point: [%d][%d], %s\n", 
        entryPoint.x, entryPoint.y, ELEMENTS_STR[entryPoint.type]);
    return entryPoint;
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
            Point_t entryPoint = getEntryPoint(room);
            /* Replace cells with appropriate rays */
            uint8_t travelledDistance = 0;
            Point_t currentPoint = { .x = entryPoint.x, .y = entryPoint.y,
                                     .type = entryPoint.type };
            while (travelledDistance < MAX_DISTANCE) {
                Point_t activePoint = getActivePoint(room, currentPoint);
                Point_t nextPoint = getNextPoint(room, currentPoint, activePoint);
                room[nextPoint.y][nextPoint.x] = typeToChar(nextPoint.type);
                printArray(room);
                currentPoint.x = nextPoint.x;
                currentPoint.y = nextPoint.y;
                currentPoint.type = nextPoint.type;
                travelledDistance++;
            }
            clearArray(room);
            lineCnt = 0;
            printf("\n---------------\n");
        } else {
            for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
                room[i][lineCnt] = line[i];
            }
            lineCnt++;
        }
    }
    return 0;
}

/** 
 * Array manipulattion functions 
 */
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
