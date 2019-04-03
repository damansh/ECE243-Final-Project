#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "address_map_arm.h"

#define PI 3.14159265
#define e  2.71828

volatile int pixel_buffer_start; // global variable
//extern short MYIMAGE [240][320];

int characters[16][2] = {
	{0x45, '0'},
	{0x16, '1'},
	{0x1E, '2'},
	{0x26, '3'},
	{0x25, '4'},
	{0x2E, '5'},
	{0x36, '6'},
	{0x3d, '7'},
	{0x3E, '8'},
	{0x46, '9'},
	{0x22, 'x'},
	{0x4A, '/'},
	{0x79, '+'},
	{0x76, '-'},
	{0x5A, 'r'}, // Enter key
	{0x7C, '*'},
};


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

void plote(){
    int x,y = 0;
    int count = 120;
    int prevX[320];
    int prevY[320];
    int counter = 0;

    for(x=0; x<320; x++) {

        y = (120 - pow(e, x-160)); // Works, but gives a clobbered register error in CPUlator

        double plotx = 4.5*(x-160)+160; // 5

        if(y>0 && y<240 && plotx > 0 && plotx < 320){
            prevX[counter] = round(plotx);
            prevY[counter] = y;
            counter++;

            plot_pixel(round(plotx),y,0xff00);
        }
    }

    int j;
    for(j=0; j < counter-1; j++) {
        draw_line(prevX[j], prevY[j], prevX[j+1], prevY[j+1],0x0000);
    }

}

void plotx(int power, int shiftx, int shifty){
    int x,y=0;
    int count = 120;
    
    int prevX[320];
    int prevY[320];
    int counter = 0;
    
    for(x=0; x<320; x++) {
        double xin = (x-160.0)/16.0;
        
        int y = (120 - 12*pow(xin, power)) - shifty*12; // Works, but gives a clobbered register error in CPUlator
        
         int plotx = (x)+shiftx*16;
        
        if(y>0 && y<240 && plotx > 0 && plotx < 320){
            prevX[counter] = round(plotx);
            prevY[counter] = y;
            counter++;
            
           plot_pixel(round(plotx),y,0xff00);
        }
    }
    
    int j;
    for(j=0; j < counter-1; j++) {
        draw_line(prevX[j], prevY[j], prevX[j+1], prevY[j+1],0x0000);
    }

}

void check_KEYs (int * option) {
    volatile int * KEY_ptr = (int *)KEY_BASE;
    int KEY_value;
    
    KEY_value = *(KEY_ptr);
    
    if (KEY_value == 0x1)  {
        *option = 1;
    } else if (KEY_value == 0x2) {
        *option = 2;
    } else if(KEY_value == 0x4) {
        *option = 3;
    } else {
        *option = 0;
    }
}

/*
void load_screen (){
   volatile short * pixelbuf = 0xc8000000;
   int i, j;
   for (i=0; i<240; i++)
   for (j=0; j<320; j++)
   *(pixelbuf + (j<<0) + (i<<9)) = MYIMAGE[i][j];
   //while (1);
} */

char HEX_PS2(char b1, char b2, char b3){

    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    unsigned char seven_seg_decode_table[] = {
            0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
            0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int shift_buffer, nibble;
    unsigned char code;
    int i;
    shift_buffer = b3; //(b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i) {
        nibble = shift_buffer & 0x0000000F;
        code = seven_seg_decode_table[nibble];
        hex_segs[i] = code;
        shift_buffer = shift_buffer >> 4;
    }
	
	int j;
	char returnedChar;
	for (j = 0; j < 17; j++) {
		if(characters[j][0] == b3) {
			returnedChar = characters[j][1];
		}
	}
	
	
    if (returnedChar == 'x')
        plotsin();
    if (b1 == 'b')
        plote();

    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}

int main(void){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
    
    int option;
    
    clear_screen();
    background();
    //plote();
	
    
	  volatile int * PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID;
  char byte1 = 0, byte2 = 0, byte3 = 0;

  *(PS2_ptr) = 0xFF; // reset
  while (1) {
  PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
  RVALID = PS2_data & 0x8000; // extract the RVALID field 
  if (RVALID) {

   byte1 = byte2; 
   byte2 = byte3;
   byte3 = PS2_data & 0xFF;
   HEX_PS2(byte1, byte2, byte3);
   if ((byte2 == (char)0xAA) && (byte3 == (char)0x00))
     *(PS2_ptr) = 0xF4;
  }
 }
     
	
	
    while(true) {
        check_KEYs(&option);
        
        if(option == 1) {
            //load_screen();
        } else if(option == 2) {
            //background();
            plotx(3, 0, 0);
        } else if(option == 3) {
            plotsin();
        }
        
    }
    
    //plotter();
    plotx(2, 2, 2);
    //plotsin();
    

    return 0;
}

/*
  volatile int * PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID;
  char byte1 = 0, byte2 = 0, byte3 = 0;

  *(PS2_ptr) = 0xFF; // reset
  while (1) {
  PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
  RVALID = PS2_data & 0x8000; // extract the RVALID field 
  if (RVALID) {

   byte1 = byte2; 
   byte2 = byte3;
   byte3 = PS2_data & 0xFF;
   HEX_PS2(byte1, byte2, byte3);
   if ((byte2 == (char)0xAA) && (byte3 == (char)0x00))
     *(PS2_ptr) = 0xF4;
  }
 }

*/



/*
 volatile short * pixelbuf = 0xc8000000;
 int i, j;
 for (i=0; i<240; i++)
 for (j=0; j<320; j++)
 *(pixelbuf + (j<<0) + (i<<9)) = MYIMAGE[i][j];
 
 while (1);
 */
