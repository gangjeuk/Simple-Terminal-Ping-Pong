#include <sys/types.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#define BUF_LEN 10000
#define LISTEN_LIMIT 1  // shoud less than 5
#define TOTAL_FDPL (LISTEN_LIMIT + 1)
int cli_sock;
int serv_sock;
int maxndfs;
int user_num = 0;
fd_set readfds;


#include <stdio.h> // stdin input
#include <stdlib.h> // malloc
#include <unistd.h>
#include <time.h> // 
#include <math.h>
#include <stdbool.h> // boolean

#include <brick.h>
#include <net.h>
#include <canvas.h>
#include <game.h>
#include <key.h>

#define SMALL_DOUBLE 0.0000000000001f // for float divide


canvas_t *g_canvas;
ball_t ball;


int p1_collide_block_idx = -1;
int p1_collide_block_nums = 0;
int p2_collide_block_idx = -1;
int p2_collide_block_nums = 0;

// else game
char g_quit_flag;
int g_score = 0;
int g_delay;

// var for bar
float p1_bar_move_spd = INITIAL_BAR_MOVE;
float p1_bar_key_delay = BAR_MOVE_KEY_DELAY;
int p2_bar_move_spd = INITIAL_BAR_MOVE;
int p2_bar_key_delay = BAR_MOVE_KEY_DELAY;

