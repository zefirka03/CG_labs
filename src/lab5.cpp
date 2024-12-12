#include <SDL3/SDL.h>
#include <vector>
#include <glm.hpp>

SDL_Renderer* renderer;

void drawPixel(int x, int y, SDL_Color color = {255,255,255,255}) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(renderer, x, y);
}

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

void drawLine(Point3D start, Point3D end) {
    int dx = end.x - start.x;
    int dy = end.y - start.y;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);
    int sX = (dx > 0) ? 1 : -1; // Направление по оси X
    int sY = (dy > 0) ? 1 : -1; // Направление по оси Y

    // Рисуем линию, если длина по X больше, чем по Y
    if (abs_dx > abs_dy) {
        int err = abs_dx / 2;
        while (start.x != end.x) {
            drawPixel(start.x, start.y); // Рисуем пиксель
            err -= abs_dy;
            if (err < 0) {
                start.y += sY; // Изменяем Y
                err += abs_dx;
            }
            start.x += sX; // Изменяем X
        }
    }
    else { // Рисуем линию, если длина по Y больше, чем по X
        int err = abs_dy / 2;
        while (start.y != end.y) {
            drawPixel(start.x, start.y); // Рисуем пиксель
            err -= abs_dx;
            if (err < 0) {
                start.x += sX; // Изменяем X
                err += abs_dy;
            }
            start.y += sY; // Изменяем Y
        }
    }
    drawPixel(end.x, end.y); // Рисуем конечный пиксель
}

std::vector<std::vector<Point3D>> transform(std::vector<std::vector<Point3D>> faces, std::vector<std::vector<double>> T) {
    std::vector<std::vector<Point3D>> result = {};

    for (int i = 0; i < faces.size(); ++i) {

        std::vector<Point3D> face = {};

        for (int j = 0; j < 4; ++j) {

            Point3D p = faces[i][j];


            double x_new = p.x * T[0][0] + p.y * T[1][0] + p.z * T[2][0] + T[3][0];
            double y_new = p.x * T[0][1] + p.y * T[1][1] + p.z * T[2][1] + T[3][1];
            double z_new = p.x * T[0][2] + p.y * T[1][2] + p.z * T[2][2] + T[3][2];
            double H = p.x * T[0][3] + p.y * T[1][3] + p.z * T[2][3] + T[3][3];

            if (H == 0) {
                H = 1;
            }

            Point3D p_new = { x_new / H, y_new / H, z_new / H };

            face.push_back(p_new);
        }

        result.push_back(face);
    }

    return result;
}

std::vector<std::vector<Point3D>> resize(std::vector<std::vector<Point3D>> faces, double M) {
    std::vector<std::vector<double>> T = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1 / M},
    };

    return transform(faces, T);
}

std::vector<std::vector<Point3D>> translate(std::vector<std::vector<Point3D>> faces, double tx, double ty, double tz) {
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
        { {1, 0, 1}, {1, 1, 1},  {0, 1, 1}, {0, 0, 1}, }, // верхняя грань
        { {0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0} }, // нижняя грань
        { {1, 0, 1}, {0, 0, 1}, {0, 0, 0}, {1, 0, 0}, }, // левая грань
        { {1, 1, 0},  {0, 1, 0},  {0, 1, 1}, {1, 1, 1}, }, // правая грань
        { {0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}, }, // задняя грань
        { {1, 1, 1}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0} }, // передняя грань
    };

    faces = resize(faces, 200);
    faces = translate(faces, 200, 200, 200);

    return faces;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("lab5",
        800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;


    std::vector<Point3D> cuboidVertices = createCuboidVertices(100,100,100);

    float k = 5.0f; // Центр проекции
    float rotationSpeed = 0.01f; // Скорость вращения
    float deltaTime = 0.016f; // Примерно 60 кадров в секунду


    while (running) {
        // Обработка событий
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EventType::SDL_EVENT_QUIT) {
                running = false;  // Завершаем цикл, если получено событие закрытия
            }
        }

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255); // Устанавливаем цвет фона
        SDL_RenderClear(renderer);

        drawCuboidWithCulling(cuboidVertices);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
