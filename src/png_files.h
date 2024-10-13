#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <png.h>

using image_t = png_bytep*;

struct Image{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    image_t pixels = nullptr;

    png_structp png;
    png_infop info;
};

Image read_png_file(char *filename) {
    Image out;

    FILE *fp = fopen(filename, "rb");
    
    out.png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!out.png) abort();
    
    out.info = png_create_info_struct(out.png);
    if(!out.info) abort();
    
    if(setjmp(png_jmpbuf(out.png))) abort();
    
    png_init_io(out.png, fp);
    
    png_read_info(out.png, out.info);
    
    out.width      = png_get_image_width(out.png, out.info);
    out.height     = png_get_image_height(out.png, out.info);
    out.color_type = png_get_color_type(out.png, out.info);
    out.bit_depth  = png_get_bit_depth(out.png, out.info);

    printf("width: %d\nheight: %d\nbit_depth: %d\n", out.width, out.height, out.bit_depth);
    
    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt
    
    if(out.bit_depth == 16)
      png_set_strip_16(out.png);
    
    if(out.color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(out.png);
    
    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(out.color_type == PNG_COLOR_TYPE_GRAY && out.bit_depth < 8)
      png_set_expand_gray_1_2_4_to_8(out.png);
    
    if(png_get_valid(out.png, out.info, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(out.png);
    
    // These color_type don't have an alpha channel then fill it with 0xff.
    if(out.color_type == PNG_COLOR_TYPE_RGB ||
       out.color_type == PNG_COLOR_TYPE_GRAY ||
       out.color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_filler(out.png, 0xFF, PNG_FILLER_AFTER);
    
    if(out.color_type == PNG_COLOR_TYPE_GRAY ||
       out.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(out.png);
    
    png_read_update_info(out.png, out.info);

    out.pixels = (png_bytep*)malloc(sizeof(png_bytep) * out.height);
    for(int y = 0; y < out.height; y++)
        out.pixels[y] = (png_byte*)malloc(png_get_rowbytes(out.png,out.info));
    
    png_read_image(out.png, out.pixels);
    
    fclose(fp);

    //png_destroy_write_struct(&png, &info);
    
    return out;
}

void write_png_file(Image const& img, char *filename) {
    int y;

    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
      png,
      info,
      img.width, img.height,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    if (!img.pixels) abort();

    png_write_image(png, img.pixels);
    png_write_end(png, NULL);

    fclose(fp);

    //png_destroy_write_struct(&png, &info);
}

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