#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

const double PI = 3.1415926535897932;

SDL_Renderer* renderer;

struct Point3D {
    float x, y, z;

    Point3D operator+(const Point3D& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    Point3D operator-(const Point3D& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    Point3D cross(const Point3D& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    float dot(const Point3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};

void drawPixel(int x, int y, SDL_Color color = { 255, 255, 255, 255 }) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(renderer, x, y);
}

void drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    int dx = x2 - x1, dy = y2 - y1;
    int ix = (dx > 0) - (dx < 0);
    int iy = (dy > 0) - (dy < 0);
    dx = std::abs(dx);
    dy = std::abs(dy);

    if (dx >= dy) {
        int e = 2 * dy - dx;
        for (int x = x1, y = y1, i = 0; i <= dx; ++i) {
            drawPixel(x, y, color);
            if (e >= 0) {
                y += iy;
                e -= 2 * dx;
            }
            x += ix;
            e += 2 * dy;
        }
    }
    else {
        int e = 2 * dx - dy;
        for (int x = x1, y = y1, i = 0; i <= dy; ++i) {
            drawPixel(x, y, color);
            if (e >= 0) {
                x += ix;
                e -= 2 * dy;
            }
            y += iy;
            e += 2 * dx;
        }
    }
}

std::vector<std::vector<Point3D>> transform(const std::vector<std::vector<Point3D>>& faces, const std::vector<std::vector<double>>& T) {
    std::vector<std::vector<Point3D>> result;

    for (const auto& face : faces) {
        std::vector<Point3D> transformedFace;
        for (const auto& p : face) {
            double x_new = p.x * T[0][0] + p.y * T[1][0] + p.z * T[2][0] + T[3][0];
            double y_new = p.x * T[0][1] + p.y * T[1][1] + p.z * T[2][1] + T[3][1];
            double z_new = p.x * T[0][2] + p.y * T[1][2] + p.z * T[2][2] + T[3][2];
            double H = p.x * T[0][3] + p.y * T[1][3] + p.z * T[2][3] + T[3][3];

            if (H == 0) H = 1;

            transformedFace.push_back({ static_cast<float>(x_new / H), static_cast<float>(y_new / H), static_cast<float>(z_new / H) });
        }
        result.push_back(transformedFace);
    }

    return result;
}

std::vector<std::vector<Point3D>> resize(const std::vector<std::vector<Point3D>>& faces, double scale) {
    std::vector<std::vector<double>> T = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1 / scale},
    };

    return transform(faces, T);
}

std::vector<std::vector<Point3D>> translate(const std::vector<std::vector<Point3D>>& faces, double tx, double ty, double tz) {
    std::vector<std::vector<double>> T = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {tx, ty, tz, 1},
    };

    return transform(faces, T);
}

std::vector<std::vector<Point3D>> getDefaultCube() {
    std::vector<std::vector<Point3D>> faces = {
        { {1, 0, 1}, {1, 1, 1},  {0, 1, 1}, {0, 0, 1} },
        { {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0} },
        { {1, 0, 1}, {0, 0, 1}, {0, 0, 0}, {1, 0, 0} },
        { {1, 1, 0}, {0, 1, 0}, {0, 1, 1}, {1, 1, 1} },
        { {0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0} },
        { {1, 1, 1}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0} },
    };

    faces = resize(faces, 200);
    return translate(faces, 200, 200, 200);
}

void drawPolygon(const std::vector<Point3D>& points) {
    for (size_t i = 0; i < points.size(); ++i) {
        drawLine(points[i].x, points[i].y, points[(i + 1) % points.size()].x, points[(i + 1) % points.size()].y, { 255, 255, 255, 255 });
    }
}

std::vector<std::vector<Point3D>> projectTo2D(const std::vector<std::vector<Point3D>>& faces) {
    std::vector<std::vector<Point3D>> result;
    for (const auto& face : faces) {
        std::vector<Point3D> projectedFace;
        for (const auto& p : face) {
            projectedFace.push_back({ p.x, p.y, 0 });
        }
        result.push_back(projectedFace);
    }
    return result;
}

void drawCube(const std::vector<std::vector<Point3D>>& faces) {
    auto faces2D = projectTo2D(faces);
    for (const auto& face : faces2D) {
        drawPolygon(face);
    }
}

