#ifndef DRAWLIST_H
#define DRAWLIST_H

#include <lua.h>

#include "types.h"

/*
Drawlist Functions
*/
void draw(NodeDrawable *node);
void clear_drawlist();
void add_drawable(void *drawable, char type);
void add_text(char *text_s, int x, int y);
void draw_text(TextItem *text);
void add_line(int x1, int y1, int x2, int y2, Color color);
void draw_line(LineItem *line);
void add_rect(int x, int y, int width, int height, bool filled, Color color);
void draw_rect(RectItem *rect);
void add_circle(int center_x, int center_y, int radius, bool filled, Color color, bool has_border, Color border_color);
void draw_circle(CircleItem *circle);
/*
Lua Functions
*/
int lua_draw_text(lua_State *L);
int lua_draw_line(lua_State *L);
int lua_draw_rect(lua_State *L);
int lua_draw_circle(lua_State *L);
#endif
