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

class KMeans {
private:
    int k;
    int max_iterations;

public:
    KMeans(int k, int max_iterations = 100) : k(k), max_iterations(max_iterations) {
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
        std::vector<Color<int>> new_centroids(k, { 0, 0, 0, 255 }); 
        std::vector<Color_8> new_centroids_out(k); 
        std::vector<int> counts(k, 0);

        for (size_t i = 0; i < colors.size(); ++i) {
            int label = labels[i];
            new_centroids[label].r += colors[i].r;
            new_centroids[label].g += colors[i].g;
            new_centroids[label].b += colors[i].b;
            new_centroids[label].a = colors[i].a;
            ++counts[label];
        }

        for (int i = 0; i < k; ++i) {
            if (counts[i] > 0) {
                new_centroids_out[i].r = new_centroids[i].r / counts[i];
                new_centroids_out[i].g = new_centroids[i].g / counts[i];
                new_centroids_out[i].b = new_centroids[i].b / counts[i];
                new_centroids_out[i].a = new_centroids[i].a;
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

    KMeans kmeans(k, 50);
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
        quantize_image(a, 2);
        a.write_png_file("img/eifel_2.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/eifel_4.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/eifel_12.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/eifel_24.png");
    }

    {
        Image a("img/capy.png");
        quantize_image(a, 2);
        a.write_png_file("img/capy_2.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/capy_4.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/capy_12.png");
    }
    {
        Image a("img/eifel.png");
        quantize_image(a, 24);
        a.write_png_file("img/capy_24.png");
    }

    return 0;
}