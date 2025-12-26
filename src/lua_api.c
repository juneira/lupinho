#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "drawlist.h"

//----------------------------------------------------------------------------------
// ui.draw_text(texto:string, x:int, y:int)
//----------------------------------------------------------------------------------
int lua_draw_text(lua_State *L) {
    char *text = luaL_checkstring(L, 1);
    int x = luaL_optinteger(L, 2, 10);
    int y = luaL_optinteger(L, 3, 10);

    add_text(text, x, y);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_line(x1:int, y1:int, x2:int, y2:int, cor:int)
//----------------------------------------------------------------------------------
int lua_draw_line(lua_State *L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    int cor = luaL_checkinteger(L, 5);

    add_line(x1, y1, x2, y2, get_palette_color(cor));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_rect(x:int, y:int, largura:int, altura:int, prenchido:bool, cor:int)
//----------------------------------------------------------------------------------
int lua_draw_rect(lua_State *L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int largura = luaL_checkinteger(L, 3);
    int altura = luaL_checkinteger(L, 4);
    bool prenchido = lua_toboolean(L, 5);
    int cor = luaL_checkinteger(L, 6);

    add_rect(x, y, largura, altura, prenchido, get_palette_color(cor));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_circle(centro_x:int, centro_y:int, raio:int, prenchido:bool, cor:int, borda:bool, cor:int)
//----------------------------------------------------------------------------------
int lua_draw_circle(lua_State *L) {
    int centro_x = luaL_checkinteger(L, 1);
    int centro_y = luaL_checkinteger(L, 2);
    int raio = luaL_checkinteger(L, 3);
    bool prenchido = lua_toboolean(L, 4);
    int cor = luaL_checkinteger(L, 5);
    bool borda = lua_toboolean(L, 6);
    int cor_borda = luaL_checkinteger(L, 7);

    add_circle(centro_x, centro_y, raio, prenchido, get_palette_color(cor), borda, get_palette_color(cor_borda));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_triangle(p1_x:int, p1_y:int, p2_x:int, p2_y:int, p3_x:int, p3_y:int, cor:int)
//----------------------------------------------------------------------------------
int lua_draw_triangle(lua_State *L) {
    int p1_x = luaL_checkinteger(L, 1);
    int p1_y = luaL_checkinteger(L, 2);
    int p2_x = luaL_checkinteger(L, 3);
    int p2_y = luaL_checkinteger(L, 4);
    int p3_x = luaL_checkinteger(L, 5);
    int p3_y = luaL_checkinteger(L, 6);
    int cor = luaL_checkinteger(L, 7);

    add_triangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, get_palette_color(cor));

    return 0;
}

//----------------------------------------------------------------------------------
// ui.palset(posicao:int, cor:int)
//----------------------------------------------------------------------------------
int lua_palset(lua_State *L) {
    int posicao = luaL_checkinteger(L, 1);
    int cor = luaL_checkinteger(L, 2);

    palset(posicao, cor);

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
