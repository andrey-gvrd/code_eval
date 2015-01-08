#include <stdio.h>  // printf
//#include <unistd.h> // usleep

#define ROOM_DIMENSIONS     (10)
#define MAX_DISTANCE        (40)
#define MAX_RAYS            (23)
#define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
 #define PRINT(...) printf(__VA_ARGS__);
#else
 #define PRINT(...) (void)0
#endif

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
    left = 0, upLeft = 1, up = 2, upRight = 3, 
    right = 4, downRight = 5, down = 6, downLeft = 7
} Direction_t;

static const char *DIRECTION_STR[] = {
    "left", "up left", "up", "up right", 
    "right", "down right", "down", "down left"
};

typedef struct {
    uint8_t x;
    uint8_t y;
    Elements_t type;
} Point_t;

static uint8_t rayCnt;

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
static void printRays(Ray_t *ray, uint8_t rayCnt);

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
        potentialCorner = true;
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
    }
    return wall;
}

static bool isWithinBounds(Point_t point)
{
    return (bool)((point.x < ROOM_DIMENSIONS) && (point.y < ROOM_DIMENSIONS));
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
        PRINT("Potential point [%d][%d] is within bounds\n", point->x, point->y);
        point->type = charToType(room[point->x][point->y]);
        PRINT("Potential point's type: %s\n", ELEMENTS_STR[point->type]);
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
    PRINT("Working with a ray of type %s at [%d][%d]\n", 
        ELEMENTS_STR[ray->currentPoint.type], 
        ray->currentPoint.x, ray->currentPoint.y);
    if (ray->currentPoint.type == ray45) {
        if ((ray->direction == upRight) || (ray->direction == downLeft)) {
            ray->activePoint = getNeighbour(ray->currentPoint, ray->direction);
            PRINT("Selected active point\n");
        } else {
            PRINT("Wrong ray direction: %s\n", DIRECTION_STR[ray->direction]);
        }
    } else if (ray->currentPoint.type == ray225) {
        if ((ray->direction == upLeft) || (ray->direction == downRight)) {
            ray->activePoint = getNeighbour(ray->currentPoint, ray->direction);
            PRINT("Selected active point\n");
        } else {
            PRINT("Wrong ray direction: %s\n", DIRECTION_STR[ray->direction]);
        }
    }
    if (!isActivePoint(room, &(ray->activePoint), ray->currentPoint.type)) {
        ray->finished = true;
        PRINT("Ray's done because inactive point has been reached\n");
    }
    PRINT("Active point: [%d][%d], %s\n", 
        ray->activePoint.x, ray->activePoint.y, 
        ELEMENTS_STR[ray->activePoint.type]);
}

static Direction_t oppositeDirection(Direction_t inDir)
{
    Direction_t outDir;
    if (inDir <= 3) {outDir = inDir + 4;}
    else            {outDir = inDir - 4;}
    return outDir;
}

