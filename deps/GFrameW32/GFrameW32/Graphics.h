#pragma once
#include <vector>

struct point {
    int x;
    int y;

    point& operator+(point& a) {
        x += a.x;
        y += a.y;
        return *this;
    }

    point& operator*(double t) {
        x *= t;
        y *= t;
        return *this;
    }
};

enum CLPointType {
    LEFT,
    RIGHT,
    BEYOND,
    BEHIND,
    BETWEEN,
    ORIGIN,
    DESTINATION
};

enum EType { 
    TOUCHING, 
    CROSS_LEFT, 
    CROSS_RIGHT, 
    INESSENTIAL
};

enum PType { 
    INSIDE, 
    OUTSIDE 
};

enum PolygonType{
    CONVEX,
    CONCAVE
};

enum IntersectType {
    SAME, 
    PARALLEL,
    SKEW, 
    SKEW_CROSS, 
    SKEW_NO_CROSS
};

// Graphics processing handler declarations
bool gfInitScene();
void gfDrawScene();
void gfCleanupScene();
void gfOnLMouseClick( int x, int y );
void gfOnRMouseClick( int x, int y );
void gfOnKeyDown( UINT key );
void gfOnKeyUp( UINT key );

PType PInPolygonEOMode(double x, double y, const point *p, int n);
PType PInPolygonNZWMode(double x, double y, const point* p, int n);
void gfDrawPolygon(std::vector<point> const& points, PType(*inFucn)(double, double, const point* p, int n) = PInPolygonEOMode);
PolygonType getPolygonType(std::vector<point> const& vert);
void gfDrawBezie(point p0, point p1, point p2, point p3, RGBPIXEL color = RGBPIXEL::Blue(), int qual = 200);
void gfDrawBezie(point p0, point p1, point p2, RGBPIXEL color = RGBPIXEL::Blue(), int qual = 200);

void DrawCircle(point p0, double r, RGBPIXEL color = RGBPIXEL::Blue());

IntersectType Intersect (
    double ax, double ay, 
    double bx, double by, 
    double cx, double cy,
    double dx, double dy, 
    double *t
);

IntersectType Cross(
    double ax, double ay, 
    double bx, double by,
    double cx, double cy,
    double dx, double dy, 
    double *tab, double *tcd
);