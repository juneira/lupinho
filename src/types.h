#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <string.h>
#include <stdint.h>

#define MAX_TEXT_LENGTH 100

// Text
typedef struct {
    char *text;
    int x;
    int y;
    int fontSize;
    Color color;
} TextItem;

// Line
typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
    Color color;
} LineItem;

// Rect
typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool filled;
    Color color;
} RectItem;

// Circle
typedef struct {
    int center_x;
    int center_y;
    int radius;
    bool filled;
    bool has_border;
    Color border_color;
    Color color;
} CircleItem;

// Triangle
typedef struct {
    int p1_x;
    int p1_y;
    int p2_x;
    int p2_y;
    int p3_x;
    int p3_y;
    Color color;
} TriangleItem;

// Tile
typedef struct {
    const char *data;
    int width;
    int height;
    int tile_index;
    int x;
    int y;
} TileItem;

// Sprite
typedef struct {
    const char *data;
    int x;
    int y;
    int tile_width;
    int tile_height;
    int ntiles;
} SpriteItem;

// List Objects
typedef struct NodeDrawable NodeDrawable;

struct NodeDrawable{
    char type;
    void *drawable;
    NodeDrawable *next;
};

typedef struct {
    int count;
    NodeDrawable *root;
} Drawlist;

#endif // TYPES_H
