#include <stdio.h>  // printf, sprintf
#include <stdint.h> // uintX_t
#include <unistd.h> // usleep

#define ROOM_DIMENSIONS     (10)
#define MAX_DISTANCE        (40)

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

typedef struct {
    bool left;
    bool top;
    bool right;
    bool down;
    bool corner;
} Wall_t;

typedef enum {
    left = 0, up = 1, right = 2, down = 3, 
    upLeft = 4, upRight = 5, downRight = 6, downLeft = 7
} Direction_t;

static const char *DIRECTION_STR[] = {
    "left", "up", "right", "down", 
    "top left", "top right", "down right", "down left"
};

typedef struct {
    uint8_t x;
    uint8_t y;
    Elements_t type;
} Point_t;

/* TODO: Move to a separate ray object */
static bool rayDone = false;   // Set if ray reaches a corner or enters the path it has already taken
static Direction_t rayDirection = down;

static void clearArray(char array[][ROOM_DIMENSIONS]);
static void clearLastScreen(uint8_t lines);
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
    Wall_t wall = { .left = false, .top = false, .right = false, .down = false,
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
        wall.down = true;
        if (potentialCorner) wall.corner = true;
        else potentialCorner = true;
    }
    return wall;
}

static bool isWithinBounds(Point_t point)
{
    return ((point.x < ROOM_DIMENSIONS) && (point.y < ROOM_DIMENSIONS));
}

static Point_t getNeighbour(Point_t point, Direction_t direction)
{
    Point_t newPoint = { .x = point.x, .y = point.y };
    switch (direction) {
        case left:      { newPoint.x--;               break; }
        case upLeft:    { newPoint.x--; newPoint.y--; break; }
        case up:        {               newPoint.y--; break; }
        case upRight:   { newPoint.x++; newPoint.y--; break; }
        case right:     { newPoint.x++;               break; }
        case downRight: { newPoint.x++; newPoint.y++; break; }
        case down:      {               newPoint.y++; break; }
        case downLeft:  { newPoint.x--; newPoint.y++; break; }
    }
    return newPoint;
}

static bool isActivePoint(char room[][ROOM_DIMENSIONS], Point_t *point, Elements_t currentRay)
{
    if (isWithinBounds(*point)) {
        printf("Point is within bounds\n");
        point->type = charToType(room[point->y][point->x]);
        printf("Point's type: %s\n", ELEMENTS_STR[point->type]);
        if (point->type != currentRay) {
            return true;
        }
    }
    return false;
}

static Point_t getActivePoint(char room[][ROOM_DIMENSIONS], Point_t point)
{
    /* Active point is a point with which the ray can interact:
       different ray, column, prism, wall */
    Point_t activePoint = { .x = 0, .y = 0 };
    if (point.type == ray45) {
        if ((rayDirection == upRight) || (rayDirection == downLeft)) {
            activePoint = getNeighbour(point, rayDirection);
        } else {
            printf("Wrong ray direction: %s\n", DIRECTION_STR[rayDirection]);
        }
    } else if (point.type == ray225) {
        if ((rayDirection == upLeft) || (rayDirection == downRight)) {
            activePoint = getNeighbour(point, rayDirection);
        } else {
            printf("Wrong ray direction: %s\n", DIRECTION_STR[rayDirection]);
        }
    }
    if (!isActivePoint(room, &activePoint, point.type)) {
        rayDone = true;
        printf("Ray's done\n");
    }
    printf("Active point: [%d][%d], %s\n", 
        activePoint.x, activePoint.y, ELEMENTS_STR[activePoint.type]);
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
                    if (currentPoint.type == ray45) {
                        reflection = down;
                        rayDirection = downRight;
                    } else if (currentPoint.type == ray225) {
                        reflection = up;
                        rayDirection = upRight;
                    }
                } else if (wall.top) {
                    if (currentPoint.type == ray45) {
                        reflection = right;
                        rayDirection = downRight;
                    } else if (currentPoint.type == ray225) {
                        reflection = left;
                        rayDirection = downLeft;
                    }
                } else if (wall.right) {
                    if (currentPoint.type == ray45) {
                        reflection = up;
                        rayDirection = upLeft;
                    } else if (currentPoint.type == ray225) {
                        reflection = down;
                        rayDirection = downLeft;
                    }
                } else if (wall.down) {
                    if (currentPoint.type == ray45) {
                        reflection = left;
                        rayDirection = upLeft;
                    } else if (currentPoint.type == ray225) {
                        reflection = right;
                        rayDirection = upRight;
                    }
                }
                /* Calculate next point's coordinates */
                nextPoint = getNeighbour(currentPoint, reflection);
                if      (currentPoint.type == ray45)  nextPoint.type = ray225;
                else if (currentPoint.type == ray225) nextPoint.type = ray45;
            }
            break;
        }
        case column: {
            rayDone = true;
            break;
        }
        default: {
            if (activePoint.type == currentPoint.type) {
                rayDone = true;
            }
            break;
        }
        case prism: {
            /* Definitely needs a new coordinate calculating function */
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

static Direction_t getEntryDirection(Point_t entryPoint)
{
    Direction_t _rayDirection = down;
    Wall_t wall = getWallSide(entryPoint);
    if (entryPoint.type == ray45) {
        if (wall.right || wall.top) {
            _rayDirection = downLeft;
        } else {
            _rayDirection = upRight;
        }
    } else if (entryPoint.type == ray225) {
        if (wall.left || wall.top) {
            _rayDirection = downRight;
        } else {
            _rayDirection = upLeft;
        }
    }
    return _rayDirection;
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
            /* Set initial direction of the ray */
            rayDirection = getEntryDirection(entryPoint);
            printf("Entry direction: %s\n", DIRECTION_STR[rayDirection]);
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
        } else {
            for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
                room[i][lineCnt] = line[i];
            }
            lineCnt++;
        }
    }
    //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
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

static void clearLastScreen(uint8_t lines)
{
    for (uint8_t i = 0; i < lines; ++i) {
        printf("\033[8F\033");  // Erase the current line
        printf("\b");
    }
    printf("\x1B[0E\n\n\n\n");  // Move to the beginning of the current line
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
    /*
    usleep(1E5);
    clearLastScreen(ROOM_DIMENSIONS);
    */
}
