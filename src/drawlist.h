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

/*
Lua Functions
*/
int lua_draw_text(lua_State *L);

#endif
