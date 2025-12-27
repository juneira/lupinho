#include "drawlist.h"
#include "sprite_loader.h"
#include "map_loader.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "raylib.h"

/**
Global objects
**/
Drawlist drawlist;
lua_State *globalLuaState = NULL;

// Sprite system globals
int sprit_current_index = 0;
SpriteItem sprite_sheet[MAX_SPRITE_SHEETS];

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
                printf("Error in update(): %s\n", lua_tostring(globalLuaState, -1));
                lua_pop(globalLuaState, 1);
            } else {
                printf("Update successful\n");
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

    lua_pushcfunction(globalLuaState, lua_palset);
    lua_setfield(globalLuaState, -2, "palset");

    lua_pushcfunction(globalLuaState, lua_tile);
    lua_setfield(globalLuaState, -2, "tile");

    lua_pushcfunction(globalLuaState, lua_btn);
    lua_setfield(globalLuaState, -2, "btn");

    lua_pushcfunction(globalLuaState, lua_btnp);
    lua_setfield(globalLuaState, -2, "btnp");

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

    // Initialize sprite system before loading game
    initialize_sprite_system("game-example");

    // Initialize map system
    initialize_map_system(globalLuaState, "game-example");

    // Add game-example directory to Lua's package.path so require() can find modules there
    lua_getglobal(globalLuaState, "package");
    lua_getfield(globalLuaState, -1, "path");
    const char *current_path = lua_tostring(globalLuaState, -1);
    lua_pop(globalLuaState, 1);

    lua_pushfstring(globalLuaState, "%s;./game-example/?.lua", current_path);
    lua_setfield(globalLuaState, -2, "path");

    // Register sprites loader in package.preload
    lua_getfield(globalLuaState, -1, "preload");
    lua_pushcfunction(globalLuaState, lua_require_sprites);
    lua_setfield(globalLuaState, -2, "sprites");
    lua_pop(globalLuaState, 2);

    if (luaL_dofile(globalLuaState, "game-example/game.lua") != LUA_OK) {
        printf("Error loading game-example/game.lua: %s\n", lua_tostring(globalLuaState, -1));
        lua_pop(globalLuaState, 1);
    } else {
        printf("Game-example/game.lua loaded successfully\n");
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
