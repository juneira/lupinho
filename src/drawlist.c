#include <stdlib.h>
#include <stdio.h>

#include "drawlist.h"

/*
Global vars
*/
extern Drawlist drawlist;
Color palette[PALETTE_SIZE];

/*
Drawable Functions
*/
void draw(NodeDrawable *node) {
    switch(node->type) {
        case 't':
            draw_text((TextItem *) node->drawable);
            break;
        case 'l':
            draw_line((LineItem *) node->drawable);
            break;
        case 'r':
            draw_rect((RectItem *) node->drawable);
            break;
        case 'c':
            draw_circle((CircleItem *) node->drawable);
            break;
        case 'v':
            draw_triangle((TriangleItem *) node->drawable);
            break;
        case 's':
            draw_tile((TileItem *) node->drawable);
            break;
        case 'w':
            draw_sprite((SpriteItem *) node->drawable);
            break;
    }
}

void clear_drawlist() {
    NodeDrawable *current = drawlist.root;
    while(current != NULL) {
        NodeDrawable *next = current->next;
        free(current->drawable);
        free(current);
        current = next;
    }
    drawlist.count = 0;
    drawlist.root = NULL;
}

void add_drawable(void *drawable, char type) {
    if(drawlist.count == 0) {
        drawlist.count++;

        NodeDrawable *node = (NodeDrawable *) malloc(sizeof(NodeDrawable));
        drawlist.root = node;
        node->drawable = drawable;
        node->type = type;
        node->next = NULL;
        return;
    }

    drawlist.count++;

    NodeDrawable *current = drawlist.root;
    for(; current->next != NULL; current = current->next) {}

    NodeDrawable *node = (NodeDrawable *) malloc(sizeof(NodeDrawable));
    current->next = node;
    node->next = NULL;
    node->drawable = drawable;
    node->type = type;
}

/**
Text Functions
**/
void add_text(char *text_s, int x, int y) {
    TextItem *text = (TextItem *) malloc(sizeof(TextItem));
    text->text = text_s;
    text->x = x;
    text->y = y;
    text->fontSize = 20;
    text->color = DARKGRAY;

    add_drawable(text, 't');
}

void draw_text(TextItem *text) {
    DrawText(text->text, text->x, text->y, text->fontSize, text->color);
}

/**
Line Functions
**/
void add_line(int x1, int y1, int x2, int y2, Color color) {
    LineItem *line = (LineItem *) malloc(sizeof(LineItem));
    line->x1 = x1;
    line->y1 = y1;
    line->x2 = x2;
    line->y2 = y2;
    line->color = color;

    add_drawable(line, 'l');
}

void draw_line(LineItem *line) {
    DrawLine(line->x1, line->y1, line->x2, line->y2, line->color);
}

/**
Rect Functions
**/
void add_rect(int x, int y, int width, int height, bool filled, Color color) {
    RectItem *rect = (RectItem *) malloc(sizeof(RectItem));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->filled = filled;
    rect->color = color;

    add_drawable(rect, 'r');
}

void draw_rect(RectItem *rect) {
    if(rect->filled) {
        DrawRectangle(rect->x, rect->y, rect->width, rect->height, rect->color);
    } else {
        DrawRectangleLines(rect->x, rect->y, rect->width, rect->height, rect->color);
    }
}

/**
Circle Functions
**/
void add_circle(int center_x, int center_y, int radius, bool filled, Color color, bool has_border, Color border_color) {
    CircleItem *circle = (CircleItem *) malloc(sizeof(CircleItem));
    circle->center_x = center_x;
    circle->center_y = center_y;
    circle->radius = radius;
    circle->filled = filled;
    circle->color = color;
    circle->has_border = has_border;
    circle->border_color = border_color;

    add_drawable(circle, 'c');
}

