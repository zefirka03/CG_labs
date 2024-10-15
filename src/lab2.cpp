#include "png_files.h"
#include <vector>
#include <algorithm>
#include <cmath>

#define F_P -1

struct Filter{
private:
    std::vector<std::vector<int>> m_data;
    int m_sum = 0;

    int m_px_x = -1;
    int m_px_y = -1;
public:
    Filter(std::vector<std::vector<int>> const& _data) : m_data(_data) {
        for (int y = 0; y < m_data.size(); ++y) {
            for (int x = 0; x < m_data[y].size(); ++x) {
                if (m_data[y][x] != F_P)
                    m_sum += m_data[y][x];
                else m_px_x = x, m_px_y = y;
            }
        }
    }   

    void apply(
        std::vector<std::vector<int>>& img, 
        int width, int height,
        int x, int y,
        int n
    ) {
        int* origin_px = &(img[y][x * 4]);
        int colors = 1 << n;

        int err_px[4];
        for (int i = 0; i < 4; ++i) {
            int new_px = int(std::round(origin_px[i] / 255.f * (colors - 1)) * (255.f / float(colors - 1)));
            err_px[i] = origin_px[i] - new_px;
            origin_px[i] = new_px;
        }

        for (int f_y = 0; f_y < m_data.size(); ++f_y) {
            for (int f_x = 0; f_x < m_data[f_y].size(); ++f_x) {
                int _x = x + f_x - m_px_x;
                int _y = y + f_y - m_px_y;

                if (
                    _x < 0 || _x >= width ||
                    _y < 0 || _y >= height
                ) continue;

                if (m_data[f_y][f_x] == F_P) 
                    continue;
                
                int* px = &(img[_y][_x * 4]);
                for (int i = 0; i < 4; ++i) 
                    px[i] += err_px[i] * (m_data[f_y][f_x] / float(m_sum));
                
            }
        }
    }
};

Filter default_filter(
    { 
        {0, F_P, 7} ,
        {3, 5, 1}
    }
);

void FloydStainberg(
    Image& img,
    int n,
    Filter filter = default_filter
) {
    std::vector<std::vector<int>> pixels(img.height, std::vector<int>(img.width * 4, 0));
    
    for (int y = 0; y < img.height; y++) 
        for (int x = 0; x < img.width * 4; x++) 
            pixels[y][x] = static_cast<int>(img.pixels[y][x]);

    int normals[4] = { 0, 0, 0, 0 };
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width * 4; x++) {
            filter.apply(pixels, img.width, img.height, x / 4, y, n);
            normals[x % 4] = std::max(normals[x % 4], pixels[y][x]);
        }
    }

    for (int y = 0; y < img.height; y++) 
        for (int x = 0; x < img.width * 4; x++) 
            pixels[y][x] = std::round((pixels[y][x] / float(normals[x % 4])) * 255);

    for (int y = 0; y < img.height; y++)
        for (int x = 0; x < img.width * 4; x++)
            img.pixels[y][x] = pixels[y][x];
}

int main(){
    {
        Image a("img/capy.png");
        FloydStainberg(a, 1);
        a.write_png_file("img/capy_1.png");
    }
    {
        Image a("img/capy.png");
        FloydStainberg(a, 2);
        a.write_png_file("img/capy_2.png");
    }
    {
        Image a("img/capy.png");
        FloydStainberg(a, 4);
        a.write_png_file("img/capy_4.png");
    }
    {
        Image a("img/capy.png");
        FloydStainberg(a, 8);
        a.write_png_file("img/capy_8.png");
    }



    return 0;
}