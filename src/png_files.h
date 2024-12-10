#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <memory>
#include <png.h>

using image_t = png_bytep*;

class Image {
private:

	void _allocate_pixels(){
		pixels = (png_bytep*)malloc(sizeof(png_bytep) * height);
		for(int y = 0; y < height; y++)
			pixels[y] = (png_byte*)malloc(row_bytes);
	}
public:
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    image_t pixels = nullptr;
	size_t row_bytes;

	// Create copy of this image with other pixel buffer
	void same(Image& other) const {
		other.width = width;
		other.height = height;
		other.color_type = color_type;
		other.bit_depth = bit_depth;
		other.row_bytes = row_bytes;

		other._allocate_pixels();
	}

	Image() {};

    Image(const char* filename){
		read_png_file(filename);
    }

	void read_png_file(const char* filename){
		FILE *fp = fopen(filename, "rb");

		png_structp png;
		png_infop info;
    
		png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png) abort();
		
		info = png_create_info_struct(png);
		if(!info) abort();
		
		if(setjmp(png_jmpbuf(png))) abort();
		
		png_init_io(png, fp);
		
		png_read_info(png, info);
		
		width      = png_get_image_width(png, info);
		height     = png_get_image_height(png, info);
		color_type = png_get_color_type(png, info);
		bit_depth  = png_get_bit_depth(png, info);

		printf("width: %d\nheight: %d\nbit_depth: %d\n", width, height, bit_depth);

		if(bit_depth == 16)
		png_set_strip_16(png);
		
		if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
		
		if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
		
		if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);
		
		if(
			color_type == PNG_COLOR_TYPE_RGB ||
			color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_PALETTE
		) png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
		
		if(
			color_type == PNG_COLOR_TYPE_GRAY ||
			color_type == PNG_COLOR_TYPE_GRAY_ALPHA
		) png_set_gray_to_rgb(png);
		
		png_read_update_info(png, info);

		row_bytes = png_get_rowbytes(png, info);
		_allocate_pixels();
		
		png_read_image(png, pixels);
		
		fclose(fp);

		png_destroy_read_struct(&png, &info, (png_infopp)NULL);
	}

	void write_png_file(const char *filename) const {
		FILE *fp = fopen(filename, "wb");
		if(!fp) abort();


		png_structp png;
		png_infop info;

		png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png) abort();

		info = png_create_info_struct(png);
		if (!info) abort();

		if (setjmp(png_jmpbuf(png))) abort();

		png_init_io(png, fp);

		png_set_IHDR(
			png,
			info,
			width, height,
			8,
			PNG_COLOR_TYPE_RGBA,
			PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT
		);
		png_write_info(png, info);

		if (!pixels) abort();

		png_write_image(png, pixels);
		png_write_end(png, NULL);

		fclose(fp);

		png_destroy_write_struct(&png, &info);
	}

    ~Image(){
		if (pixels) {
			for (int y = 0; y < height; y++)
				free(pixels[y]);
			free(pixels);
		}
    }
};

/*
void process_png_file() {
    for(int y = 0; y < height; y++) {
      png_bytep row = row_pointers[y];
      for(int x = 0; x < width; x++) {
        png_bytep px = &(row[x * 4]);
        // Do something awesome for each pixel here...
        //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
      }
    }
}
*/