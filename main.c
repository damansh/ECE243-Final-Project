#include <stdio.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// code not shown for clear_screen() and draw_line() subroutines
void draw_line(int x0, int y0, int x1, int y1, short int line_color) {

    bool is_steep = abs(y1 - y0) > abs(x1 - x0);

    if(is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if(x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax/2);
    int y = y0;

    int y_step;
    if (y0 < y1) {
        y_step = 1;
    } else {
        y_step = -1;
    }

    int x;
    for (x = x0; x < x1; x++) {
        if(is_steep) {
            plot_pixel(y,x, line_color);
        } else {
            plot_pixel(x,y, line_color);
        }
        error = error + deltay;

        if(error >= 0) {
            y = y + y_step;
            error = error - deltax;
        }

    }
}

void clear_screen() {
    int x;
    for(x = 0; x < 320; x++) {
        int y;
        for(y = 0; y < 240; y++) {
            plot_pixel(x, y, 0xffff);
        }
    }
}

void background(){
    clear_screen();
    draw_line(0, 120, 319, 120, 0x0000);   // this line is blue
    draw_line(160, 0, 160, 239, 0x0000); // this line is green
    int x,y;
    for(x=0;x<320;x=x+16){
        draw_line(x,0,x,239,0x3432);
    }
    for(y=0;y<240;y=y+12){
        draw_line(0,y,319,y,0x1111);
    }
    draw_line(0,239,319,239,0x0000);
    draw_line(319,0,319,239,0x3432);
    draw_line(0, 120, 319, 120, 0x0000);   // this line is blue
    draw_line(160, 0, 160, 239, 0x0000); // this line is green
}

void plotter(){
    int x = 0;
    for(; x<320; x++) {
        int y = 120 - ((x-160)+1);
        /*
        if(y>0) {
            y = 120-y;
        } else {
            y = y + 120;
        } */
        if(y > 0 && y < 240) {
            plot_pixel(x, y, 0xff00);
        }
    }
}

void plotxsquared(){
    int x,y=0;
    int count = 120;
//    for (x=0; x<320; x++){
//        int y =  x^2;
//        if(y>0 && y<240){
//            plot_pixel(x,y,0xff00);
//        }
//    }

    for (x=160;x<320;x++){
        if(count >0 ){
            plot_pixel(x,count,0xff00);
            count --;
        }

    }

}

int main(void){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
    background();
    //plotter();
    plotxsquared();
}