void applyXYProjection(const std::vector<std::vector<Point3D>>& faces) {
    std::vector<std::vector<double>> T = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 1},
    };
    auto new_faces = transform(faces, T);
    drawCube(new_faces);
}

std::vector<std::vector<Point3D>> rotateFaces(const std::vector<std::vector<Point3D>>& faces, const Point3D& point, double angle) {
    double norm = std::sqrt(point.x * point.x + point.y * point.y + point.z * point.z);
    Point3D n = { point.x / norm, point.y / norm, point.z / norm };

    std::vector<std::vector<double>> T = {
        {cos(angle) + n.x * n.x * (1 - cos(angle)), n.x * n.y * (1 - cos(angle)) + n.z * sin(angle), n.x * n.z * (1 - cos(angle)) - n.y * sin(angle), 0},
        {n.x * n.y * (1 - cos(angle)) - n.z * sin(angle), cos(angle) + n.y * n.y * (1 - cos(angle)), n.y * n.z * (1 - cos(angle)) + n.x * sin(angle), 0},
        {n.x * n.z * (1 - cos(angle)) + n.y * sin(angle), n.y * n.z * (1 - cos(angle)) - n.x * sin(angle), cos(angle) + n.z * n.z * (1 - cos(angle)), 0},
        {0, 0, 0, 1}
    };

    return transform(faces, T);
}

void translateAndDraw(const std::vector<std::vector<Point3D>>& faces, double r) {
    std::vector<std::vector<double>> T = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, r},
        {0, 0, 0, 1},
    };

    auto new_faces = transform(faces, T);
    drawCube(new_faces);
}

void Scene_transformPoint(double r) {
    auto faces = getDefaultCube();
    translateAndDraw(faces, r);
}

void Scene_rotateXYProjection(double angle) {
    auto faces = getDefaultCube();
    faces = rotateFaces(faces, { 10, 10, 10 }, angle);
    applyXYProjection(faces);
}

void Scene_rotatePointProjection(double r, double angle) {
    auto faces = getDefaultCube();
    faces = rotateFaces(faces, { 10, 10, 10 }, angle);
    translateAndDraw(faces, r);
}

std::vector<std::vector<Point3D>> deleteBackFaces(const std::vector<std::vector<Point3D>>& faces) {
    std::vector<std::vector<Point3D>> result;
    for (const auto& face : faces) {
        if (face.size() < 3) continue;

        Point3D v1 = face[0], v2 = face[1], v3 = face[2];
        Point3D u = v2 - v1;
        Point3D w = v3 - v1;

        Point3D n = u.cross(w);
        if (n.z < 0) {
            result.push_back(face);
        }
    }
    return result;
}

void Scene_deleteFaces() {
    auto faces = getDefaultCube();
    faces = rotateFaces(faces, { 10, 10, 10 }, PI / 4);
    faces = deleteBackFaces(faces);
    applyXYProjection(faces);
}

void initSDL(SDL_Window*& window) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL :(");
    }
    window = SDL_CreateWindow("Cube", 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Quit();
        throw std::runtime_error("Failed to create window :(");
    }
}

void cleanupSDL(SDL_Window* window) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    try {
        initSDL(window);
        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Failed to create renderer :(");
        }

        bool running = true;
        SDL_Event event;
        Uint64 LAST = 0;
        Uint64 NOW = SDL_GetPerformanceCounter();
        double deltaTime = 0;
        double angle = 0;

        int scene = 4;

        while (running) {
            LAST = NOW;
            NOW = SDL_GetPerformanceCounter();
            deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EventType::SDL_EVENT_QUIT) {
                    running = false;
                }
            }

            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            SDL_RenderClear(renderer);

            switch (scene) {
            case 0:
                Scene_rotateXYProjection(PI / 6);
                break;
            case 1:
                Scene_transformPoint(-0.0005);
                break;
            case 2:
                Scene_deleteFaces();
                break;
            case 3:
                Scene_rotateXYProjection(angle);
                break;
            case 4:
                Scene_rotatePointProjection(-0.0005, angle);
                break;
            default:
                break;
            }

            angle += PI / 16 * deltaTime;
            SDL_RenderPresent(renderer);
        }

        cleanupSDL(window);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    return 0;
}