static void getNextPoint(char room[][ROOM_DIMENSIONS], Ray_t *ray)
{
    PRINT("Current: %s, Active: %s\n", 
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
                PRINT("Corner reached\n");
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
                Elements_t reflectionSpaceType = charToType(room[ray->nextPoint.x]
                                                                [ray->nextPoint.y]);
                if ((ray->nextPoint.type != reflectionSpaceType) && 
                    ((reflectionSpaceType == ray45) || (reflectionSpaceType == ray225))) {
                    PRINT("Crossing a different ray type after reflection\n");
                    ray->nextPoint.type = cross;
                }
            }
            break;
        }
        case column: {
            PRINT("Column reached\n");
            ray->finished = true;
            break;
        }
        case prism: {
            /* For 3 corners of the prism, if the point is a space or a different ray,
               start a new ray there with the same direction as the point is in relation
               to the prism, i.e. downRight point of the prism has downRight outgoing 
               direction. */
            uint8_t rayIndex = 0;
            PRINT("Ray is finished because it entered into a prism\n");
            ray->finished = true;   // Ray entering a prism is finished
            Point_t prismPoint = ray->activePoint;
            PRINT("Prism encountered at [%d][%d]\n", 
                prismPoint.x, prismPoint.y);
            /* Hacky way to iterate through direction enums */
            for (uint8_t dir = 0; dir <= 7; ++dir) {
                if (dir % 2 != 0) {
                    PRINT("Checking direction: %s\n", DIRECTION_STR[dir]);
                    if (oppositeDirection(ray->direction) != dir) {
                        if (rayCnt < MAX_RAYS) {
                            rayIndex++;
                            PRINT("Creating new ray #%d at %s from prism location\n", 
                                rayIndex, DIRECTION_STR[dir]);
                            /* Spawn a new ray */
                            /* Setting up the next point */
                            ray[rayIndex].nextPoint = getNeighbour(prismPoint, (Direction_t)dir);
                            PRINT("New ray's next coordinates: [%d][%d]\n", 
                                ray[rayIndex].nextPoint.x, 
                                ray[rayIndex].nextPoint.y);
                            ray[rayIndex].direction = (Direction_t)dir;
                            PRINT("New ray's direction: %s\n", 
                                DIRECTION_STR[ray[rayIndex].direction]);
                            if ((ray[rayIndex].direction == upRight) || 
                                (ray[rayIndex].direction == downLeft)) {
                                ray[rayIndex].nextPoint.type = ray45;
                            } else {
                                ray[rayIndex].nextPoint.type = ray225;
                            }
                            PRINT("New ray's type: %s\n", 
                                ELEMENTS_STR[ray[rayIndex].nextPoint.type]);
                            /* Setting up the current point */
                            ray[rayIndex].currentPoint.x = prismPoint.x;
                            ray[rayIndex].currentPoint.y = prismPoint.y;
                            PRINT("New ray's current coordinates: [%d][%d]\n",
                                ray[rayIndex].currentPoint.x, 
                                ray[rayIndex].currentPoint.y);
                            ray[rayIndex].currentPoint.type = ray[rayIndex].nextPoint.type;
                            /* Setting up ray's parameters */
                            ray[rayIndex].finished = false;
                            ray[rayIndex].travelledDistance = 0;
                            ray[rayIndex].justReflected = false;
                            rayCnt++;
                        } else {
                            printf("MAX AMOUNT OF RAYS HAS BEEN REACHED\n");
                        }
                    } else {
                        PRINT("This is where the ray came from\n")
                    }
                }
            }
            break;
        }
        /* Handling meeting with other rays */
        default: {
            if (ray->activePoint.type == ray->currentPoint.type) {
                PRINT("Ray of the same type reached\n");
                ray->finished = true;
            } else {
                PRINT("Ray of a different type encountered\n");
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
    Direction_t rayDirection = down;
    Wall_t wall = getWallSide(entryPoint);
    if (entryPoint.type == ray45) {
        if (wall.right || wall.top) {
            rayDirection = downLeft;
        } else {
            rayDirection = upRight;
        }
    } else if (entryPoint.type == ray225) {
        if (wall.left || wall.top) {
            rayDirection = downRight;
        } else {
            rayDirection = upLeft;
        }
    }
    return rayDirection;
}

static bool isWall(Point_t point)
{
    return (bool)((point.x == ROOM_DIMENSIONS - 1) || (point.x == 0) ||
                  (point.y == ROOM_DIMENSIONS - 1) || (point.y == 0));
}

static uint8_t getEntryRays(char room[][ROOM_DIMENSIONS], Ray_t *rays)
{
    uint8_t _rayCnt = 0;
    Point_t tmpPoint;
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            tmpPoint.x = i;
            tmpPoint.y = j;
            if (isWall(tmpPoint)) {
                tmpPoint.type = charToType(room[i][j]);
                if ((tmpPoint.type == ray45) ||
                    (tmpPoint.type == ray225)) {
                    /* Initialise a new ray object */
                    rays[_rayCnt].currentPoint.x = i;
                    rays[_rayCnt].currentPoint.y = j;
                    rays[_rayCnt].currentPoint.type = tmpPoint.type;
                    rays[_rayCnt].direction = getEntryDirection(rays[_rayCnt].currentPoint);
                    rays[_rayCnt].finished = false;
                    rays[_rayCnt].travelledDistance = 0;
                    PRINT("Ray #%d with type %s entered at: [%d][%d]\n", 
                        _rayCnt, ELEMENTS_STR[rays[_rayCnt].currentPoint.type], 
                        rays[_rayCnt].currentPoint.x, rays[_rayCnt].currentPoint.y);
                    _rayCnt++;
                }
            }
        }
    }
    PRINT("Total of %d rays found.\n", _rayCnt);
    return _rayCnt;
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
                for (uint i = 0; i < rayCnt; i++) {
                    if (rays[i].travelledDistance >= MAX_DISTANCE) {
                        PRINT("Ray #%d travelled for more than %d points and is done\n",
                            i, MAX_DISTANCE);
                        rays[i].finished = true;
                    }
                    /* Ray may be finished for variety of reasons */
                    if (!rays[i].finished) {
                        PRINT("Working with ray #%d\n", i);
                        //printRays(rays, rayCnt);
                        getActivePoint(room, &rays[i]);
                        getNextPoint(room, &rays[i]);
                        //printRays(rays, rayCnt);
                        /* Second check because it might have finished while inside */
                        if (!rays[i].finished) {
                            room[rays[i].nextPoint.x][rays[i].nextPoint.y] =
                            typeToChar(rays[i].nextPoint.type);
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
                            PRINT("Ray #%d has been finished off while looking for a next point\n", i);
                            allDone = areAllRaysDone(rays, rayCnt);
                        }
                    } else {
                        PRINT("Finished ray #%d found\n", i);
                        allDone = areAllRaysDone(rays, rayCnt);
                    }
                    printArray(room);
                }
                PRINT("Finished cycling through %d rays\n", rayCnt);
            }
            PRINT("All rays are done\n");
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
    //PRINT("\n\n\n\n\n\n\n\n\n\n\n\n\n");
    return 0;
}

