#include "StdAfx.h"
#include "GF.h"
#include "Graphics.h"

#ifndef M_PI
const double M_PI = 3.1415926535897932384626433832795;
#endif

void DrawLine( int x0, int y0, int x1, int y1, RGBPIXEL color ) {
    int x = x0, y = y0;
    int dx = x1 - x0, dy = y1 - y0;
    int ix, iy;
    int e;
    int i;

    if (dx > 0) ix = 1;
    else if (dx < 0) { ix = -1; dx = -dx; }
    else ix = 0;

    if (dy > 0) iy = 1;
    else if (dy < 0) { iy = -1; dy = -dy; }
    else iy = 0;

    if (dx >= dy) {
        e = 2 * dy - dx;
        if (iy >= 0) {
            for (i = 0; i <= dx; i++) {
                gfSetPixel(x, y, color);
                if (e >= 0) {
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += dy * 2;
            }
        }
        else {
            for (i = 0; i <= dx; i++) {
                gfSetPixel(x, y, color);
                if (e > 0) {
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += dy * 2;
            }
        }
    }
    else {
        e = 2 * dx - dy; 
        if (ix >= 0) {
            for (i = 0; i <= dy; i++) {
                gfSetPixel(x, y, color);
                if (e >= 0) {
                    x += ix; 
                    e -= 2 * dy;
                }
                y += iy;
                e += dx * 2; 
            }
        }
        else { 
            for (i = 0; i <= dy; i++) {
                gfSetPixel(x, y, color);
                if (e > 0) { 
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
    double ax = x2 - x1;
    double ay = y2 - y1;
    double bx = x - x1;
    double by = y - y1;
    double s = ax*by - bx*ay;
    if (s > 0) return LEFT;
    if (s < 0) return RIGHT;
    if ((ax*bx < 0) || (ay*by < 0))
        return BEHIND;
    if ((ax*ax + ay*ay) < (bx*bx + by*by))
        return BEHIND;
    if (x1==x && y1==y)
        return ORIGIN;
    if (x2==x && y2==y)
        return DESTINATION;
    return BETWEEN;
}

EType EdgeType(double ox, double oy, double dx, double dy, double ax, double ay) {
    switch(Classify(ox, oy, dx, dy, ax, ay)) {
        case LEFT:
            if(ay > oy && ay <= dy) return CROSS_LEFT;
            else return INESSENTIAL;
        case RIGHT:
            if(ay > dy && ay <= oy) return CROSS_RIGHT;
            else return INESSENTIAL;
        case BETWEEN:
        case ORIGIN:
        case DESTINATION:
            return TOUCHING;
        default:
            return INESSENTIAL;
    }
}

PType PInPolygonEOMode(double x, double y, const point *p, int n) { 
    int param = 0;
    for(int i = 0; i < n; i++) {
        switch(EdgeType(p[i].x, p[i].y, p[(i+1)%n].x, p[(i+1)%n].y, x, y)) {
            case TOUCHING:
                return INSIDE;
            case CROSS_LEFT:
            case CROSS_RIGHT:
                param = 1 - param;
        }
    }
    if (param == 1) return INSIDE;
    else return OUTSIDE;
}

PType PInPolygonNZWMode(double x, double y, const point* p, int n) {
    int param = 0;
    for (int i = 0; i < n; i++) {
        switch (EdgeType(p[i].x, p[i].y, p[(i + 1) % n].x, p[(i + 1) % n].y, x, y)) {
            case TOUCHING:
                return INSIDE;
            case CROSS_LEFT:
                ++param;
                break;
            case CROSS_RIGHT:
                --param;
                break;
            default:
                break;
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
    
    std::vector<point> star{ {100, 400}, {250, 100}, {400, 400}, {80, 150}, {420, 150} };

    DrawLine(300, 300, 500, 0, RGBPIXEL::Green());
    gfDrawPolygon(star, PInPolygonEOMode);

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
