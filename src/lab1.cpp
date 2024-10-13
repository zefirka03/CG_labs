#include <iostream>
#include "png_files.h"

int main(){
    auto point = read_png_file("file.png");

    for(int y = 0; y < height; y++) {
        png_bytep row = point[y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            // Do something awesome for each pixel here...
            //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
            px[0] = 0;
        }
    }
    write_png_file(point, "file2.png");

    return 0;
}