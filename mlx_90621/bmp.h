#ifndef BMP_H
#define BMP_H

typedef struct {
	unsigned char r, g, b;
}RGB_DATA;

void save_bitmap(char* file_name, int width, int height, int dpi, RGB_DATA** pixel_data);
void generateBMP();
void generateGradient();
void generateFields( float** values, int rows_count, int columns_count,int cell_width, int cell_height, int padding);
#endif /* BMP_H */