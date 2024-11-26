#pragma once
#include <vector>

struct point {
    int x;
    int y;
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

// Graphics processing handler declarations
bool gfInitScene();
void gfDrawScene();
void gfCleanupScene();
void gfOnLMouseClick( int x, int y );
void gfOnRMouseClick( int x, int y );
void gfOnKeyDown( UINT key );
void gfOnKeyUp( UINT key );
void gfDrawPolygon(std::vector<point> const& points);
