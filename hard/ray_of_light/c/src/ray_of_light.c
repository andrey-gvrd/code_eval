#include <stdio.h>  // printf, sprintf
#include <stdint.h> // uintX_t
#include <unistd.h> // usleep

#define ROOM_DIMENSIONS     (10)
#define MAX_DISTANCE        (40)
#define MAX_RAYS            (10)

typedef enum {
    false = 0,
    true = 1
} bool;

static const char *BOOL_STR[] = {
    "false", "true"
};

typedef enum { 
    ray225 = 0, ray45 = 1, space = 2, column = 3, prism = 4, wall = 5, cross = 6
} Elements_t;

static const char *ELEMENTS_STR[] = {
    "ray225", "ray45", "space", "column", "prism", "wall", "cross"
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

typedef struct {
    Point_t currentPoint;
    Point_t activePoint;
    Point_t nextPoint;
    Direction_t direction;
    uint8_t travelledDistance;
    bool finished;
    bool justReflected;
} Ray_t;

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
    if (c == 'X')  type = cross;
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
    if (type == cross)  c = 'X';
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

static bool isActivePoint(char room[][ROOM_DIMENSIONS], 
                          Point_t *point, Elements_t currentRay)
{
    if (isWithinBounds(*point)) {
        printf("Potential point [%d][%d] is within bounds\n", point->x, point->y);
        point->type = charToType(room[point->y][point->x]);
        printf("Potential point's type: %s\n", ELEMENTS_STR[point->type]);
        if (point->type != currentRay) {
            return true;
        }
    }
    return false;
}

static void getActivePoint(char room[][ROOM_DIMENSIONS], Ray_t *ray)
{
    /* Active point is a point with which the ray can interact:
       different ray, column, prism, wall */
    printf("Working with a ray of type %s at [%d][%d]\n", 
        ELEMENTS_STR[ray->currentPoint.type], ray->currentPoint.x, ray->currentPoint.y);
    if (ray->currentPoint.type == ray45) {
        if ((ray->direction == upRight) || (ray->direction == downLeft)) {
            ray->activePoint = getNeighbour(ray->currentPoint, ray->direction);
        } else {
            printf("Wrong ray direction: %s\n", DIRECTION_STR[ray->direction]);
        }
    } else if (ray->currentPoint.type == ray225) {
        if ((ray->direction == upLeft) || (ray->direction == downRight)) {
            ray->activePoint = getNeighbour(ray->currentPoint, ray->direction);
        } else {
            printf("Wrong ray direction: %s\n", DIRECTION_STR[ray->direction]);
        }
    }
    if (!isActivePoint(room, &(ray->activePoint), ray->currentPoint.type)) {
        rayDone = true;
        printf("Ray's done because inactive point has been reached\n");
    }
    printf("Active point: [%d][%d], %s\n", 
        ray->activePoint.x, ray->activePoint.y, 
        ELEMENTS_STR[ray->activePoint.type]);
}

static void getNextPoint(char room[][ROOM_DIMENSIONS], Ray_t *ray)
{
    printf("Current: %s, Active: %s\n", 
        ELEMENTS_STR[ray->currentPoint.type], ELEMENTS_STR[ray->activePoint.type]);
    ray->justReflected = false;
    switch(ray->activePoint.type) {
        case space: {
            if ((ray->currentPoint.type == ray45) || 
                (ray->currentPoint.type == ray225)) {
                ray->nextPoint.x = ray->activePoint.x;
                ray->nextPoint.y = ray->activePoint.y;
                ray->nextPoint.type = ray->currentPoint.type;
            }
            break;
        }
        case wall: {
            ray->justReflected = true;
            Wall_t wall = getWallSide(ray->activePoint);
            if (wall.corner) {
                printf("Corner reached\n");
                ray->finished = true;
            } else {
                /* Figure out reflection direction */
                Direction_t reflection = left;
                if (wall.left) {
                    if (ray->currentPoint.type == ray45) {
                        reflection = down;
                        ray->direction = downRight;
                    } else if (ray->currentPoint.type == ray225) {
                        reflection = up;
                        ray->direction = upRight;
                    }
                } else if (wall.top) {
                    if (ray->currentPoint.type == ray45) {
                        reflection = right;
                        ray->direction = downRight;
                    } else if (ray->currentPoint.type == ray225) {
                        reflection = left;
                        ray->direction = downLeft;
                    }
                } else if (wall.right) {
                    if (ray->currentPoint.type == ray45) {
                        reflection = up;
                        ray->direction = upLeft;
                    } else if (ray->currentPoint.type == ray225) {
                        reflection = down;
                        ray->direction = downLeft;
                    }
                } else if (wall.down) {
                    if (ray->currentPoint.type == ray45) {
                        reflection = left;
                        ray->direction = upLeft;
                    } else if (ray->currentPoint.type == ray225) {
                        reflection = right;
                        ray->direction = upRight;
                    }
                }
                ray->nextPoint = getNeighbour(ray->currentPoint, reflection);
                if      (ray->currentPoint.type == ray45)  ray->nextPoint.type = ray225;
                else if (ray->currentPoint.type == ray225) ray->nextPoint.type = ray45;
                /* Handling crossing different rays after reflection */
                Elements_t reflectionSpaceType = charToType(room[ray->nextPoint.y]
                                                                [ray->nextPoint.x]);
                if ((ray->nextPoint.type != reflectionSpaceType) && 
                    ((reflectionSpaceType == ray45) || (reflectionSpaceType == ray225))) {
                    printf("Crossing a different ray type after reflection\n");
                    ray->nextPoint.type = cross;
                }
            }
            break;
        }
        case column: {
            printf("Column reached\n");
            ray->finished = true;
            break;
        }
        case prism: {
            #if 0
            /* For 3 corners of the prism, if the point is a space or a different ray,
               start a new ray there with the same direction as the point is in relation
               to the prism, i.e. downRight point of the prism has downRight direction. */
            Point_t rays[MAX_RAYS];
            /* Hacky way to iterate through direction enums */
            for (uint8_t direction = 4; direction < 7; ++direction) {
                if (rayDirection != direction) {
                    /* Spawn a new ray */
                    nextPoint = getNeighbour(activePoint, rayDirection);
                    if ((direction == upRight) || (direction == downLeft)) {
                        nextPoint.type = ray225;
                    } else {
                        nextPoint.type = ray45;
                    }
                }
            }
            #endif
            break;
        }
        /* Handling meeting with other rays */
        default: {
            if (ray->activePoint.type == ray->currentPoint.type) {
                printf("Ray of the same type reached\n");
                ray->finished = true;
            } else {
                printf("Ray of a different type encountered\n");
                ray->nextPoint.x = ray->activePoint.x;
                ray->nextPoint.y = ray->activePoint.y;
                ray->nextPoint.type = cross;
            }
            break;
        }
    }
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

static bool isWall(Point_t point)
{
    return ((point.x == ROOM_DIMENSIONS - 1) || (point.x == 0) ||
            (point.y == ROOM_DIMENSIONS - 1) || (point.y == 0));
}

static uint8_t getEntryRays(char room[][ROOM_DIMENSIONS], Ray_t *rays)
{
    uint8_t rayCnt;
    Point_t tmpPoint;
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            tmpPoint.x = j;
            tmpPoint.y = i;
            if (isWall(tmpPoint)) {
                tmpPoint.type = charToType(room[i][j]);
                if ((tmpPoint.type == ray45) ||
                    (tmpPoint.type == ray225)) {
                    /* Initialise a new ray object */
                    rays[rayCnt].currentPoint.x = j;
                    rays[rayCnt].currentPoint.y = i;
                    rays[rayCnt].currentPoint.type = tmpPoint.type;
                    rays[rayCnt].direction = getEntryDirection(rays[rayCnt].currentPoint);
                    rays[rayCnt].finished = false;
                    rays[rayCnt].travelledDistance = 0;

                    printf("Ray #%d with type %s entered at: [%d][%d]\n", 
                        rayCnt, ELEMENTS_STR[rays[rayCnt].currentPoint.type], 
                        rays[rayCnt].currentPoint.x, rays[rayCnt].currentPoint.y);

                    rayCnt++;
                }
            }
        }
    }
    printf("Total of %d rays found.\n", rayCnt);
    return rayCnt;
}

