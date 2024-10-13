#include <algorithm>
#include "png_files.h"

#define ERROR 0
#define OK 1

using blend_func_t = png_byte(*)(png_byte, png_byte, png_byte);
    
png_byte default_blend_func(png_byte a, png_byte b, png_byte alpha) {
    return a * (alpha / 255.f) + b * (1.f - alpha / 255.f);
}

int blend(
    Image const& a,
    Image const& b,
    Image const& mask,
    Image& out,
    blend_func_t blend_func = default_blend_func
){
    if(
        a.height != b.height ||
        a.width != b.width ||
        a.height != mask.height ||
        a.width != mask.width
    ) return ERROR;

    if (!out.pixels) {
        out.pixels = (png_bytep*)malloc(sizeof(png_bytep) * out.height);
        for (int y = 0; y < out.height; y++)
            out.pixels[y] = (png_byte*)malloc(png_get_rowbytes(a.png, a.info));
    }

    for(int y = 0; y < a.height; y++) {
        png_bytep row_a = a.pixels[y];
        png_bytep row_b = b.pixels[y];
        png_bytep row_mask = mask.pixels[y];
        png_bytep row_out = out.pixels[y];

        for(int x = 0; x < a.width; x++) {
            png_bytep px_a = &(row_a[x * 4]);
            png_bytep px_b = &(row_b[x * 4]);
            png_bytep px_mask = &(row_mask[x * 4]);
            png_bytep px_out = &(row_out[x * 4]);

            for(int i = 0; i < 4; ++i)
                px_out[i] = blend_func(px_a[i], px_b[i], px_mask[3]);
        }
    }

    return OK;
}

int circle_image(
    Image const& a,
    Image& out
) {
    int radius = std::min(a.width, a.height) / 2;

    if (!out.pixels) {
        out.pixels = (png_bytep*)malloc(sizeof(png_bytep) * out.height);
        for (int y = 0; y < out.height; y++)
            out.pixels[y] = (png_byte*)malloc(png_get_rowbytes(a.png, a.info));
    }

    int center_w = a.width / 2;
    int center_h = a.height / 2;

    for (int y = 0; y < a.height; y++) {
        png_bytep row_a = a.pixels[y];
        png_bytep row_out = out.pixels[y];

        for (int x = 0; x < a.width; x++) {
            png_bytep px_a = &(row_a[x * 4]);
            png_bytep px_out = &(row_out[x * 4]);

            if ((x - center_w) * (x - center_w) + (y - center_h) * (y - center_h) <= radius * radius) {
                for (int i = 0; i < 4; ++i)
                    px_out[i] = px_a[i];
            } else {
                for (int i = 0; i < 4; ++i)
                    px_out[i] = 0;
            }
        }
    }

    return OK;
}

int main(){
    Image out;
    out.width = 1200;
    out.height = 901;

    Image a = read_png_file("file1.png");
    Image b = read_png_file("file2.png");
    Image mask = read_png_file("file3.png");

    blend(a, b, mask, out);
    write_png_file(out, "out.png");

    circle_image(a, out);
    write_png_file(out, "out2.png");

    return 0;
}