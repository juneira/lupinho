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

void add_triangle(int p1_x, int p1_y, int p2_x, int p2_y, int p3_x, int p3_y, Color color);
void draw_triangle(TriangleItem *triangle);

void add_tile(SpriteInMemory *sprite_in_memory, int tile_index, int x, int y);
void draw_tile(TileItem *tile);
void add_sprite(SpriteInMemory *sprite_in_memory, int x, int y);
void draw_sprite(SpriteItem *sprite);

/*
Palette Functions
*/
#define PALETTE_SIZE 256
extern Color palette[PALETTE_SIZE];
void palset(int position, int bgr555);
Color get_palette_color(int index);

/*
Sprites In Memory Functions
*/
extern SpritesInMemory sprites_in_memory;
void load_sprites_in_memory_from_lua(lua_State *L);
void add_sprite_in_memory(char *name, char *data, int width, int height, int ntiles);
SpriteInMemory* get_sprite_in_memory(char *name);

/*
Lua Functions
*/
int lua_draw_text(lua_State *L);
int lua_draw_line(lua_State *L);
int lua_draw_rect(lua_State *L);
int lua_rect(lua_State *L);
int lua_rectfill(lua_State *L);
int lua_draw_circle(lua_State *L);
int lua_circfill(lua_State *L);
int lua_trisfill(lua_State *L);
int lua_palset(lua_State *L);
int lua_tile(lua_State *L);
int lua_spr(lua_State *L);
int lua_require_sprites(lua_State *L);
int lua_btn(lua_State *L);
int lua_btnp(lua_State *L);

// TODO
int lua_camera(lua_State *L);
int lua_clip(lua_State *L);
int lua_cls(lua_State *L);
int lua_preload_spritesheet(lua_State *L);
int lua_draw_sprite(lua_State *L);
int lua_print(lua_State *L);
int lua_set_pallet(lua_State *L);
int lua_fillp(lua_State *L);
int lua_log(lua_State *L);
#endif