static bool areAllRaysDone(Ray_t *rays, uint8_t rayCnt)
{
    bool allDone = true;
    for (uint8_t i = 0; i < rayCnt; ++i) {
        if (!rays[i].finished) {
            allDone = false;
        }
    }
    return allDone;
}

int32_t main(int32_t argc, const char *argv[]) 
{   
    char room[ROOM_DIMENSIONS][ROOM_DIMENSIONS];
    clearArray(room);

    Ray_t rays[MAX_RAYS];
    uint8_t rayCnt;

    FILE *file = fopen(argv[1], "r");
    char line[ROOM_DIMENSIONS];
    uint8_t lineCnt = 0;

    while (fgets(line, ROOM_DIMENSIONS + 1, file)) {
        if (lineCnt == ROOM_DIMENSIONS) {
            printArray(room);
            /* Find first rays */
            rayCnt = getEntryRays(room, rays);
            bool allDone = false;  // true if all rays are done
            while (!allDone) {
                for (uint i = 0; i < rayCnt; ++i) {
                    if (rays[i].travelledDistance >= MAX_DISTANCE) {
                        printf("Ray #%d travelled for more than %d points and is done\n",
                            i, MAX_DISTANCE);
                        rays[i].finished = true;
                    }
                    /* Ray may be finished for variety of reasons */
                    if (!rays[i].finished) {
                        printf("Working with ray #%d\n", i);
                        getActivePoint(room, &rays[i]);
                        getNextPoint(room, &rays[i]);
                        /* Second check because it might have finished while inside */
                        if (!rays[i].finished) {
                            room[rays[i].nextPoint.y][rays[i].nextPoint.x] =
                            typeToChar(rays[i].nextPoint.type);
                            printArray(room);
                            rays[i].currentPoint.x = rays[i].nextPoint.x;
                            rays[i].currentPoint.y = rays[i].nextPoint.y;
                            /* Hacky way to save ray's type if a different ray has been crossed */
                            if (!rays[i].justReflected) {
                                if (rays[i].nextPoint.type != cross) {
                                    rays[i].currentPoint.type = rays[i].nextPoint.type;
                                }
                            } else {
                                /* So sorry for this */
                                if (rays[i].currentPoint.type == ray45) {
                                    rays[i].currentPoint.type = ray225;
                                } else if (rays[i].currentPoint.type == ray225){
                                    rays[i].currentPoint.type = ray45;
                                }
                            }
                            rays[i].travelledDistance++;
                        } else {
                            printf("Finished ray #%d found\n", i);
                            allDone = areAllRaysDone(rays, rayCnt);
                        }
                    } else {
                        printf("Finished ray #%d found\n", i);
                        allDone = areAllRaysDone(rays, rayCnt);
                    }
                }
            }
            printf("All rays are done\n");
            clearArray(room);
            allDone = false;
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
