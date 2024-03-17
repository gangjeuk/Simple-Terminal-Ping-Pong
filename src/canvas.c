#include <stdlib.h>
#include <stdio.h>

#include <canvas.h>

canvas_t* init_canvas(int width, int height){
    canvas_t *new_canvas;
    new_canvas = malloc(sizeof(canvas_t));

    new_canvas->width = width;
    new_canvas->height = height;

    // WHY?
    new_canvas->draw_buffer = (char *) malloc(sizeof(char) * (width +20) * (height + 5));
    clear_canvas(new_canvas);

    return new_canvas;
}

void cleanup_canvas(canvas_t* canvas) {
    free(canvas->draw_buffer);
    free(canvas);
    canvas = NULL;
    return;
}

void print_canvas(canvas_t* canvas){
    // WHY?
    // MOVE the cursor to 0,0 and print
    printf("\033[2J"); 

    int x, y;
    for (y = 0; y < canvas->height; y++)
    {
        for (x = 0; x < canvas->width; x++)
            printf("%c", canvas->draw_buffer[ y*canvas->width + x]);
        printf("\r\n");
    }        
}

// place image_data(draw_data)
void draw_image_to_canvas(canvas_t *canvas, int x, int y, int width, int height, char *draw_data){
    int cy, cx;
    int canvas_idx;
    volatile int draw_idx = 0;

    for (cy = y; cy < y + height; cy++){
        for (cx = x; cx < x + width; cx++) {
            canvas_idx = cy * canvas->width + cx;
            draw_idx = (cy - y) * width + (cx - x);

            if (draw_data[draw_idx] != -1) // -1 is "skip" / "black" char
                canvas->draw_buffer[canvas_idx] = draw_data[draw_idx];
        }
    }
}
void clear_canvas(canvas_t *canvas) {
    int i;
    int total_pixel = canvas->width * canvas->width;
    for(i = 0; i < total_pixel;i++){
        canvas->draw_buffer[i] = ' ';
    }
}