#include <SDL3/SDL.h>
#include <vector>

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

// Функция для построения параллельной проекции параллелепипеда
void drawParallelProjection(const std::vector<Point3D>& vertices) {
    for (const auto& vertex : vertices) {
        drawPixel(static_cast<int>(vertex.x), static_cast<int>(vertex.y));
    }
}

// Функция для построения одноточечной перспективной проекции
void drawPerspectiveProjection(const std::vector<Point3D>& vertices, float k) {
    for (const auto& vertex : vertices) {
        float z = vertex.z + k; // смещение по оси Z
        if (z != 0) {
            int x_perspective = static_cast<int>(vertex.x * (k / z));
            int y_perspective = static_cast<int>(vertex.y * (k / z));
            drawPixel(x_perspective, y_perspective);
        }
    }
}

// Функция для удаления невидимых ребер
void drawVisibleEdges(const std::vector<Point3D>& vertices, const std::vector<std::pair<int, int>>& edges) {
    for (const auto& edge : edges) {
        Point3D start = vertices[edge.first];
        Point3D end = vertices[edge.second];

        // Простой алгоритм: если оба конца ребра имеют положительное Z, то оно видимо
        if (start.z > 0 && end.z > 0) {
            drawLine(start, end);
        }
    }
}

// Функция для анимации параллелепипеда
void animateCuboid(std::vector<Point3D>& vertices, float rotationSpeed, float deltaTime) {
    // Простая анимация: вращение параллелепипеда вокруг оси
    float angle = rotationSpeed * deltaTime;

    for (auto& vertex : vertices) {
        // Применение вращения (например, вокруг оси Z)
        float x_new = vertex.x * cos(angle) - vertex.y * sin(angle);
        float y_new = vertex.x * sin(angle) + vertex.y * cos(angle);
        vertex.x = x_new;
        vertex.y = y_new;
    }
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


    std::vector<Point3D> cuboidVertices = {
        { -100, -100, -100 }, { 100, -100, -100 }, { 100, 100, -100 }, { -100, 100, -100 },
        { -100, -100, 100 }, { 100, -100, 100 }, { 100, 100, 100 }, { -100, 100, 100 }
    };

    std::vector<std::pair<int, int>> edges = {
        { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },
        { 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }
    };

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

        animateCuboid(cuboidVertices, rotationSpeed, deltaTime);

        // Отрисовка параллельной проекции
        drawParallelProjection(cuboidVertices);

        // Отрисовка перспективной проекции
        drawPerspectiveProjection(cuboidVertices, k);

        // Удаление невидимых ребер
        drawVisibleEdges(cuboidVertices, edges);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
