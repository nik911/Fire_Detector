#define _GNU_SOURCE 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "bmp.h"

#define DEBUG 0

//#define WIDTH 800
//#define HEIGHT 200

void save_bitmap(char* file_name, int width, int height, int dpi, RGB_DATA** pixel_data) {
	// create a file object that we will use to write our image
	FILE *image;
	// we want to know how many pixels to reserve
	int image_size = width * height;
	// a byte is 4 bits but we are creating a 24 bit image so we can represent a pixel with 3
	// our final file size of our image is the width * height * 4 + size of bitmap header
	int file_size = 54 + 4 * image_size;
	// pixels per meter https://www.wikiwand.com/en/Dots_per_inch
	int ppm = dpi * 39.375;

	// bitmap file header (14 bytes)
	// we could be savages and just create 2 array but since this is for learning lets
	// use structs so it can be parsed by someone without having to refer to the spec

	// since we have a non-natural set of bytes, we must explicitly tell the
	// compiler to not pad anything, on gcc the attribute alone doesn't work so
	// a nifty trick is if we declare the smallest data type last the compiler
	// *might* ignore padding, in some cases we can use a pragma or gcc's
	// __attribute__((__packed__)) when declaring the struct
	// we do this so we can have an accurate sizeof() which should be 14, however
	// this won't work here since we need to order the bytes as they are written
	struct bitmap_file_header {
	    unsigned char   bitmap_type[2];     // 2 bytes
	    int             file_size;          // 4 bytes
	    short           reserved1;          // 2 bytes
	    short           reserved2;          // 2 bytes
	    unsigned int    offset_bits;        // 4 bytes
	} bfh;

	// bitmap image header (40 bytes)
	struct bitmap_image_header {
	    unsigned int    size_header;        // 4 bytes
	    unsigned int    width;              // 4 bytes
	    unsigned int    height;             // 4 bytes
	    short int       planes;             // 2 bytes
	    short int       bit_count;          // 2 bytes
	    unsigned int    compression;        // 4 bytes
	    unsigned int    image_size;         // 4 bytes
	    unsigned int    ppm_x;              // 4 bytes
	    unsigned int    ppm_y;              // 4 bytes
	    unsigned int    clr_used;           // 4 bytes
	    unsigned int    clr_important;      // 4 bytes
	} bih;

	// if you are on Windows you can include <windows.h>
	// and make use of the BITMAPFILEHEADER and BITMAPINFOHEADER structs

	memcpy(&bfh.bitmap_type, "BM", 2);
	bfh.file_size       = file_size;
	bfh.reserved1       = 0;
	bfh.reserved2       = 0;
	bfh.offset_bits     = 0;

	bih.size_header     = sizeof(bih);
	bih.width           = width;
	bih.height          = height;
	bih.planes          = 1;
	bih.bit_count       = 24;
	bih.compression     = 0;
	bih.image_size      = file_size;
	bih.ppm_x           = ppm;
	bih.ppm_y           = ppm;
	bih.clr_used        = 0;
	bih.clr_important   = 0;

	image = fopen(file_name, "wb");
	// compiler woes so we will just use the constant 14 we know we have
	fwrite(&bfh, 1, 14, image);
	fwrite(&bih, 1, sizeof(bih), image);

	int index = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			RGB_DATA BGR = pixel_data[i][j];

			unsigned char red   = (BGR.r);
	   		unsigned char green = (BGR.g);
	   		unsigned char blue  = (BGR.b);

	   		unsigned char color[3] = {
	       		blue, green, red
	   		};

	   		fwrite(color, 1, sizeof(color), image);
		}
	}
	
	fclose(image);
}

void generateBMP() {

	int columns_count  = 300;
	int rows_count = 300;
	int dpi = 96;

	int rows_counter = 0;

	RGB_DATA **pixels = NULL;
	RGB_DATA **temp = (RGB_DATA**) malloc(sizeof(RGB_DATA*) * rows_count);
	if (temp == NULL) {
		if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
	} else { 
		pixels = temp;
	}
	while (rows_counter != rows_count) {
		RGB_DATA *temp = (RGB_DATA*) calloc(columns_count, sizeof(RGB_DATA));
		if (temp == NULL) {
			if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
		} else { 
			pixels[rows_counter] = temp;
		}
		rows_counter++;
	}

	for (int x = 0; x < rows_count; x++) {
	    for (int y = 0; y < columns_count; y++) {
	        //int a = y * width + x;

	        if ((x > 50 && x < 350) && (y > 50 && y < 350)) {
	            pixels[x][y].r = 255;
	            pixels[x][y].g = 255;
	            pixels[x][y].b = 5;
	        } else {
	            pixels[x][y].r = 55;
	            pixels[x][y].g = 55;
	            pixels[x][y].b = 55;
	        }
	    }
	}

	save_bitmap("black_border.bmp", columns_count, rows_count, dpi, pixels);

  rows_counter = rows_count;
  while (rows_counter--) {
    if(pixels[rows_counter]) free(pixels[rows_counter]); 
  }
  if (pixels) free(pixels);
}

