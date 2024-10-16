#include "png_files.h"
#include <vector>
#include <algorithm>
#include <cmath>

#define F_P -1

#define RGB 3
#define RGBA 4

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
        Image& img,
        int x, int y,
        int n,
        int channels = RGB
    ) const {
        auto origin_px = &(img.pixels[y][x * 4]);
        int colors = 1 << n;

        int err_px[4];
        for (int i = 0; i < channels; ++i) {
            int new_px = int(std::round(origin_px[i] / 255.f * (colors - 1)) * (255.f / float(colors - 1)));
            err_px[i] = origin_px[i] - new_px;
            origin_px[i] = new_px;
        }

        for (int f_y = 0; f_y < m_data.size(); ++f_y) {
            for (int f_x = 0; f_x < m_data[f_y].size(); ++f_x) {
                int _x = x + f_x - m_px_x;
                int _y = y + f_y - m_px_y;

                if (
                    _x < 0 || _x >= img.width ||
                    _y < 0 || _y >= img.height
                ) continue;

                if (m_data[f_y][f_x] == F_P) 
                    continue;
                
                auto px = &(img.pixels[_y][_x * 4]);
                for (int i = 0; i < channels; ++i)
                    px[i] = std::min(std::max(0, int(px[i]) + int(err_px[i] * (m_data[f_y][f_x] / float(m_sum)))), 255);
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
    Filter filter = default_filter,
    int channels = RGB
) {
    for (int y = 0; y < img.height; y++) 
        for (int x = 0; x < img.width; x++) 
            filter.apply(img, x, y, n);
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