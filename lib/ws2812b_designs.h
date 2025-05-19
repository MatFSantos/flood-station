#ifndef WS2812B_DESIGNS_H
#define WS2812B_DESIGNS_H

#include <stdint.h>

/**
 * @brief Struct to the color configuration.
 * 
 * @param leds matrix that contains wich led in WS2812B matrix will be used
 * @param color matrix that contains the color that will be used in frame (values between 0 and 1)
 */
typedef struct {
    uint8_t leds[25];
    double color[3];
} Frame;

static const Frame ZERO = {
    {0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame ONE = {
    {0, 1, 1, 1, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 1, 1, 0, 0,
    0, 0, 1, 0, 0},
    {0.1, 0, 0}
};
static const Frame TWO = {
    {0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame THREE = {
    {0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame FOUR = {
    {0, 1, 0, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0},
    {0.1, 0, 0}
};
static const Frame FIVE = {
    {0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame SIX = {
    {0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame SEVEN = {
    {0, 1, 0, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 0, 0, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame EIGHT = {
    {0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};
static const Frame NINE = {
    {0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0},
    {0.1, 0, 0}
};

static const Frame SMILE = {
    {0,0,0,0,0,
    0,1,1,1,0,
    1,0,0,0,1,
    0,1,0,1,0,
    0,0,0,0,0},
    {0,0.1,0}
};

static const Frame SAD = {
    {1,0,0,0,1,
    0,1,1,1,0,
    0,0,0,0,0,
    0,1,0,1,0,
    0,0,0,0,0},
    {0.1,0,0}
};

static const Frame NEUTRAL = {
    {0,0,0,0,0,
    1,1,1,1,1,
    0,0,0,0,0,
    0,1,0,1,0,
    0,0,0,0,0},
    {0.1,0.1,0.1}
};

static const Frame ARROW_LEFT = {
    {0,0,1,0,0,
    0,1,0,0,0,
    1,1,1,1,1,
    0,1,0,0,0,
    0,0,1,0,0},
    {0.1,0.1,0.1}
};
static const Frame ARROW_RIGHT = {
    {0,0,1,0,0,
    0,0,0,1,0,
    1,1,1,1,1,
    0,0,0,1,0,
    0,0,1,0,0},
    {0.1,0.1,0.1}
};

static const Frame GREEN_TL = {
    {0, 0, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 0, 0},
    {0, 0.1, 0}
};
static const Frame YELLOW_TL = {
    {0, 0, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 0, 0},
    {0.1, 0.1, 0}
};

static const Frame RED_TL = {
    {0, 0, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 0, 0},
    {0.1, 0, 0}
};

static const Frame DANGER = {
    {0, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     0, 1, 1, 1, 0,
     0, 0, 1, 0, 0,
     0, 0, 0, 0, 0},
    {0.1, 0, 0}
};

static const Frame *NUMERIC[] = {
    &ZERO, &ONE, &TWO,
    &THREE, &FOUR, &FIVE,
    &SIX, &SEVEN, &EIGHT,
    &NINE
};

#endif