void generateGradient() {
	if (DEBUG) fprintf(stderr, "Generate gradient\n");
	int columns_count  = 1024;
	int rows_count = 200;
	int dpi = 96;

	int rows_counter = 0;

	RGB_DATA **pixels = NULL;
	RGB_DATA **temp = (RGB_DATA**) malloc(sizeof(RGB_DATA*) * rows_count);
	if (temp == NULL) {
		if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
	} else { 
		pixels = temp;
	}
	while (rows_counter != rows_count) {
		RGB_DATA *temp = (RGB_DATA*) calloc(columns_count, sizeof(RGB_DATA));
		if (temp == NULL) {
			if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
		} else { 
			pixels[rows_counter] = temp;
		}
		rows_counter++;
	}

	RGB_DATA data;
	int stage = 0;

	for (int x = 0; x < rows_count; x++) {
		data.r = 0;
		data.g = 0;
		data.b = 255;
		stage = 0;
	    for (int y = 0; y < columns_count; y++) {
	        //if ((x > 5 && x < rows_count - 5) && (y > 5 && y < columns_count - 5)) {

	        	// pixels[x][y].r = 255;
	         //    pixels[x][y].g = 255;
	         //    pixels[x][y].b = 5;
				switch(stage) {
					case 0: // -20..0
						data.g++;
						if(data.g == 255) stage++;
						break;
					case 1: // 0..100
						data.b--;
						if(!data.b) stage++;
						break;
					case 2: // 
						data.r++;
						if(data.r == 255) stage++;
						break;
					case 3:
						data.g--;
						if(!data.g) stage++;
						break;
				}
	        	pixels[x][y] = data;
	        // } else {
	        // 	pixels[x][y].r = 55;
	        //     pixels[x][y].g = 55;
	        //     pixels[x][y].b = 55;
	        // }
	    }
	}
	save_bitmap("gradient.bmp", columns_count, rows_count, dpi, pixels);

  rows_counter = rows_count;
  while (rows_counter--) {
    if(pixels[rows_counter]) free(pixels[rows_counter]); 
  }
  if (pixels) free(pixels);
}