// Game
void play(long delay_time, int multiplay) {

    struct sockaddr_in cli_addr;
    char buf[BUF_LEN];
    int accept_len = sizeof(struct sockaddr_in);

    /*
     * game grid 40 x ?
     * // TODO select ? later
     * 40 is because of the bar size 
     * bar's width 10 and game allows 3 moves.
     * every move take 5 
     * 
     * |-----|-----|-----||----------||-----|-----|-----|
     *                          ^
     *                          |
     *                  initial bar position
     */

// init variables
int canvas_width, canvas_height, canvas_total_char;
int brick_width, brick_height;
int bar_width, bar_shape_length;
const char *bar_shape;
int ball_start_x, ball_start_y;
int wall_thickness;
switch(multiplay){

case true:
    wall_thickness = MULTIPLAY_WALL_THICKNESS;
    canvas_width = MULTIPLAY_WIDTH, canvas_height = MULTIPLAY_HEIGHT;
    canvas_total_char = canvas_width * canvas_height;
    
    brick_width = MULTIPLAY_BRICK_WIDTH, brick_height = MULTIPLAY_BRICK_HEIGHT;

    bar_shape = multiplay_bar_shape;
    bar_width = canvas_height - 2 * wall_thickness;
    bar_shape_length = sizeof(multiplay_bar_shape);

    ball_start_x = 30;
    ball_start_y = 12;
break;
case false:
    wall_thickness = SINGLEPLAY_WALL_THICKNESS;
    canvas_width = SINGLEPLAY_WIDTH, canvas_height = SINGLEPLAY_HEIGHT;
    canvas_total_char = canvas_width * canvas_height;
    
    brick_width = SINGLEPLAY_BRICK_WIDTH, brick_height = SINGLEPLAY_BRICK_HEIGHT;

    bar_shape = singleplay_bar_shape;
    bar_width = canvas_width - 2 * wall_thickness;
    bar_shape_length = sizeof(singleplay_bar_shape);    

    ball_start_x = 21;
    ball_start_y = 26;
break;
}

    g_canvas = init_canvas(canvas_width, canvas_height);
    char border[canvas_total_char];
    char p1_bricks[brick_width * brick_height];
    char p2_bricks[brick_width * brick_height];
    char p1_bar[bar_width];
    char p2_bar[bar_width];
    setup_border((char*) border, canvas_total_char);
    init_ball(ball_start_x, ball_start_y);
    init_bricks((char *)p1_bricks, brick_width, brick_height);
    init_bricks((char *)p2_bricks, brick_width, brick_height);
    init_bar(p1_bar, bar_width, bar_shape, bar_shape_length);
    init_bar(p2_bar, bar_width, bar_shape, bar_shape_length);

    g_quit_flag = 0;
    g_delay = delay_time;
    p1_collide_block_idx = -1;
    p1_collide_block_nums = 0;    
    p2_collide_block_idx = -1;
    p2_collide_block_nums = 0;


switch(multiplay) {
    // when multiplay
case true:

    serv_sock = tcp_to_listen(PORT_NUM);
    FD_ZERO(&readfds);
    FD_SET(serv_sock, &readfds);

    FD_SET(cli_sock, &readfds);

    // routine for get new user
    if(FD_ISSET(serv_sock, &readfds)){
        cli_sock = accept(serv_sock, (struct sockaddr*)&cli_addr, (socklen_t*)&accept_len);
        if(cli_sock == -1)
        {
            printf("accept failed\n");
            exit(EXIT_FAILURE);
        }
        send(cli_sock, "WELCOME!!\n", sizeof("WELCOME!!\n"), 0);
        memset(buf, 0, BUF_LEN);
        
        // set maxndfs
        maxndfs = cli_sock + 1;
    }


    while(!g_quit_flag) {
                struct timeval tv = { 0L, 0L };
                FD_ZERO(&readfds);
                FD_SET(serv_sock, &readfds);
                FD_SET(STDIN_FILENO, &readfds);
                FD_SET(cli_sock, &readfds);
                select(cli_sock + 1, &readfds, NULL, NULL, &tv);
        /* ---------- game routine ---------------- */
            // draw bar
                get_bar_movement_input(p2_bar, P2);
                get_bar_movement_input(p1_bar, P1);
                draw_image_to_canvas(g_canvas, 28, 3, 1, bar_width, p1_bar);
                draw_image_to_canvas(g_canvas, 47, 3, 1, bar_width, p2_bar);

                // draw border
                draw_image_to_canvas(g_canvas, 0, 0, g_canvas->width, g_canvas->height, (char *)border);

                /*  ball physics and draw. complicate!!*/
                // draw ball
                draw_image_to_canvas(g_canvas, (int)ball.past_x, (int)ball.past_y, 1, 1, " ");

                // cal ball 
                // draw ball
                update_ball();

                /* ball finish */

                // check hit 
                update_brick(p1_bricks, p1_collide_block_idx);
                update_brick(p2_bricks, p2_collide_block_idx);

                // draw bricks
                draw_image_to_canvas(g_canvas, 8, 6, brick_width, brick_height, p1_bricks);
                draw_image_to_canvas(g_canvas, 63, 6, brick_width, brick_height, p2_bricks);

                // finally print g_canvas buffer
                print_canvas(g_canvas);

                sprintf(buf, "%s", g_canvas->draw_buffer);
                send(cli_sock, buf, sizeof(buf), 0);

                usleep(g_delay);
        /* ---------- game routine ---------------- */


    }
    close(cli_sock);
    close(serv_sock);
break;
    // when single play
case false:

    while(!g_quit_flag) {
        struct timeval tv = { 0L, 0L };
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
        // draw bar
        get_bar_movement_input(p1_bar, P1);
        draw_image_to_canvas(g_canvas, 1, 27, bar_width, 1, p1_bar);
        

        // draw border
        draw_image_to_canvas(g_canvas, 0, 0, g_canvas->width, g_canvas->height, (char *)border);

        // draw score  
        draw_score();

        /*  ball physics and draw. complicate!!*/
        // draw ball
        draw_image_to_canvas(g_canvas, (int)ball.past_x, (int)ball.past_y, 1, 1, " ");

        // cal ball 
        // draw ball
        update_ball();

        /* ------  ball finish  ------ */

        // check hit 
        update_brick(p1_bricks, p1_collide_block_idx);

        // draw bricks
        draw_image_to_canvas(g_canvas, 4, 4, brick_width, brick_height, p1_bricks);

        // finally print g_canvas buffer
        print_canvas(g_canvas);

        usleep(g_delay);
    }

break;
}

    draw_game_over();
    print_canvas(g_canvas);

    char key = 0;

    while (key != 127) {
        while(!keyboard_hit(0));
        key = get_char(0);
    }

    cleanup_canvas(g_canvas);
    
}