/** 
 * Array manipulation functions
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
        PRINT("\033[8F\033");  // Erase the current line
        PRINT("\b");
    
}    PRINT("\x1B[0E\n");  // Move to the beginning of the current line
}

static void printArray(char array[][ROOM_DIMENSIONS])
{
    printf("\n");
    /* Print top index */
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        PRINT("%d", i);
    }
    PRINT("\n\n");
    for (uint8_t i = 0; i < ROOM_DIMENSIONS; ++i) {
        for (uint8_t j = 0; j < ROOM_DIMENSIONS; ++j) {
            printf("%c", array[j][i]);
        }
        PRINT(" %d", i);   // Print right index
        printf("\n");
    }
    /*
    usleep(1.3E5);
    clearLastScreen(ROOM_DIMENSIONS);
    */
}

static void printRays(Ray_t *ray, uint8_t rayCnt)
{
    for (uint8_t i = 0; i < rayCnt; ++i) {
        printf("#%d: c[%d][%d] %s, n[%d][%d] %s, a[%d][%d] %s, dir %s, dst %d, jr? %s, f? %s\n", 
            i,
            ray[i].currentPoint.x, ray[i].currentPoint.y, ELEMENTS_STR[ray[i].currentPoint.type],
            ray[i].nextPoint.x, ray[i].nextPoint.y, ELEMENTS_STR[ray[i].nextPoint.type],
            ray[i].activePoint.x, ray[i].activePoint.y, ELEMENTS_STR[ray[i].activePoint.type],
            DIRECTION_STR[ray[i].direction], ray[i].travelledDistance, 
            BOOL_STR[ray[i].justReflected], BOOL_STR[ray[i].finished]);
    }
}
