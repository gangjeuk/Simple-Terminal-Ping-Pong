#pragma once

#include <canvas.h> // canvas buffer

const char brick_shape = '#';
const char ball_shape[] = "x";
const char singleplay_bar_shape[] = "----------";
const char multiplay_bar_shape[] = "||||||";

typedef enum {WALL, BRCIKS, BALL, BAR} brick_type_t;

typedef struct{
    int max_moveable_block;
    float max_x_speed, max_y_speed;

    float x, y;
    float past_x, past_y;
    float x_speed, y_speed;

}ball_t;



