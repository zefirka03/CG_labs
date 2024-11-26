#include "StdAfx.h"
#include "GF.h"
#include "Graphics.h"

#ifndef M_PI
const double M_PI = 3.1415926535897932384626433832795;
#endif

void DrawLine( int x0, int y0, int x1, int y1, RGBPIXEL color ) {
    int x = x0, y = y0; //текущее значение точки, инициализируем координатами начальной точки
    int dx = x1 - x0, dy = y1 - y0;
    int ix, iy; //величина приращений (-1, 0, 1) по координатам определяют направление движения
    int e; //ошибка
    int i; //счетчик цикла
    //определение величины приращения по координатам, а также получение абсолютных значений dx, dy
    if (dx > 0) ix = 1;
    else if (dx < 0) { ix = -1; dx = -dx; }
    else ix = 0;

    if (dy > 0) iy = 1;
    else if (dy < 0) { iy = -1; dy = -dy; }
    else iy = 0;

    if (dx >= dy) {
        e = 2 * dy - dx; //инициализация ошибки с поправкой на половину пиксела
        if (iy >= 0) { //увеличиваем или не изменяем y
            //основной цикл
            for (i = 0; i <= dx; i++) {
                gfSetPixel(x, y, color); // выводим точку
                if (e >= 0) { //ошибка стала неотрицательной
                    y += iy; //изменяем y
                    e -= 2 * dx; //уменьшаем ошибку
                }
                x += ix; //всегда изменяем x
                e += dy * 2; //и увеличиваем ошибку
            }
        }
        else { //y уменьшается
            for (i = 0; i <= dx; i++) {
                gfSetPixel(x, y, color);
                if (e > 0) { // ошибка стала положительной. Условие изменилось с >= на >
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += dy * 2;
            }
        }
    }
    else {
        e = 2 * dx - dy; //инициализация ошибки с поправкой на половину пиксела
        if (ix >= 0) { //увеличиваем или не изменяем y
            //основной цикл
            for (i = 0; i <= dy; i++) {
                gfSetPixel(x, y, color); // выводим точку
                if (e >= 0) { //ошибка стала неотрицательной
                    x += ix; //изменяем y
                    e -= 2 * dy; //уменьшаем ошибку
                }
                y += iy; //всегда изменяем x
                e += dx * 2; //и увеличиваем ошибку
            }
        }
        else { //y уменьшается
            for (i = 0; i <= dy; i++) {
                gfSetPixel(x, y, color);
                if (e > 0) { // ошибка стала положительной. Условие изменилось с >= на >
                    x += ix;
                    e -= 2 * dy;
                }
                y += iy;
                e += dx * 2;
            }
        }
    }
}

CLPointType Classify(double x1, double y1, double x2, double y2, double x, double y) {
    double ax = x2 - x1; //a
    double ay = y2 - y1;
    double bx = x - x1; //b
    double by = y - y1;
    double s = ax*by - bx*ay;
    if (s > 0) return LEFT;
    if (s < 0) return RIGHT;
    if ((ax*bx < 0) || (ay*by < 0)) //противоположно направлению
        return BEHIND; //позади
    if ((ax*ax + ay*ay) < (bx*bx + by*by))
        return BEHIND; //впереди
    if (x1==x && y1==y) //совпадает с началом
        return ORIGIN;
    if (x2==x && y2==y) //совпадает с концом
        return DESTINATION;
    return BETWEEN; //между
}

EType EdgeType(double ox, double oy, double dx, double dy, double ax, double ay) {
    switch(Classify(ox, oy, dx, dy, ax, ay)) {
        case LEFT:
            if(ay > oy && ay <= dy) return CROSS_LEFT; // пересекающая, A слева
            else return INESSENTIAL; // безразличная
        case RIGHT:
            if(ay > dy && ay <= oy) return CROSS_RIGHT; // пересекающая, A справа
            else return INESSENTIAL; // безразличная
        case BETWEEN:
        case ORIGIN:
        case DESTINATION:
            return TOUCHING; // касающаяся
        default:
            return INESSENTIAL; // безразличная
    }
}

PType PInPolygonEOMode(double x, double y, const point *p, int n) { 
    int param = 0;
    for(int i = 0; i < n; i++) {
        switch(EdgeType(p[i].x, p[i].y, p[(i+1)%n].x, p[(i+1)%n].y, x, y)) {
            case TOUCHING: //если лежит на полигоне, то заведомо принадлежит
                return INSIDE;
            case CROSS_LEFT:
            case CROSS_RIGHT:
                param = 1 - param; //изменяем значение четности
        }
    }
    if (param == 1) return INSIDE; //нечетное
    else return OUTSIDE;
}

PType PInPolygonNZWMode(double x, double y, const point* p, int n) {
    int param = 0;
    for (int i = 0; i < n; i++) {
        switch (EdgeType(p[i].x, p[i].y, p[(i + 1) % n].x, p[(i + 1) % n].y, x, y)) {
            case TOUCHING: //если лежит на полигоне, то заведомо принадлежит
                return INSIDE;
            case CROSS_LEFT:
                ++param;
            case CROSS_RIGHT:
                --param;
        }
    }
    if (param != 0) return INSIDE;
    else return OUTSIDE;
}

void gfDrawPolygon(std::vector<point> const& points, PType(*inFucn)(double, double, const point* p, int n) = PInPolygonEOMode){
    if (points.empty()) return;

    int x_min = INT_MAX, x_max = -INT_MAX;
    int y_min = INT_MAX, y_max = -INT_MAX;

    // bounds
    for (auto& point : points) {
        x_min = min(x_min, point.x);
        x_max = max(x_max, point.x);

        y_min = min(y_min, point.y);
        y_max = max(y_max, point.y);
    }

    for (int x = x_min; x < x_max; ++x) {
        for (int y = y_min; y < y_max; ++y) {
            if (inFucn(x, y, points.data(), points.size()) == PType::INSIDE) {
                gfSetPixel(x, y, RGBPIXEL(255, 0, 0));
            }
        }
    }
}

bool gfInitScene(){
    gfSetWindowSize( 640, 480 );
    
    DrawLine(300, 300, 500, 0, RGBPIXEL::Green());
    gfDrawPolygon({ {100, 400}, {250, 100}, {400, 400}   }, PInPolygonNZWMode);
    //gfDrawPolygon({ {100, 400}, {250, 100}}, PInPolygonNZWMode);

    return true;
}

void gfDrawScene() {
    
}

void gfCleanupScene() {}

void gfOnLMouseClick( int x, int y ) {
    x; y;
    gfDrawRectangle(x - 10, y - 10, x + 10, y + 10, RGBPIXEL::Green());
}

void gfOnRMouseClick( int x, int y ) {
    x; y;
}

void gfOnKeyDown( UINT key ){
    key;

    if( key == 'A' )
        gfDisplayMessage( "'A' key has been pressed" );
}

void gfOnKeyUp( UINT key ) {
    key;
}
