#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <string.h>
#include <stdint.h>

//----------------------------------------------------------------------------------
// Line
//----------------------------------------------------------------------------------
typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
    int color_index;
} LineItem;

//----------------------------------------------------------------------------------
// Rect
//----------------------------------------------------------------------------------
typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool filled;
    int color_index;
} RectItem;

//----------------------------------------------------------------------------------
// Circle
//----------------------------------------------------------------------------------
typedef struct {
    int center_x;
    int center_y;
    int radius;
    bool filled;
    bool has_border;
    int border_color_index;
    int color_index;
} CircleItem;

//----------------------------------------------------------------------------------
// Triangle
//----------------------------------------------------------------------------------
typedef struct {
    int p1_x;
    int p1_y;
    int p2_x;
    int p2_y;
    int p3_x;
    int p3_y;
    int color_index;
} TriangleItem;

//----------------------------------------------------------------------------------
// Clear
//----------------------------------------------------------------------------------
typedef struct {
    int color_index;
} ClearItem;

//----------------------------------------------------------------------------------
// Map Layer Data
//----------------------------------------------------------------------------------
typedef struct {
    const char *path;
    int tile_w;
    int tile_h;
    int *tile_ids;  // -1 = empty, otherwise tile_id_flags
    int count;      // map_width * map_height
} MapLayerData;

//----------------------------------------------------------------------------------
// Keyboard
//----------------------------------------------------------------------------------
typedef struct {
    int key;
    int alt_key;
} KeyboardKeyData;

#endif // TYPES_H
