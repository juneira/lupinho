#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <string.h>
#include <stdint.h>

#define MAX_TEXT_LENGTH 100
#define MAX_TILES_PER_SPRITE 4096  // Increased to support 64x64 sprites
#define MAX_SPRITE_SHEETS 5000

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

// Tile (for drawing)
typedef struct {
    int spritesheet;
    int tile_index;
    int x;
    int y;
} TileItem;

// Sprite
typedef struct {
    int width;
    int height;
    uint8_t pallet_index[MAX_TILES_PER_SPRITE];
} SpriteItem;

extern int sprit_current_index;
extern SpriteItem sprite_sheet[MAX_SPRITE_SHEETS];

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
