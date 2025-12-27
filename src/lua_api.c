#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "drawlist.h"
#include "raylib.h"

//----------------------------------------------------------------------------------
// ui.draw_text(text:string, x:int, y:int)
//----------------------------------------------------------------------------------
int lua_draw_text(lua_State *L) {
    char *text = luaL_checkstring(L, 1);
    int x = luaL_optinteger(L, 2, 10);
    int y = luaL_optinteger(L, 3, 10);

    add_text(text, x, y);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_line(x1:int, y1:int, x2:int, y2:int, color:int)
//----------------------------------------------------------------------------------
int lua_draw_line(lua_State *L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int color = luaL_checkinteger(L, 5);

    add_line(x1, y1, x2, y2, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_rect(x:int, y:int, width:int, height:int, filled:bool, color:int)
//----------------------------------------------------------------------------------
int lua_draw_rect(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);
    bool filled = lua_toboolean(L, 5);
    int color = luaL_checkinteger(L, 6);

    add_rect(x, y, width, height, filled, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.rect(x1, y1, x2, y2, color)
//----------------------------------------------------------------------------------
int lua_rect(lua_State *L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int color = luaL_checkinteger(L, 5);

    add_rect(x1, y1, (x2 - x1), (y2 - y1), false, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.rectfill(x1, y1, x2, y2, color)
//----------------------------------------------------------------------------------
int lua_rectfill(lua_State *L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int color = luaL_checkinteger(L, 5);

    add_rect(x1, y1, (x2 - x1), (y2 - y1), true, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_circle(center_x:int, center_y:int, radius:int, filled:bool, color:int, border:bool, border_color:int)
//----------------------------------------------------------------------------------
int lua_draw_circle(lua_State *L) {
    int center_x = luaL_checkinteger(L, 1);
    int center_y = luaL_checkinteger(L, 2);
    int radius = luaL_checkinteger(L, 3);
    bool filled = lua_toboolean(L, 4);
    int color = luaL_checkinteger(L, 5);
    bool border = lua_toboolean(L, 6);
    int border_color = luaL_checkinteger(L, 7);

    add_circle(center_x, center_y, radius, filled, get_palette_color(color), border, get_palette_color(border_color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.circfill(x, y, radius, color)
//----------------------------------------------------------------------------------
int lua_circfill(lua_State *L) {
    int center_x = luaL_checkinteger(L, 1);
    int center_y = luaL_checkinteger(L, 2);
    int radius = luaL_checkinteger(L, 3);
    int color = luaL_checkinteger(L, 4);

    add_circle(center_x, center_y, radius, true, get_palette_color(color), true, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.trisfill(p1_x:int, p1_y:int, p2_x:int, p2_y:int, p3_x:int, p3_y:int, color:int)
//----------------------------------------------------------------------------------
int lua_trisfill(lua_State *L) {
    int p1_x = luaL_checkinteger(L, 1);
    int p1_y = luaL_checkinteger(L, 2);
    int p2_x = luaL_checkinteger(L, 3);
    int p2_y = luaL_checkinteger(L, 4);
    int p3_x = luaL_checkinteger(L, 5);
    int p3_y = luaL_checkinteger(L, 6);
    int color = luaL_checkinteger(L, 7);

    add_triangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.palset(position:int, color:int)
//----------------------------------------------------------------------------------
int lua_palset(lua_State *L) {
    int position = luaL_checkinteger(L, 1);
    int color = luaL_checkinteger(L, 2);

    palset(position, color);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.tile(spritesheet:int, tile_index:int, x:int, y:int)
//----------------------------------------------------------------------------------
int lua_tile(lua_State *L) {
    int spritesheet = luaL_checkinteger(L, 1);
    int tile_index = luaL_checkinteger(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);

    add_tile(spritesheet, tile_index, x, y);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.spr(spritesheet:int, x:int, y:int)
//----------------------------------------------------------------------------------
int lua_spr(lua_State *L) {
    int spritesheet = luaL_checkinteger(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    add_sprite(spritesheet, x, y);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.btn(button:int, pad:int) -> bool
//----------------------------------------------------------------------------------
int lua_btn(lua_State *L) {
    int button = luaL_checkinteger(L, 1);
    int pad = luaL_optinteger(L, 2, 0);

    bool is_down = IsGamepadButtonDown(pad, button);
    lua_pushboolean(L, is_down);

    return 1;
}

//----------------------------------------------------------------------------------
// ui.btnp(button:int, pad:int) -> bool
//----------------------------------------------------------------------------------
int lua_btnp(lua_State *L) {
    int button = luaL_checkinteger(L, 1);
    int pad = luaL_optinteger(L, 2, 0);

    bool is_pressed = IsGamepadButtonPressed(pad, button);
    lua_pushboolean(L, is_pressed);

    return 1;
}

//----------------------------------------------------------------------------------
// require("sprites") - returns SpriteSheets table
//----------------------------------------------------------------------------------
extern int get_sprite_count();
extern const char* get_sprite_name_at(int idx);
extern int get_sprite_index_at(int idx);

int lua_require_sprites(lua_State *L) {
    // Create SpriteSheets table
    lua_newtable(L);

    int sprite_count = get_sprite_count();
    for (int i = 0; i < sprite_count; i++) {
        const char* name = get_sprite_name_at(i);
        int index = get_sprite_index_at(i);

        if (name) {
            lua_pushinteger(L, index);
            lua_setfield(L, -2, name);
        }
    }

    // Set as global SpriteSheets
    lua_pushvalue(L, -1);
    lua_setglobal(L, "SpriteSheets");

    return 1;
}
