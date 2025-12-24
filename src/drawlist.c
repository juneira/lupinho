#include <stdlib.h>

#include "drawlist.h"

/*
Global vars
*/
extern Drawlist drawlist;

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
    }
}

void clear_drawlist() {
    NodeDrawable *current = drawlist.root;
    while(current != NULL) {
        NodeDrawable *next = current->next;
        free(current->drawable);  // Free the TextItem (or other drawable)
        free(current);            // Then free the node itself
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
