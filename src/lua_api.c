#include <stdio.h>
#include <string.h>
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
// ui.tile(spritesheet:table, tile_index:int, x:int, y:int)
// tile_index can have bit 10 (1024) set to flip horizontally
//----------------------------------------------------------------------------------
int lua_tile(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "name");
    const char *name = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    int tile_index_with_flags = luaL_checkinteger(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);

    bool flipped = (tile_index_with_flags & 1024) != 0;
    int tile_index = tile_index_with_flags & ~1024;

    SpriteInMemory *sprite_in_memory = get_sprite_in_memory(name);

    add_tile(sprite_in_memory, tile_index, x, y, flipped);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.spr(spritesheet:table, x:int, y:int, flipped:bool = false)
//----------------------------------------------------------------------------------
int lua_spr(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "name");
    const char *name = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    bool flipped = false;
    if(lua_gettop(L) > 3) {
        flipped = lua_toboolean(L, 4);
    }

    SpriteInMemory *sprite_in_memory = get_sprite_in_memory(name);

    add_sprite(sprite_in_memory, x, y, flipped);

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
// ui.cls(color:int)
//----------------------------------------------------------------------------------
int lua_cls(lua_State *L) {
    int color = luaL_checkinteger(L, 1);
    add_clear(get_palette_color(color));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.log(message:string)
//----------------------------------------------------------------------------------
int lua_log(lua_State *L) {
    char *message = luaL_checkstring(L, 1);
    printf("[LUPINHO] %s\n", message);
    return 0;
}

//----------------------------------------------------------------------------------
// ui.fillp([pattern_bytes...])
//----------------------------------------------------------------------------------
int lua_fillp(lua_State *L) {
    int nargs = lua_gettop(L);

    if (nargs == 0) {
        memset(fill_pattern, 0, sizeof(fill_pattern));
        return 0;
    }

    for (int i = 0; i < nargs && i < 8; i++) {
        fill_pattern[i] = (uint8_t)luaL_checkinteger(L, i + 1);
    }

    // Fill remaining bytes with 0 if fewer than 8 arguments
    for (int i = nargs; i < 8; i++) {
        fill_pattern[i] = 0;
    }

    return 0;
}


// TODO

//----------------------------------------------------------------------------------
// ui.camera(x, y)
//----------------------------------------------------------------------------------
int lua_camera(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.clip(x, y, width, height)
//----------------------------------------------------------------------------------
int lua_clip(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.preload_spritesheet(spritesheet)
//----------------------------------------------------------------------------------
int lua_preload_spritesheet(lua_State *L) {
    int spritesheet = luaL_checkinteger(L, 1);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_sprite(x, y, sprite_index, size)
//----------------------------------------------------------------------------------
int lua_draw_sprite(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int sprite_index = luaL_checkinteger(L, 3);
    int size = luaL_checkinteger(L, 4);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.print(text, x, y, color)
//----------------------------------------------------------------------------------
int lua_print(lua_State *L) {
    char *text = luaL_checkstring(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int color = luaL_checkinteger(L, 4);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.set_pallet(start_index, count, palette_table)
//----------------------------------------------------------------------------------
int lua_set_pallet(lua_State *L) {
    int start_index = luaL_checkinteger(L, 1);
    int count = luaL_checkinteger(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);  // Check that argument 3 is a table

    return 0;
}

//----------------------------------------------------------------------------------
// Load sprites in memory from Lua global table "SpriteSheets"
// Reads the SpriteSheets table and populates the C sprites in memory array using add_sprite_in_memory()
//----------------------------------------------------------------------------------
void load_sprites_in_memory_from_lua(lua_State *L) {
    lua_getglobal(L, "SpriteSheets");

    if (!lua_istable(L, -1)) {
        printf("Warning: SpriteSheets table not found in Lua state\n");
        lua_pop(L, 1);
        return;
    }

    lua_pushnil(L);
    while(lua_next(L, -2) != 0) {
        // Key is at -2, value is at -1
        const char *name = lua_tostring(L, -2);

        lua_getfield(L, -1, "data");
        const char *data = luaL_checkstring(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "width");
        int width = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "height");
        int height = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "ntiles");
        int ntiles = luaL_checkinteger(L, -1);
        lua_pop(L, 1);

        add_sprite_in_memory(name, data, width, height, ntiles);

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}