void setup_border(char *border, int size) {
    int thickness = g_canvas->width == SINGLEPLAY_WIDTH ? SINGLEPLAY_WALL_THICKNESS : MULTIPLAY_WALL_THICKNESS;
    int i, j,total_char = g_canvas->width * g_canvas->height;
            // DANGER need init_canvase before call this function

    for (i = 0; i < total_char; i++)
        border[i] = -1;

    for(i = 0; i < g_canvas->width; i++){
        for(j = 0; j < thickness; j++){
            border[i + g_canvas->width * j] = '#'; // top of canvas
            border[i + g_canvas->width * (g_canvas->height - 1 - j)] = '#'; // bottom of canvas
        }
    }
    for(i = 0; i < g_canvas->height; i++){
        for(j = 0; j < thickness; j++){
            border[g_canvas->width * i + j] = '|';// leftside of canvas
            border[g_canvas->width * i + g_canvas->width - 1 - j] = '|';// rightside of canvas
        }
    }
}

void draw_game_over() {
    for(int i = 4; i < g_canvas->width - 4; i++)
        g_canvas->draw_buffer[(10 * g_canvas->width) + i] = ' ';

    for(int i = 4; i < g_canvas->width - 4; i++)
        g_canvas->draw_buffer[(11 * g_canvas->width) + i] = ' ';

    for(int i = 4; i < g_canvas->width - 4; i++)
        g_canvas->draw_buffer[(13 * g_canvas->width) + i] = ' ';

    for(int i = 4; i < g_canvas->width - 4; i++)
        g_canvas->draw_buffer[(14 * g_canvas->width) + i] = ' ';

    g_canvas->draw_buffer[11 * g_canvas->width + 13] = 'G';
    g_canvas->draw_buffer[11 * g_canvas->width + 15] = 'A';
    g_canvas->draw_buffer[11 * g_canvas->width + 17] = 'M';
    g_canvas->draw_buffer[11 * g_canvas->width + 19] = 'E';
    g_canvas->draw_buffer[11 * g_canvas->width + 23] = 'O';
    g_canvas->draw_buffer[11 * g_canvas->width + 25] = 'V';
    g_canvas->draw_buffer[11 * g_canvas->width + 27] = 'E';
    g_canvas->draw_buffer[11 * g_canvas->width + 29] = 'R';

    g_canvas->draw_buffer[13 * g_canvas->width + 3] = 'P';
    g_canvas->draw_buffer[13 * g_canvas->width + 5] = 'r';
    g_canvas->draw_buffer[13 * g_canvas->width + 7] = 'e';
    g_canvas->draw_buffer[13 * g_canvas->width + 9] = 's';
    g_canvas->draw_buffer[13 * g_canvas->width + 11] = 's';
    g_canvas->draw_buffer[13 * g_canvas->width + 13] = 'B';
    g_canvas->draw_buffer[13 * g_canvas->width + 15] = 'A';
    g_canvas->draw_buffer[13 * g_canvas->width + 17] = 'C';
    g_canvas->draw_buffer[13 * g_canvas->width + 19] = 'K';
    g_canvas->draw_buffer[13 * g_canvas->width + 21] = 't';
    g_canvas->draw_buffer[13 * g_canvas->width + 23] = 'o';
    g_canvas->draw_buffer[13 * g_canvas->width + 25] = 'R';
    g_canvas->draw_buffer[13 * g_canvas->width + 27] = 'e';
    g_canvas->draw_buffer[13 * g_canvas->width + 29] = 's';
    g_canvas->draw_buffer[13 * g_canvas->width + 31] = 'e';
    g_canvas->draw_buffer[13 * g_canvas->width + 33] = 't';
    g_canvas->draw_buffer[13 * g_canvas->width + 35] = '.';
}

