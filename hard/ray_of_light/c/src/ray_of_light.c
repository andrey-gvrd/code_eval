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

#define WALL_SIDE_NO        (0x00)
#define WALL_SIDE_LEFT      (0x01)
#define WALL_SIDE_TOP       (0x02)
#define WALL_SIDE_RIGHT     (0x04)
#define WALL_SIDE_BOTTOM    (0x08)

typedef struct {
    bool left;
    bool top;
    bool right;
    bool bottom;
    bool corner;
} Wall_t;

typedef enum {
    left = 0, up = 1, right = 2, down = 3
} Direction_t;

static const char *DIRECTION_STR[] = {
    "left", "up", "right", "down"
};

typedef struct {
    uint8_t x;
    uint8_t y;
    Elements_t type;
} Point_t;

bool rayDone = false;   // Set if ray reaches a corner

static void clearArray(char array[][ROOM_DIMENSIONS]);
static void printArray(char array[][ROOM_DIMENSIONS]);

static Elements_t charToType(char c)
{
    Elements_t type = space;
    if (c == '\\') type = ray225;
    if (c == '/')  type = ray45;
    if (c == ' ')  type = space;
    if (c == 'o')  type = column;
    if (c == '*')  type = prism;
    if (c == '#')  type = wall;
    return type;
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

static Wall_t getWallSide(Point_t point)
{
    Wall_t wall = { .left = false, .top = false, .right = false, .bottom = false,
                    .corner = false };
    bool potentialCorner = false;
    if (point.x == 0) {
        wall.left = true;
        if (potentialCorner) wall.corner = true;
        else potentialCorner = true;
    }
    if (point.x == ROOM_DIMENSIONS - 1) {
        wall.right = true;
        if (potentialCorner) wall.corner = true;
        else potentialCorner = true;
    }
    if (point.y == 0) {
        wall.top = true;
        if (potentialCorner) wall.corner = true;
        else potentialCorner = true;
    }
    if (point.y == ROOM_DIMENSIONS - 1) {
        wall.bottom = true;
        if (potentialCorner) wall.corner = true;
        else potentialCorner = true;
    }
    return wall;
}

static bool isWithinBounds(Point_t point)
{
    return ((point.x < ROOM_DIMENSIONS) && (point.y < ROOM_DIMENSIONS));
}

static Point_t getActivePoint(char room[][ROOM_DIMENSIONS], Point_t point)
{
    /* Find direction by looking at both potential point */
    /* Active point is a point with which the ray can interact:
       different ray, column, prism, wall */
    Point_t activePoint = { .x = 0, .y = 0 };
    if (point.type == ray45) {
        Point_t topRightPoint = { .x = point.x + 1, 
                                  .y = point.y - 1 };
        Point_t bottomLeftPoint = { .x = point.x - 1, 
                                    .y = point.y + 1 };
        printf("TR: [%d][%d]; BL: [%d][%d]\n", 
            topRightPoint.x, topRightPoint.y, bottomLeftPoint.x, bottomLeftPoint.y);
        /* Checking the top right point */
        if (isWithinBounds(topRightPoint)) {
            printf("Top right point is within bounds\n");
            topRightPoint.type = charToType(room[topRightPoint.y][topRightPoint.x]);
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
            bottomLeftPoint.type = charToType(room[bottomLeftPoint.y][bottomLeftPoint.x]);
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
    } else if (point.type == ray225) {
        Point_t topLeftPoint = { .x = point.x - 1, 
                                 .y = point.y - 1 };
        Point_t bottomRightPoint = { .x = point.x + 1, 
                                     .y = point.y + 1 };
        printf("TL: [%d][%d]; BR: [%d][%d]\n", 
            topLeftPoint.x, topLeftPoint.y, bottomRightPoint.x, bottomRightPoint.y);
        /* Checking the top left point */
        if (isWithinBounds(topLeftPoint)) {
            printf("Top left point is within bounds\n");
            topLeftPoint.type = charToType(room[topLeftPoint.y][topLeftPoint.x]);
            printf("TL: %s\n", ELEMENTS_STR[topLeftPoint.type]);
            if (topLeftPoint.type != ray45) {
                printf("Top left point is an active point\n");
                activePoint.x = topLeftPoint.x;
                activePoint.y = topLeftPoint.y;
                activePoint.type = topLeftPoint.type;
            }
        }
        /* Checking the bottom right point */
        if (isWithinBounds(bottomRightPoint)) {
            printf("Bottom right point is within bounds\n");
            bottomRightPoint.type = charToType(room[bottomRightPoint.y][bottomRightPoint.x]);
            printf("BR: %s\n", ELEMENTS_STR[bottomRightPoint.type]);
            if (bottomRightPoint.type != ray45) {
                printf("Bottom right point is an active point\n");
                activePoint.x = bottomRightPoint.x;
                activePoint.y = bottomRightPoint.y;
                activePoint.type = bottomRightPoint.type;
            }
        }
        printf("Active point: [%d][%d], %s\n", 
            activePoint.x, activePoint.y, ELEMENTS_STR[activePoint.type]);
    }
    return activePoint;
}

static Point_t getNextPoint(char room[][ROOM_DIMENSIONS],
                            Point_t currentPoint, Point_t activePoint)
{
    Point_t nextPoint = { .x = 0, .y = 0, .type = prism };
    printf("Current: %s, Active: %s\n", 
        ELEMENTS_STR[currentPoint.type], ELEMENTS_STR[activePoint.type]);
    switch(activePoint.type) {
        case space: {
            if ((currentPoint.type == ray45) || (currentPoint.type == ray225)) {
                nextPoint.x = activePoint.x;
                nextPoint.y = activePoint.y;
                nextPoint.type = currentPoint.type;
            }
            break;
        }
        case wall: {
            Wall_t wall = getWallSide(activePoint);
            if (wall.corner) {
                printf("Corner reached\n");
                rayDone = true;
            } else {
                /* Figure out reflection direction */
                Direction_t reflection = left;
                if (wall.left) {
                    if      (currentPoint.type == ray45)  reflection = down;
                    else if (currentPoint.type == ray225) reflection = up;
                } else if (wall.top) {
                    if      (currentPoint.type == ray45)  reflection = right;
                    else if (currentPoint.type == ray225) reflection = left;
                } else if (wall.right) {
                    if      (currentPoint.type == ray45)  reflection = up;
                    else if (currentPoint.type == ray225) reflection = down;
                } else if (wall.bottom) {
                    if      (currentPoint.type == ray45)  reflection = left;
                    else if (currentPoint.type == ray225) reflection = right;
                }
                /* Calculate next point's coordinates */
                switch (reflection) {
                    case left: {
                        nextPoint.x = currentPoint.x - 1;
                        nextPoint.y = currentPoint.y;
                        break;
                    }
                    case up: {
                        nextPoint.x = currentPoint.x;
                        nextPoint.y = currentPoint.y - 1;
                        break;
                    }
                    case right: {
                        nextPoint.x = currentPoint.x + 1;
                        nextPoint.y = currentPoint.y;
                        break;
                    }
                    case down: {
                        nextPoint.x = currentPoint.x;
                        nextPoint.y = currentPoint.y + 1;
                        break;
                    }
                }
                if      (currentPoint.type == ray45)  nextPoint.type = ray225;
                else if (currentPoint.type == ray225) nextPoint.type = ray45;
            }
            break;
        }
    }
    return nextPoint;
}

static Point_t getEntryPoint(char room[][ROOM_DIMENSIONS])
{
    Point_t entryPoint = { .x = 0, .y = 0 };
    bool entryFound = false;
    for (uint8_t i = 0; i < ROOM_DIMENSIONS && !entryFound; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS && !entryFound; ++j) {
            entryPoint.type = charToType(room[i][j]);
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
                if (!rayDone) {
                    room[nextPoint.y][nextPoint.x] = typeToChar(nextPoint.type);
                    printArray(room);
                    currentPoint.x = nextPoint.x;
                    currentPoint.y = nextPoint.y;
                    currentPoint.type = nextPoint.type;
                    travelledDistance++;
                } else {
                    rayDone = false;
                    break;
                }
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
