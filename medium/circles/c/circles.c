#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LINES 1024
#define PI 3.14159265358979323846

typedef enum bool_e 
{
    false = 0,
    true
} bool;

typedef struct circle_s
{
    float centerX;
    float centerY;
    float radius;
} circle_t;

typedef struct point_s
{
    float x;
    float y;
} point_t;

static float getFloat(const char *str, uint8_t *pos)
{
    uint32_t j = 0;
    char numbBuff[6] = {0};
    while ((isdigit(str[*pos])) || 
                   (str[*pos] == '.') || 
                   (str[*pos] == '-')) {
        numbBuff[j++] = str[(*pos)++];
    }
    (*pos)++;
    return atof(numbBuff);
}

/* Returns the position of the first character
   after the found fragment in the base string */
static char endOfStrStr(const char *strBase, const char *strFragm)
{
    uint8_t relativePos = strstr(strBase, strFragm) - &strBase[0];
    return (relativePos + strlen(strFragm));
}

static bool isInsideCircle(const circle_t c, const point_t p)
{
    float angle;
    float opposite;
    float adjacent;
    for (angle = 0.0; angle <= 90.0; angle += 0.1) {
        opposite = c.radius * sin(angle * PI / 180);
        adjacent = c.radius * cos(angle * PI / 180);
        if ((abs(opposite) >= abs(p.y)) && (abs(adjacent) >= abs(p.x))) {
            return true;
        }
    }
    return false;
}

int32_t main(int32_t argc, const char *argv[]) 
{
    FILE *file = fopen(argv[1], "r");
    char line[MAX_LINES];
    while (fgets(line, MAX_LINES, file)) {
        circle_t c;
        char pos;
        /* Parsing circle's coordinates */
        pos = endOfStrStr(&line[0], "Center: (");
        c.centerX = getFloat(&line[0], &pos);
        pos++;
        c.centerY = getFloat(&line[0], &pos);
        //printf("Circle is at: [%.2f, %.2f]\n", c.centerX, c.centerY);
        /* Parsing circle's radius */
        pos = endOfStrStr(&line[0], "Radius: ");
        c.radius = getFloat(&line[0], &pos);
        //printf("Circle's radius is: %.2f\n", c.radius);
        /* Parsing point's coordinates */
        pos = endOfStrStr(&line[0], "Point: (");
        point_t p;
        p.x = getFloat(&line[0], &pos);
        pos++;
        p.y = getFloat(&line[0], &pos);
        //printf("Point is at: [%.2f, %.2f]\n", p.x, p.y);
        /* Centering the cirlce on zero and moving the point */
        p.x -= c.centerX;
        p.y -= c.centerY;
        //printf("Moved point is at: [%.2f, %.2f]\n", p.x, p.y);
        /* Checking whether the point is inside the circle */
        if (isInsideCircle(c, p)) {
            printf("true\n");
        } else {
           printf("false\n");
        }
    }
    return 0;
}
