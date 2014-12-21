#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES           (128)
#define MAX_CHARS_IN_NUMBER (4)
#define NUM_POINTS          (2000)
#define MAX_LINE_LENGTH     ((MAX_CHARS_IN_NUMBER + 1) * NUM_POINTS)

#define MOVING_AVERAGE_SAMPLE_SIZE  (64)

typedef enum bool_e 
{
    false = 0,
    true
} bool;

typedef struct point_s
{
    int32_t x;
    int32_t y;
} point_t;

typedef struct arrayEntry_s
{
    int32_t value;
    uint32_t index;
} arrayEntry_t;

static void createNumberMatrix(const char *fileName, int32_t numMatrix[][NUM_POINTS], 
                               uint32_t *rowIndex)
{
    FILE *file = fopen(fileName, "r");
    char line[MAX_LINE_LENGTH];
    *rowIndex = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        uint32_t i = 0;
        uint32_t columnIndex = 0;
        char numbBuff[MAX_CHARS_IN_NUMBER] = {0};
        for (columnIndex = 0; columnIndex < NUM_POINTS; columnIndex++) {
            uint32_t j = 0;
            while (isdigit(line[i]) ||
                          (line[i]) == '-') {
                numbBuff[j++] = line[i++];
            }
            i++;
            numMatrix[*rowIndex][columnIndex] = atoi(numbBuff);
            memset(&numbBuff[0], 0x00, MAX_CHARS_IN_NUMBER);
        }
        (*rowIndex)++;
    }
}

static void saveMatrix(int32_t array[][NUM_POINTS], const uint32_t arraySize, 
                       const char *fileName)
{
    uint32_t i, j;
    FILE *file = fopen(fileName, "w");
    for (i = 0; i < arraySize; i++) {
        for (j = 0; j < NUM_POINTS; j++) {
            fprintf(file, "%d\n", array[i][j]);
        }
        fprintf(file, "\n");
    }
}

static void movingAverage(const int32_t inArray[], const uint32_t inArraySize, 
                          const uint32_t sampleSize,
                          int32_t outArray[], uint32_t *outArraySize)
{
    uint32_t i, j;
    int64_t movingSum = 0;
    uint32_t sampleSizeBy2 = sampleSize / 2;
    *outArraySize = 0;
    for (i = sampleSizeBy2; i < inArraySize - sampleSizeBy2; i++) {
        for (j = i - sampleSizeBy2; j < i + sampleSizeBy2; j++) {
            movingSum += inArray[j];
        }
        outArray[i] = movingSum / sampleSize;
        (*outArraySize)++;
        movingSum = 0;
    }
}

static void lineariseArray(int32_t array[], const float a, const float b)
{
    uint32_t i;
    for (i = 0; i < NUM_POINTS; i++) {
        array[i] -= a * i + b;
    }
}

static void getLinearCoefficients(point_t p1, point_t p2,
                                  float *a, float *b)
{
    *a = (p1.y - p2.y) / (float)(p1.x - p2.x);
    *b = (p2.y * (p1.x - p2.x) - p2.x * (p1.y - p2.y)) / (float)(p1.x - p2.x);
}

/* Actually the way to do this is detect near pairs of values on opposite 
   sides of x line */
static void getZeroes(const int32_t inArray[], int32_t *outArray, 
                      uint32_t *zeroCnt, uint32_t *firstIndex, uint32_t *lastIndex)
{
    uint32_t i, j;
    uint32_t tempIndex = 0;
    *zeroCnt = 0;
    *firstIndex = 0;
    *lastIndex = 0;
    for (i = 0; i < NUM_POINTS; i++) {
        outArray[i] = 0;
    }
    for (i = 0; i < NUM_POINTS - 2; i++) {
        arrayEntry_t first, second;
        first.index = i;
        first.value = inArray[first.index];
        second.index = i + 2;
        second.value = inArray[second.index];
        if ((first.value >= 0) && (second.value < 0)) {
            outArray[i + 1] = 50;
            if (*firstIndex == 0) {
                *firstIndex = i + 1;
            }
            i += 3;
            (*zeroCnt)++;
            tempIndex = i + 1;
            continue;
        }
        *lastIndex = tempIndex;
    }
}

int32_t main(int32_t argc, const char *argv[]) 
{
    int32_t input[MAX_LINES][NUM_POINTS] = {0};
    int32_t output[MAX_LINES][NUM_POINTS] = {0};
    uint32_t numSignals = 0;
    uint32_t i, j;
    /* Exctracting data from the input file */
    createNumberMatrix(argv[1], input, &numSignals);
    /* Running moving average on every signal */
    for (i = 0; i < numSignals; i++) {
        uint32_t outSize = 0;
        /* Create a straight line from the signal representing average value */
        movingAverage(input[i], NUM_POINTS,
                      MOVING_AVERAGE_SAMPLE_SIZE,
                      output[i], &outSize);
        point_t p1, p2;
        p1.x = MOVING_AVERAGE_SAMPLE_SIZE + 1;
        p1.y = output[i][MOVING_AVERAGE_SAMPLE_SIZE + 1];
        p2.x = outSize;
        p2.y = output[i][outSize];
        float a, b;
        getLinearCoefficients(p1, p2, &a, &b);
        lineariseArray(input[i], a, b);
        memset(&output[i], 0, NUM_POINTS);
        uint32_t firstIndex, lastIndex;
        uint32_t zeroCnt;
        getZeroes(input[i], output[i], &zeroCnt, &firstIndex, &lastIndex);
        int64_t numbCnts = lastIndex - firstIndex;
        //printf("nmbcnts: %d, zeroCnt: %d\n", numbCnts, zeroCnt);
        float seconds = numbCnts / (float)20000;
        uint32_t frequency = (float)(zeroCnt - 1) / seconds;
        uint8_t remainder = frequency % 10;
        if (remainder >= 5) {
            frequency += (10 - remainder);
        } else {
            frequency -= remainder;
        }
        printf("%d\n", frequency);
    }
    /* Debug output */
    //saveMatrix(output, numSignals, "out.txt");
    return 0;
}
