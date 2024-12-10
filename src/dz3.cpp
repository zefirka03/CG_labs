#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include "png_files.h"

template<class T = unsigned char>
struct Color {
    T r, g, b, a;

    double distance(const Color& other) const {
        return std::sqrt(static_cast<double>((r - other.r) * (r - other.r) +
            (g - other.g) * (g - other.g) +
            (b - other.b) * (b - other.b)));
    }

    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
};

using Color_8 = Color<unsigned char>;

class KMedians {
private:
    int k;
    int max_iterations;

public:
    KMedians(int k, int max_iterations = 100) : k(k), max_iterations(max_iterations) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    std::vector<Color_8> quantize(const std::vector<Color_8>& colors) {
        auto centroids = _initialize_centroids(colors);
        std::vector<int> labels(colors.size());

        for (int iteration = 0; iteration < max_iterations; ++iteration) {
            for (size_t i = 0; i < colors.size(); ++i) {
                labels[i] = nearest_centroid(colors[i], centroids);
            }

            auto new_centroids = _update_centroids(colors, labels);
            if (centroids == new_centroids) {
                break; 
            }
            centroids = std::move(new_centroids);
        }

        return centroids;
    }

    int nearest_centroid(const Color_8& color, const std::vector<Color_8>& centroids) {
        int nearest = 0;
        double min_distance = color.distance(centroids[0]);

        for (int i = 1; i < centroids.size(); ++i) {
            double distance = color.distance(centroids[i]);
            if (distance < min_distance) {
                min_distance = distance;
                nearest = i;
            }
        }
        return nearest;
    }

private:
    std::vector<Color_8> _initialize_centroids(const std::vector<Color_8>& colors) {
        std::vector<Color_8> centroids;
        std::sample(colors.begin(), colors.end(), std::back_inserter(centroids), k, std::mt19937{ std::random_device{}() });
        return centroids;
    }

    std::vector<Color_8> _update_centroids(const std::vector<Color_8>& colors, const std::vector<int>& labels) {
        std::vector<std::vector<Color_8>> color_groups(k);

        for (size_t i = 0; i < colors.size(); ++i) {
            int label = labels[i];
            color_groups[label].push_back(colors[i]);
        }

        std::vector<Color_8> new_centroids_out(k);

        for (int i = 0; i < k; ++i) {
            if (!color_groups[i].empty()) {
                std::vector<int> r_values, g_values, b_values;
                for (const auto& color : color_groups[i]) {
                    r_values.push_back(color.r);
                    g_values.push_back(color.g);
                    b_values.push_back(color.b);
                }

                std::sort(r_values.begin(), r_values.end());
                std::sort(g_values.begin(), g_values.end());
                std::sort(b_values.begin(), b_values.end());

                size_t n = r_values.size();
                new_centroids_out[i].r = (n % 2 == 0) ? (r_values[n / 2 - 1] + r_values[n / 2]) / 2 : r_values[n / 2];
                new_centroids_out[i].g = (n % 2 == 0) ? (g_values[n / 2 - 1] + g_values[n / 2]) / 2 : g_values[n / 2];
                new_centroids_out[i].b = (n % 2 == 0) ? (b_values[n / 2 - 1] + b_values[n / 2]) / 2 : b_values[n / 2];
                new_centroids_out[i].a = color_groups[i][0].a;
            }
        }

        return new_centroids_out;
    }
};


void quantize_image(Image& img, int k) {
    std::vector<Color_8> colors(img.width * img.height);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            png_byte* pixel = img.pixels[y] + x * 4;
            colors[y * img.width + x] = { pixel[0], pixel[1], pixel[2], pixel[3] };
        }
    }

    KMedians kmeans(k, 50);
    auto centroids = kmeans.quantize(colors);

    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            png_byte* pixel = img.pixels[y] + x * 4;
            int nearest = kmeans.nearest_centroid(colors[y * img.width + x], centroids);
            pixel[0] = centroids[nearest].r;
            pixel[1] = centroids[nearest].g;
            pixel[2] = centroids[nearest].b;
            pixel[3] = centroids[nearest].a;
        }
    }
}

int main() {
    {
        Image a("img/eifel.png");
        quantize_image(a, 256);
        a.write_png_file("img/eifel_256.png");
    }
    

    return 0;
}