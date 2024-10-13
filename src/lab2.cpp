#include "png_files.h"
#include <vector>
#include <algorithm>

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

    void apply(Image& img, int x, int y) {
        png_bytep origin_px = &(img.pixels[y][x * 4]);

        png_byte err_px[4];
        for (int i = 0; i < 4; ++i)
            err_px[i] = origin_px[i] - (origin_px[i] > 128 ? 255 : 0);

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

                png_bytep px = &(img.pixels[_y][_x * 4]);
                for (int i = 0; i < 4; ++i)
                    px[i] += err_px[i] * (m_data[f_y][f_x] / static_cast<float>(m_sum));
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
    Filter filter = default_filter
){
    for (int y = 0; y < img.height; y++)
        for (int x = 0; x < img.width; x++)
            filter.apply(img, x, y);
}

int main(){
    Image out;
    out.width = 1200;
    out.height = 901;

    Image a = read_png_file("test.png");
    
    FloydStainberg(a);

    write_png_file(a, "out_p.png");

    return 0;
}