void generateFields( float** values, int rows_count, int columns_count, int cell_width, int cell_height, int padding) {
	if (DEBUG) fprintf(stderr, "Generate temperature map file\n");

	int width = columns_count * (cell_height+padding*2);
	int height = rows_count * (cell_width+padding*2);
	int dpi = 96;

	int rows_counter = 0;

	RGB_DATA **pixels = NULL;
	RGB_DATA **temp = (RGB_DATA**) malloc(sizeof(RGB_DATA*) * height);
	if (temp == NULL) {
		if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
	} else { 
		pixels = temp;
	}
	while (rows_counter != height) {
		RGB_DATA *temp = (RGB_DATA*) calloc(width, sizeof(RGB_DATA));
		if (temp == NULL) {
			if (DEBUG) fprintf(stderr, "Cannot allocate more memory.\n");
		} else { 
			pixels[rows_counter] = temp;
		}
		rows_counter++;
	}

	RGB_DATA background_color = { .r = 55, .g = 55, .b = 55};

	// values[0][0] = -20;		values[0][1] = -15;		values[0][2] = -10;		values[0][3] = -5;
	// values[0][4] = 0; 		values[0][5] = 5;		values[0][6] = 10;		values[0][7] = 15;
	// values[0][8] = 20;		values[0][9] = 25; 		values[0][10] = 30;		values[0][11] = 35;
	// values[0][12] = 40;	 	values[0][13] = 45;		values[0][14] = 50;		values[0][15] = 55;

	// values[1][0] = 60;		values[1][1] = 65;		values[1][2] = 70;		values[1][3] = 75;
	// values[1][4] = 80; 		values[1][5] = 85;		values[1][6] = 90;		values[1][7] = 95;
	// values[1][8] = 100;		values[1][9] = 105;		values[1][10] = 110;	values[1][11] = 125;
	// values[1][12] = 130;	values[1][13] = 135;	values[1][14] = 140;	values[1][15] = 145;

	// values[2][0] = 150;		values[2][1] = 155;		values[2][2] = 160;		values[2][3] = 165;
	// values[2][4] = 170; 	values[2][5] = 180;		values[2][6] = 185;		values[2][7] = 190;
	// values[2][8] = 195;		values[2][9] = 200;		values[2][10] = 205;	values[2][11] = 210;
	// values[2][12] = 215;	values[2][13] = 220;	values[2][14] = 225;	values[2][15] = 230;

	// values[3][0] = 235;		values[3][1] = 240;		values[3][2] = 245;		values[3][3] = 250;
	// values[3][4] = 260; 	values[3][5] = 265;		values[3][6] = 270;		values[3][7] = 275;
	// values[3][8] = 280;		values[3][9] = 285;		values[3][10] = 290;	values[3][11] = 295;
	// values[3][12] = 300;	values[3][13] = 305;	values[3][14] = 310;	values[3][15] = 315;


	int swapRowsIndex = 0;
	int index_y = 1;
	for (int y = 0; y < height; y++) {
		int index_x = 1;

		for (int x = 0; x < width; x++) {
			if (x > index_x*(cell_width+padding*2)) index_x++;
			if (y > index_y*(cell_height+padding*2)) index_y++;
			if (x > padding + (index_x-1)*(cell_width+padding*2) && x < padding + cell_width + (index_x-1)*(cell_width+padding*2) && y > padding + (index_y-1)*(cell_height+padding*2) && y < padding + cell_height + (index_y-1)*(cell_height+padding*2) ) {
				swapRowsIndex = rows_count - index_y;
				if (values[swapRowsIndex][index_x-1] >= 200.0) {
					if(values[swapRowsIndex][index_x-1] > 300.0) values[swapRowsIndex][index_x-1] = 300.0;
					RGB_DATA cell_color = { .r = 255, .g = 0, .b = 0};
					cell_color.b = (unsigned char) (values[swapRowsIndex][index_x-1] - 200)*255/100;
					pixels[y][x] = cell_color;
				} else if (values[swapRowsIndex][index_x-1] >= 100.0) {
					RGB_DATA cell_color = { .r = 255, .g = 255, .b = 0};
					cell_color.g = (unsigned char) (cell_color.g - (values[swapRowsIndex][index_x-1] - 100)*255/100);
					pixels[y][x] = cell_color;
				} else if (values[swapRowsIndex][index_x-1] >= 25.0) {
					RGB_DATA cell_color = { .r = 0, .g = 255, .b = 0};
					cell_color.r = (unsigned char) (values[swapRowsIndex][index_x-1] - 25)*255/75;
					pixels[y][x] = cell_color;
				} else  if (values[swapRowsIndex][index_x-1] >= 0.0) {
					RGB_DATA cell_color = { .r = 0, .g = 255, .b = 255};
					cell_color.b = (unsigned char) (cell_color.b - values[swapRowsIndex][index_x-1]*255/25);
					//if (DEBUG) fprintf(stderr, "index x,y [%d, %d] value %d\n", index_y, index_x, cell_color.b);
					pixels[y][x] = cell_color;
				} else if (values[swapRowsIndex][index_x-1] >= -20.0) {
					RGB_DATA cell_color = { .r = 0, .g = 0, .b = 255};
					cell_color.g = (unsigned char) (values[swapRowsIndex][index_x-1] + 20)*255/20;
					pixels[y][x] = cell_color;
				} else { pixels[y][x] = background_color; } 
			} else {
				pixels[y][x] = background_color;
			}
		}
	}

	char* filename = NULL;
	char buffer[26];
	time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", timeinfo);

    //asprintf(&filename, "./event/%s_%s", buffer, "temperatures.bmp");
    asprintf(&filename, "./event/%s", "temperatures.bmp");

   	save_bitmap(filename, width, height, dpi, pixels);

	free(filename);

	rows_counter = height;
	while (rows_counter--) {
		if(pixels[rows_counter]) free(pixels[rows_counter]); 
	}
	if (pixels) free(pixels);
}

int getCellIndex(int rows_count, int columns_count, int cell_width, int cell_height, int padding) {
	int index = 0;

	return index;
}

