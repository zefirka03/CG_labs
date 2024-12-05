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

void DrawLine(point a, point b, RGBPIXEL color) {
    DrawLine(a.x, a.y, b.x, b.y, color);
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

void gfDrawPolygon(std::vector<point> const& points, PType(*inFucn)(double, double, const point* p, int n)){
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


IntersectType Intersect (
    double ax, double ay, 
    double bx, double by, 
    double cx, double cy,
    double dx, double dy, 
    double *t
) {
    double nx= dy - cy;
    double ny= cx - dx;
    CLPointType type;
    double denom= nx*(bx - ax) + ny*(by - ay);
    if (denom == 0) {
        type = Classify(cx,cy,dx,dy,ax,ay);
        if (type == LEFT || type == RIGHT)
        return PARALLEL;
        else return SAME;
    }
    double num= nx*(ax - cx) + ny*(ay - cy);
    *t= -num/denom;
    return SKEW;
}

IntersectType Cross(
    double ax, double ay, 
    double bx, double by,
    double cx, double cy,
    double dx, double dy, 
    double *tab, double *tcd
) {
    IntersectType type = Intersect(ax, ay, bx, by, cx, cy, dx, dy, tab);
    if (type==SAME || type==PARALLEL)
    return type;
    if ((*tab < 0) || (*tab > 1))
    return SKEW_NO_CROSS;
    Intersect(cx, cy, dx, dy, ax, ay, bx, by, tcd);
    if ((*tcd < 0) || (*tcd > 1))
    return SKEW_NO_CROSS;
    return SKEW_CROSS;
}

PolygonType getPolygonType(std::vector<point> const& vert){
    for(int s = 0; s < vert.size(); ++s){
        const int vo = s;
        const int vd = (s+1)%vert.size();
        const point& a = vert[vo];
        const point& b = vert[vd];

        CLPointType curr_type = ORIGIN;
        for(int v=0; v < vert.size(); ++v){
            if(v == vo || v == vd) continue;
            CLPointType temp_type = Classify(a.x, a.y, b.x, b.y, vert[v].x, vert[v].y);
            if(curr_type == ORIGIN){
                curr_type = temp_type;
                continue;
            }

            if(curr_type != temp_type)
                return CONCAVE;
        }
    }
    return CONVEX;
}

int getPolygonCross(std::vector<point> const& vert){
    for(int s = 0; s < vert.size(); ++s){
        const int vo = s;
        const int vd = (s+1)%vert.size();
        const point& a0 = vert[vo];
        const point& b0 = vert[vd];

        for(int s1 = s + 1; s1 < vert.size(); ++s1){
            const int vo1 = s1;
            const int vd1 = (s1+1)%vert.size();
            const point& a1 = vert[vo1];
            const point& b1 = vert[vd1];

            double tab, tcd;
            if(Cross(a0.x, a0.y, b0.x, b0.y, a1.x, a1.y, b1.x, b1.y, &tab, &tcd) && tab * tcd != 0 && tab != 1 && tcd != 1)
                return 1;
        }
    }
    return 0;
}

int CyrusBeckClipLine(double& x1, double& y1, double& x2, double& y2, std::vector<point> const& vert) {
    double t1 = 0, t2 = 1, t;
    double sx = x2 - x1, sy = y2 - y1;
    double nx, ny, demon, num, x1_new, y1_new, x2_new, y2_new;
    for (int i = 0; i < vert.size(); i++) {
        nx = vert[(i + 1) % vert.size()].y - vert[i].y; ny = vert[i].x - vert[(i + 1) % vert.size()].x;
        double denom = nx * sx + ny * sy;
        num = nx * (x1 - vert[i].x) + ny * (y1 - vert[i].y);
        if (denom != 0) {
            t = -num / denom;
            if (denom < 0) { 
                if (t > t1) 
                    t1 = t;
            }
            else {
                if (t < t2) t2 = t;
            }
        }
        else { if (Classify(vert[i].x, vert[i].y, vert[(i + 1) % vert.size()].x, vert[(i + 1) % vert.size()].y, x1, y1) == LEFT) return 0; } // параллельны
    }
    if (t1 <= t2) {
        x1_new = x1 + t1 * (x2 - x1); y1_new = y1 + t1 * (y2 - y1);
        x2_new = x1 + t2 * (x2 - x1); y2_new = y1 + t2 * (y2 - y1);
        x1 = x1_new; y1 = y1_new; x2 = x2_new; y2 = y2_new;
        return 1;
    }
    return 0;
}

point line(point p0, point p1, double t) {
    return p0 * (1.0 - t) + p1 * t;
}

point BezierQuadratic(point p0, point p1, point p2, double t) {
    return line(line(p0, p1, t), line(p1, p2, t), t);
}

point BezierCubic(point p0, point p1, point p2, point p3, double t) {
    return line(BezierQuadratic(p0, p1, p2, t), BezierQuadratic(p1, p2, p3, t), t);
}

void gfDrawBezie(point p0, point p1, point p2, point p3, int qual, RGBPIXEL color) {
    for (int i = 0; i < qual; ++i) {
        DrawLine(
            BezierCubic(p0, p1, p2, p3, i / float(qual)),
            BezierCubic(p0, p1, p2, p3, (i + 1) / float(qual)),
            color
        );
    }
}

void DrawCircle(point p0, double r, RGBPIXEL color) {
    int x = 0;
    int y = static_cast<int>(r); 
    int d = 3 - 2 * static_cast<int>(r);

    auto plot_circle_points = [&](int x_offset, int y_offset) {
        gfSetPixel(static_cast<int>(p0.x + x_offset), static_cast<int>(p0.y + y_offset), color);
        gfSetPixel(static_cast<int>(p0.x - x_offset), static_cast<int>(p0.y + y_offset), color);
        gfSetPixel(static_cast<int>(p0.x + x_offset), static_cast<int>(p0.y - y_offset), color);
        gfSetPixel(static_cast<int>(p0.x - x_offset), static_cast<int>(p0.y - y_offset), color);
        gfSetPixel(static_cast<int>(p0.x + y_offset), static_cast<int>(p0.y + x_offset), color);
        gfSetPixel(static_cast<int>(p0.x - y_offset), static_cast<int>(p0.y + x_offset), color);
        gfSetPixel(static_cast<int>(p0.x + y_offset), static_cast<int>(p0.y - x_offset), color);
        gfSetPixel(static_cast<int>(p0.x - y_offset), static_cast<int>(p0.y - x_offset), color);
    };

    plot_circle_points(x, y);

    while (x < y) {
        x++;

        if (d <= 0) {
            d = d + 4 * x + 6;
        } else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        
        plot_circle_points(x, y);
    }
}

bool gfInitScene(){
    gfSetWindowSize( 640, 480 );
    
    std::vector<point> star{ {100, 400}, {250, 100}, {400, 400}, {80, 150}, {420, 150} };
    std::vector<point> triangle{ {100, 400}, {250, 100}, {400, 400}};
    int test = 0;
    
    double x1 = 0, y1 = 0, x2 = 640, y2 = 480;
    switch (test) {
    case 0:
        DrawCircle({ 300, 300 }, 50, RGBPIXEL::Red());
        break;
    case 1:
        DrawCircle({ 300, 300 }, 100, RGBPIXEL::Red());
        break;
    }
    
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
