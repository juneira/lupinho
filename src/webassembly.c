#include "drawlist.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/**
Global objects
**/
Drawlist drawlist;
lua_State *globalLuaState = NULL;

/**
Constants
**/
const int screenWidth = 800;
const int screenHeight = 450;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame(void)
{
    if (globalLuaState != NULL) {
        lua_getglobal(globalLuaState, "update");
        if (lua_isfunction(globalLuaState, -1)) {
            if (lua_pcall(globalLuaState, 0, 0, 0) != LUA_OK) {
                fprintf(stderr, "Erro em update(): %s\n", lua_tostring(globalLuaState, -1));
                lua_pop(globalLuaState, 1);
            }
        } else {
            lua_pop(globalLuaState, 1);
        }
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    NodeDrawable *node = drawlist.root;

    for (int i = 0; i < drawlist.count; i++) {
        draw(node);
        node = node->next;
    }

    clear_drawlist();

    #ifndef PRODUCTION
        DrawFPS(10, 10); // DEBUG
    #endif

    EndDrawing();
}


int main(void)
{
    globalLuaState = luaL_newstate();
    luaL_openlibs(globalLuaState);

    lua_newtable(globalLuaState);

    lua_pushcfunction(globalLuaState, lua_draw_text);
    lua_setfield(globalLuaState, -2, "draw_text");

    lua_pushcfunction(globalLuaState, lua_draw_line);
    lua_setfield(globalLuaState, -2, "draw_line");

    lua_pushcfunction(globalLuaState, lua_draw_rect);
    lua_setfield(globalLuaState, -2, "draw_rect");

    lua_pushcfunction(globalLuaState, lua_draw_circle);
    lua_setfield(globalLuaState, -2, "draw_circle");

    lua_pushcfunction(globalLuaState, lua_draw_triangle);
    lua_setfield(globalLuaState, -2, "draw_triangle");

    lua_setglobal(globalLuaState, "ui");

    InitWindow(screenWidth, screenHeight, "Lupi Emulator");

    if (luaL_dofile(globalLuaState, "script.lua") != LUA_OK) {
        fprintf(stderr, "Erro ao carregar script.lua: %s\n", lua_tostring(globalLuaState, -1));
        lua_pop(globalLuaState, 1);
    }

    SetTargetFPS(60);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();
    lua_close(globalLuaState);
    return 0;
}