void draw_score() {
    g_canvas->draw_buffer[0] = 'S';
    g_canvas->draw_buffer[1] = 'C';
    g_canvas->draw_buffer[2] = 'O';
    g_canvas->draw_buffer[3] = 'R';
    g_canvas->draw_buffer[4] = 'E';
    g_canvas->draw_buffer[5] = ':';

    // Get the first 5 digits of the score
    char digits[5];
    int score = g_score;

    for(int i = 0; i < 5; i++) {
        digits[i] = (score % 10) + '0';
        score /= 10;
    }

    g_canvas->draw_buffer[6] = digits[4];
    g_canvas->draw_buffer[7] = digits[3];
    g_canvas->draw_buffer[8] = digits[2];
    g_canvas->draw_buffer[9] = digits[1];
    g_canvas->draw_buffer[10] = digits[0];
}

void get_bar_movement_input(char *bar, player_t p){
    int i, count = 0;
    int bar_width = g_canvas->width == SINGLEPLAY_WIDTH ? SINGLEPLAY_WIDTH - SINGLEPLAY_WALL_THICKNESS * 2 : MULTIPLAY_HEIGHT - MULTIPLAY_WALL_THICKNESS * 2;
    int bar_shape_size = g_canvas->width == SINGLEPLAY_WIDTH ? sizeof(singleplay_bar_shape) - 1 : sizeof(multiplay_bar_shape) - 1;
    char bar_char = g_canvas->width == SINGLEPLAY_WIDTH ? '-' : '|';
    char key_left = p == P1 ? 'a' : 'l';
    char key_right = p == P1 ? 'd' : 'j';
    int bar_speed = p == P1 ? p1_bar_move_spd : p2_bar_move_spd;
    int fs = p == P1 ? STDIN_FILENO : cli_sock;

    if(FD_ISSET(fs, &readfds)) {
        //int key = get_char(fs, readfds);
        char key;
        char buf[BUF_LEN];
        if(fs == STDIN_FILENO){
            read(fs, &key, sizeof(key));
        }else{
            recv(fs, buf, sizeof(buf), MSG_DONTWAIT);
            key = buf[0];
        }

        if(key == 127)
            g_quit_flag = 1;
        else if(key == key_left && can_move(LEFT, bar))
            goto move_left;
        else if(key == key_right && can_move(RIGHT, bar))
            goto move_right;
        else
            goto quit;

        move_left:
        if(bar_speed > 0)
            bar_speed = 0;

        bar_speed -= BAR_SPEED_INC;
        for(i = bar_width - 1; i >= 0; i--)
        {

            if(bar[i] == bar_char){
                bar[i] = ' ';
                bar[i - bar_shape_size] = bar_char;
                count++;
            }
            if(count == bar_shape_size / 2)
                goto quit;
        }

        move_right:
        if(bar_speed < 0)
            bar_speed = 0;

        bar_speed += BAR_SPEED_INC;
        for(i = 0; i < bar_width; i++)
        {
            if(bar[i] == bar_char){
                bar[i] = ' ';
                bar[i + bar_shape_size] = bar_char;
                count++;
            }
            if(count == bar_shape_size / 2)
                goto quit;
        }
    }
quit:
    if(p == P1)
        p1_bar_move_spd = bar_speed;
    else
        p2_bar_move_spd = bar_speed;
    return;
}

// bricks init
void init_ball(int start_x, int start_y){
    ball.x = start_x;
    ball.y = start_y;
    ball.past_x = ball.x;
    ball.past_y = ball.y;
    ball.x_speed = 0.1;
    ball.y_speed = -0.1;

    return;
}

