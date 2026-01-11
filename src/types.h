#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <string.h>
#include <stdint.h>

// Text
#define MAX_TEXT_LENGTH 100
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
    uint8_t fill_pattern[8];
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
    uint8_t fill_pattern[8];
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

// Sprite In Memory
#define MAX_SPRITE_NAME_LENGTH 256
typedef struct {
    char name[MAX_SPRITE_NAME_LENGTH];
    Texture2D texture;
    int tile_width;
    int tile_height;
    int ntiles;
} SpriteInMemory;

// Sprites In Memory
typedef struct {
    SpriteInMemory **sprites;
    int count;
    int max_count;
} SpritesInMemory;

// Tile Drawable
typedef struct {
    SpriteInMemory *sprite_in_memory;
    int tile_index;
    int x;
    int y;
    bool flipped;
} TileItem;

// Sprite Drawable
typedef struct {
    SpriteInMemory *sprite_in_memory;
    int x;
    int y;
    bool flipped;
} SpriteItem;

// Clear Drawable
typedef struct {
    Color color;
} ClearItem;

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
