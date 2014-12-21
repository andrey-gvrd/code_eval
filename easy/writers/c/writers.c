#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 1024

typedef enum bool_e 
{
    false = 0,
    true
} bool;

static uint32_t findPipe(char *line)
{
    uint32_t pos = 0;
    while (line[pos] != '|') {
        if (line[pos] == '\n') {
            return -1;
        } else {
            pos++;
        }
    }
    return pos;
}

int32_t main(int32_t argc, const char *argv[]) 
{
    FILE *file = fopen(argv[1], "r");
    char line[MAX_LINES];
    while (fgets(line, MAX_LINES, file)) {
        uint32_t i = findPipe(&line[0]);
        if (i < 0) {
            continue;
        } else {
            i += 2;
        }
        while (line[i - 1] != '\n') {
            uint32_t j = 0;
            char numbBuff[2] = {0};
            while (isdigit(line[i])) {
                numbBuff[j++] = line[i++];
            }
            i++;
            printf("%c", line[atoi(numbBuff) - 1]);
            memset(&numbBuff[0], 0x00, 2);
        }
        printf("\n");
    }
    return 0;
}
