#pragma once


#define DEFAULT_TERMINAL_REFRESH 10000 //us
#define PORT_NUM 7777

// Player
typedef enum {P1 = 1, P2 } player_t;

// User bar variables
#define INITIAL_BAR_MOVE 0
#define BAR_SPEED_INC 0.05
#define BAR_MOVE_KEY_DELAY 50

// Brick variables
#define SINGLEPLAY_BRICK_WIDTH 34
#define SINGLEPLAY_BRICK_HEIGHT 10
#define SINGLEPLAY_BRICK_TOTAL = (SINGLEPLAY_BRICK_WIDTH * SINGLEPLAY_BRICK_HEIGHT)

// WALL
#define SINGLEPLAY_WALL_THICKNESS 1
#define MULTIPLAY_WALL_THICKNESS 3

// Ball ('x' symbol) variables
#define INITIAL_BALL_SPEED 0.1
#define BALL_SPEED_INC 0.0005


// Single play game box size 
#define SINGLEPLAY_WIDTH (40 + SINGLEPLAY_WALL_THICKNESS * 2) // #..."bricks"...# shape plus 2 because of #

#define SINGLEPLAY_HEIGHT (30 + SINGLEPLAY_WALL_THICKNESS * 2)  //  ------  
                                    //
                                    //
                                    //  ------   
                                    // shape plsu 2 because of -

// Multiplay
#define MULTIPLAY_BRICK_WIDTH 3
#define MULTIPLAY_BRICK_HEIGHT 18
#define MULTIPLAY_BRICK_TOTAL = (MULTIPLAY_BRICK_WIDTH * MULTIPLAY_BRICK_HEIGHT)
#define MULTIPLAY_WIDTH (70 + MULTIPLAY_WALL_THICKNESS * 2)
#define MULTIPLAY_HEIGHT (24 + MULTIPLAY_WALL_THICKNESS * 2)

// Bar movement direction
typedef enum {RIGHT, LEFT} bar_direction_t;

// Game
void play(long delay_time, int multiplay);

// game support
void setup_border(char *border, int size);
void draw_score();
void draw_game_over();
void get_bar_movement_input(char *bar, player_t p);

// bricks init
void init_ball();
void init_bricks(char *buffer, int width, int height);
void init_bar(char *bar, int bar_size, const char *bar_shape, int bar_shape_size);
void update_ball();
void update_brick();
// game physics
char can_move(bar_direction_t dir, char *bar);
int overlap(int x, int y);
