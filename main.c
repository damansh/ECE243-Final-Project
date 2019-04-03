#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.14159265

volatile int pixel_buffer_start; // global variable
extern short MYIMAGE [240][320];

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

void plotsin() {
    double valsX[100];
    double valsY[100];
    
    double i;
    int count =0;
    for(i=-5; i<=5; i = i + 0.25) {
        valsY[count] = sin(i*PI);
        valsX[count] = i*PI;
        count++;
    }
    int k;
    for(k=0; k<count; k++) {
        valsY[k] = 120 - 10*valsY[k];
        valsX[k] = 10*valsX[k] + 160;
        
        if(valsY[k]>0 && valsY[k]<240 && valsX[k] > 0 && valsX[k] < 320){
            plot_pixel((int)round(valsX[k]),(int)round(valsY[k]),0xF800);
        }
    }
    
    int j;
    for(j=0; j < count-1; j++) {
        if(valsY[j]>0 && valsY[j]<240 && valsX[j] > 0 && valsX[j] < 320){
            draw_line(round(valsX[j]), round(valsY[j]), round(valsX[j+1]), round(valsY[j+1]), 0xF800);
        }
    }
    
}

void plotxsquared(){
    int x,y=0;
    int count = 120;
    
    int prevX[320];
    int prevY[320];
    int counter = 0;
    
    for(x=0; x<320; x++) {
        int shift = 10;
        int power = 2;
        int i = 1;
        
        int y = 1;
        for(;i <= power; i++) {
            y *= (x-160);
        }
        y = 120 - y;

        int plotx = 7*(x-160)+160 + shift; // 5
        
        if(y>0 && y<240 && plotx > 0 && plotx < 320){
            prevX[counter] = plotx;
            prevY[counter] = y;
            counter++;
            
           plot_pixel(plotx,y,0xff00);
        }
    }
    
    int j;
    for(j=0; j < counter-1; j++) {
        draw_line(prevX[j], prevY[j], prevX[j+1], prevY[j+1],0x0000);
    }

}

int main(void){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen();
    //background();
    //plotter();
    //plotxsquared();
    //plotsin();
    volatile short * pixelbuf = 0xc8000000;
    int i, j;
    for (i=0; i<240; i++)
        for (j=0; j<320; j++)
        *(pixelbuf + (j<<0) + (i<<9)) = MYIMAGE[i][j];
   
    while (1);
    return 0;
}

