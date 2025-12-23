#include "types.h"
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

extern void draw(NodeDrawable *);


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

    lua_pushcfunction(globalLuaState, lua_escreva);
    lua_setfield(globalLuaState, -2, "escreva");

    lua_setglobal(globalLuaState, "ui");

    InitWindow(screenWidth, screenHeight, "Lupi Emulator");

    if (luaL_dofile(globalLuaState, "script.lua") != LUA_OK) {
        fprintf(stderr, "Erro ao carregar script.lua: %s\n", lua_tostring(globalLuaState, -1));
        lua_pop(globalLuaState, 1);
    }

    lua_getglobal(globalLuaState, "init");
    if (lua_isfunction(globalLuaState, -1)) {
        if (lua_pcall(globalLuaState, 0, 0, 0) != LUA_OK) {
            fprintf(stderr, "Erro em init(): %s\n", lua_tostring(globalLuaState, -1));
            lua_pop(globalLuaState, 1);
        }
    } else {
        lua_pop(globalLuaState, 1);
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();
    lua_close(globalLuaState);
    return 0;
}