void init_bricks(char *buffer, int width, int height){
    int i, total_char = width * height;
    
    for(i = 0; i < total_char; i++)
    {
        buffer[i] = '=';
    }
    return;
}

void init_bar(char *bar, int bar_size, const char *bar_shape, int bar_shape_size)
{
    int i;
    int half_bar_size = bar_size / 2;
    int half_bar_shape_size = bar_shape_size / 2;
    // -2 to ignore wall(ex: #)
    for(i=0; i < half_bar_size * 2; i++)
    {
        bar[i] = ' ';
    }
    for(i = half_bar_size - half_bar_shape_size; i < half_bar_size + half_bar_shape_size; i++)
        bar[i] = bar_shape[i - (half_bar_size - half_bar_shape_size)];
    return;
}

void update_brick(char *bricks, int collide_block){
    if(collide_block != -1)
        bricks[collide_block] = ' ';


    /*
    if(p1_collide_block_nums >= SINGLEPLAY_BRICK_HEIGHT * SINGLEPLAY_BRICK_WIDTH)
        g_quit_flag = 1;
    */

    // sometimes ball stuck in one place and never go back to bar

if(g_canvas->width == SINGLEPLAY_WIDTH){
    // give some value to escape from stuck
    if(fabs(ball.y_speed) < 0.65)
        ball.y_speed *= 1.000005;
    else if(fabs(ball.y_speed) > 0.7)
        ball.y_speed *= 0.999995;
}
else{
    if(fabs(ball.x_speed) < 12)
        ball.x_speed *= 1.000005;
    else if(fabs(ball.x_speed) > 1.43)
        ball.x_speed *= 0.95;    
}
    return;
}


// game physics
void update_ball(){
    draw_image_to_canvas(g_canvas, (int)ball.past_x, (int)ball.past_y, 1, 1, " ");


    int pos_x = (int)ball.past_x;
    int pos_y = (int)ball.past_y;
    int next_x = (int)(ball.x + ball.x_speed);
    int next_y = (int)(ball.y + ball.y_speed);
    
    // sorry ;; for spaghetti
    bool is_multi = g_canvas->width == MULTIPLAY_WIDTH ? true : false;
    bool right_side = ball.x > MULTIPLAY_WIDTH / 2 ? true : false;

    int overlap_idx;

    if((overlap_idx = overlap((int)next_x, (int)next_y)))
    {
        draw_image_to_canvas(g_canvas, (int)ball.past_x, (int)ball.past_y, 1, 1, (char *)&ball_shape);


switch(is_multi){
case true:
        // accerlate when meet bar
        if(overlap_idx == BAR && fabs(ball.x_speed) < 5.1f){
            if(right_side){
                ball.x_speed += p2_bar_move_spd;
                next_x = (int)(ball.x + ball.x_speed);

            }
            else{
                ball.x_speed += p1_bar_move_spd;
                next_x = (int)(ball.x + ball.x_speed);
            }
        }
        else if(overlap_idx != true)     // !=true means ball overlap with brick '='
        {
            if(right_side){
                p2_collide_block_idx = overlap_idx;
                p2_collide_block_nums += 1;
                g_score += 10;
            }
            else{
                p1_collide_block_idx = overlap_idx;
                p1_collide_block_nums += 1;
                g_score += 10;
            }
        }
break;

case false:
        // accerlate when meet bar
        if(overlap_idx == BAR && fabs(ball.x_speed) < 0.65f){
            ball.x_speed += p1_bar_move_spd;
            next_x = (int)(ball.x + ball.x_speed);
        }
        else if(overlap_idx != true)     // !=true means ball overlap with brick '='
        {
            // update later
            // Check block update
            // SUCH A BAD CODE
            p1_collide_block_idx = overlap_idx;
            p1_collide_block_nums += 1;
            g_score += 10;
        }
break;
}




        int vec_x = next_x - pos_x;
        int vec_y = next_y - pos_y;

        if(vec_x != 0 && vec_y != 0)
        {
            double sine = ball.y_speed / (sqrt((pow(ball.x_speed, 2) + pow(ball.y_speed, 2))) + SMALL_DOUBLE);

            if(sine >= 1/2 || sine <= -(1/2)){
                vec_x = 0;
            }else{
                vec_y = 0;

            }
        }

        // physics in game

        double cosine = (vec_x * ball.x_speed + vec_y * ball.y_speed) / ((sqrt(pow(vec_x , 2) + pow(vec_y, 2)) * sqrt(pow(ball.x_speed, 2) + pow(ball.y_speed, 2))) + SMALL_DOUBLE);

        double vec_x_cosine = sqrt(pow(ball.x_speed, 2) + pow(ball.y_speed, 2)) * cosine;
        double ratio = vec_x_cosine / (sqrt(pow(vec_x, 2) + pow(vec_y, 2)) + SMALL_DOUBLE);

        double basic_vec_x =  -1.0f * (double)vec_x * fabs(ratio);
        double basic_vec_y = -1.0f * (double)vec_y * fabs(ratio);
        
        ball.x_speed += 2.0f * basic_vec_x;
        ball.y_speed += 2.0f * basic_vec_y;
        ball.x += ball.x_speed;
        ball.y += ball.y_speed;
        return;
    }

    ball.x += ball.x_speed;
    ball.y += ball.y_speed;
    draw_image_to_canvas(g_canvas, (int)next_x, (int)next_y, 1, 1, (char *)&ball_shape);
    ball.past_x = ball.x;
    ball.past_y = ball.y;
    return;
    
}

