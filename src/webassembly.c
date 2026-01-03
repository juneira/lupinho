#include "drawlist.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "raylib.h"

/**
Global objects
**/
Drawlist drawlist;
lua_State *globalLuaState = NULL;

/**
Constants
**/
const int screenWidth = 480;
const int screenHeight = 270;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame(void)
{
    if (globalLuaState != NULL) {
        lua_getglobal(globalLuaState, "update");
        if (lua_isfunction(globalLuaState, -1)) {
            if (lua_pcall(globalLuaState, 0, 0, 0) != LUA_OK) {
                printf("Error in update(): %s\n", lua_tostring(globalLuaState, -1));
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

    clear_drawlist();
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

    lua_pushcfunction(globalLuaState, lua_rect);
    lua_setfield(globalLuaState, -2, "rect");

    lua_pushcfunction(globalLuaState, lua_rectfill);
    lua_setfield(globalLuaState, -2, "rectfill");

    lua_pushcfunction(globalLuaState, lua_draw_circle);
    lua_setfield(globalLuaState, -2, "draw_circle");

    lua_pushcfunction(globalLuaState, lua_circfill);
    lua_setfield(globalLuaState, -2, "circfill");

    lua_pushcfunction(globalLuaState, lua_trisfill);
    lua_setfield(globalLuaState, -2, "trisfill");

    lua_pushcfunction(globalLuaState, lua_palset);
    lua_setfield(globalLuaState, -2, "palset");

    lua_pushcfunction(globalLuaState, lua_tile);
    lua_setfield(globalLuaState, -2, "tile");

    lua_pushcfunction(globalLuaState, lua_spr);
    lua_setfield(globalLuaState, -2, "spr");

    lua_pushcfunction(globalLuaState, lua_btn);
    lua_setfield(globalLuaState, -2, "btn");

    lua_pushcfunction(globalLuaState, lua_btnp);
    lua_setfield(globalLuaState, -2, "btnp");

    lua_pushcfunction(globalLuaState, lua_log);
    lua_setfield(globalLuaState, -2, "log");

    // TODO
    lua_pushcfunction(globalLuaState, lua_camera);
    lua_setfield(globalLuaState, -2, "camera");

    lua_pushcfunction(globalLuaState, lua_clip);
    lua_setfield(globalLuaState, -2, "clip");

    lua_pushcfunction(globalLuaState, lua_cls);
    lua_setfield(globalLuaState, -2, "cls");

    lua_pushcfunction(globalLuaState, lua_preload_spritesheet);
    lua_setfield(globalLuaState, -2, "preload_spritesheet");

    lua_pushcfunction(globalLuaState, lua_draw_sprite);
    lua_setfield(globalLuaState, -2, "draw_sprite");

    lua_pushcfunction(globalLuaState, lua_print);
    lua_setfield(globalLuaState, -2, "print");

    lua_pushcfunction(globalLuaState, lua_set_pallet);
    lua_setfield(globalLuaState, -2, "set_pallet");

    lua_pushcfunction(globalLuaState, lua_fillp);
    lua_setfield(globalLuaState, -2, "fillp");

    lua_setglobal(globalLuaState, "ui");

    // Expose button constants as globals
    // These match common gamepad button conventions
    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
    lua_setglobal(globalLuaState, "RIGHT");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
    lua_setglobal(globalLuaState, "LEFT");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_LEFT_FACE_UP);
    lua_setglobal(globalLuaState, "UP");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
    lua_setglobal(globalLuaState, "DOWN");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
    lua_setglobal(globalLuaState, "BTN_Z");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_RIGHT_FACE_LEFT);
    lua_setglobal(globalLuaState, "BTN_E");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_RIGHT_FACE_UP);
    lua_setglobal(globalLuaState, "BTN_Q");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
    lua_setglobal(globalLuaState, "BTN_Z");

    InitWindow(screenWidth, screenHeight, "Lupi Emulator");

    // Add game-example directory to Lua's package.path so require() can find modules there
    lua_getglobal(globalLuaState, "package");
    lua_getfield(globalLuaState, -1, "path");
    const char *current_path = lua_tostring(globalLuaState, -1);
    lua_pop(globalLuaState, 1);

    lua_pushfstring(globalLuaState, "%s;./game-example/?.lua", current_path);
    lua_setfield(globalLuaState, -2, "path");

    if (luaL_dofile(globalLuaState, "game-example/game.lua") != LUA_OK) {
        printf("Error loading game-example/game.lua: %s\n", lua_tostring(globalLuaState, -1));
        lua_pop(globalLuaState, 1);
    } else {
        printf("Game-example/game.lua loaded successfully\n");
        load_palette_from_lua(globalLuaState);
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