void draw_circle(CircleItem *circle) {
    if(circle->filled) {
        DrawCircle(circle->center_x, circle->center_y, circle->radius, circle->color);
    }

    if(circle->has_border) {
        DrawCircleLines(circle->center_x, circle->center_y, circle->radius, circle->border_color);
    }
}

/**
Triangle Functions
**/
void add_triangle(int p1_x, int p1_y, int p2_x, int p2_y, int p3_x, int p3_y, Color color) {
    TriangleItem *triangle = (TriangleItem *) malloc(sizeof(TriangleItem));
    triangle->p1_x = p1_x;
    triangle->p1_y = p1_y;
    triangle->p2_x = p2_x;
    triangle->p2_y = p2_y;
    triangle->p3_x = p3_x;
    triangle->p3_y = p3_y;
    triangle->color = color;

    add_drawable(triangle, 'v');
}

void draw_triangle(TriangleItem *triangle) {
    Vector2 v1 = { triangle->p1_x, triangle->p1_y };
    Vector2 v2 = { triangle->p2_x, triangle->p2_y };
    Vector2 v3 = { triangle->p3_x, triangle->p3_y };

    DrawTriangle(v1, v3, v2, triangle->color);
}

/**
Sprite Functions
**/
void add_sprite(const char *data, int width, int height, int ntiles, int x, int y) {
    SpriteItem *sprite = (SpriteItem *) malloc(sizeof(SpriteItem));
    sprite->data = data;
    sprite->tile_width = width;
    sprite->tile_height = height;
    sprite->x = x;
    sprite->y = y;
    sprite->ntiles = ntiles;

    add_drawable(sprite, 'w');
}

void draw_sprite(SpriteItem *sprite) {
    printf("Drawing sprite with %d tiles\n", sprite->ntiles);

    int pixel_index = 0;

    for(int tile_index = 0; tile_index < sprite->ntiles; tile_index++) {
        for(int y = 0; y < sprite->tile_height; y++) {
            for(int x = 0; x < sprite->tile_width; x++) {
                int palette_idx = (int) sprite->data[pixel_index++];
                if (palette_idx == 0) continue; // transparent
                Color color = get_palette_color(palette_idx);
                DrawPixel(sprite->x + x + (tile_index * sprite->tile_width), sprite->y + y, color);
            }
        }
    }
}

/**
Tile Functions
**/
void add_tile(const char *data, int width, int height, int tile_index, int x, int y) {
    TileItem *tile = (TileItem *) malloc(sizeof(TileItem));
    tile->data = data;
    tile->width = width;
    tile->height = height;
    tile->tile_index = tile_index;
    tile->x = x;
    tile->y = y;

    add_drawable(tile, 's');
}

void draw_tile(TileItem *tile) {
    int start_pixel = tile->tile_index * tile->width * tile->height;

    for(int y = 0; y < tile->height; y++) {
        for(int x = 0; x < tile->width; x++) {
            int pixel_index = start_pixel + y * tile->width + x;
            int palette_idx = (int) tile->data[pixel_index];

            if (palette_idx == 0) continue; // transparent

            Color color = get_palette_color(palette_idx);
            DrawPixel(tile->x + x, tile->y + y, color);
        }
    }
}

/**
Palette Functions
**/
void palset(int position, int bgr555) {
    if (position < 0 || position >= PALETTE_SIZE) return;

    // Extract BGR555 components (5 bits each)
    int r5 = (bgr555 >> 0) & 0x1F;
    int g5 = (bgr555 >> 5) & 0x1F;
    int b5 = (bgr555 >> 10) & 0x1F;

    // Scale from 5-bit (0-31) to 8-bit (0-255)
    palette[position].r = (r5 << 3) | (r5 >> 2);
    palette[position].g = (g5 << 3) | (g5 >> 2);
    palette[position].b = (b5 << 3) | (b5 >> 2);
    palette[position].a = 255;
}

Color get_palette_color(int index) {
    if (index >= 0 && index < PALETTE_SIZE) {
        return palette[index];
    }
    return DARKGRAY; // fallback
}