char can_move(bar_direction_t dir, char *bar){
    int tmp = 2;
    int bar_length = g_canvas->width == SINGLEPLAY_WIDTH ? SINGLEPLAY_WIDTH - SINGLEPLAY_WALL_THICKNESS * 2 : MULTIPLAY_HEIGHT - MULTIPLAY_WALL_THICKNESS * 2;
    if(dir == RIGHT && bar[bar_length - tmp] == ' '){
        return true;
    }
    else if(dir == LEFT && bar[tmp] == ' '){
        return true;
    }

    return false;
}

int overlap(int x, int y)
{
    int brick_width, dis_from_wall_x, dis_from_wall_y;
    brick_width = g_canvas->width == SINGLEPLAY_WIDTH ? SINGLEPLAY_BRICK_WIDTH : MULTIPLAY_BRICK_WIDTH;  
    dis_from_wall_x = g_canvas->width == SINGLEPLAY_WIDTH ? 4 : 9;  
    dis_from_wall_y = g_canvas->width == SINGLEPLAY_WIDTH ? 4 : 6; 

    if(g_canvas->width != SINGLEPLAY_WIDTH)
        dis_from_wall_x = x > MULTIPLAY_WIDTH / 2 ? 55 : 8; 
    // becareful!
    // return true is equal to return 1 


    int idx = x + g_canvas->width * y;
    int empty = g_canvas->draw_buffer[idx] == ' ' ? false : true;
    
    char overlap_char = g_canvas->draw_buffer[idx];
    if(overlap_char == '=')
    {
        // should not be 1
        return (x - dis_from_wall_x) + brick_width * (y - dis_from_wall_y);
    }
    else if(overlap_char == '-') // accelerate when meet bar
    {
        return BAR;
    }else if(overlap_char == '|' && g_canvas->width != SINGLEPLAY_WIDTH)
    {
        return BAR;
    }

    return empty;
}




/* ---------- multi play -------------------------*/


void set_maxndfs()
{   
    int i = 0;
    for(i = 0; i < user_num; i++)
    {
        if(cli_sock > maxndfs)
            maxndfs = cli_sock;
    }

    maxndfs += 1;
}


