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

int32_t main(int32_t argc, const char *argv[]) 
{   
    char room[ROOM_DIMENSIONS][ROOM_DIMENSIONS];
    FILE *file = fopen(argv[1], "r");
    char line[ROOM_DIMENSIONS];
    uint8_t lineCnt = 0;
    while (fgets(line, ROOM_DIMENSIONS + 1, file)) {
        if (lineCnt == ROOM_DIMENSIONS) {
            printArray(room);
            clearArray(room);
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
