#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lua_api.h"
#include "ui.h"
#include "raylib.h"

lua_State *globalLuaState = NULL;

static char current_game_dir[512] = "";
static lua_Number frame_counter = 0;

//----------------------------------------------------------------------------------
// UI Functions
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// ui.draw_line(x1:number, y1:number, x2:number, y2:number, color:number)
//----------------------------------------------------------------------------------
int lua_draw_line(lua_State *L) {
    int x1 = (int)luaL_checknumber(L, 1);
    int y1 = (int)luaL_checknumber(L, 2);
    int x2 = (int)luaL_checknumber(L, 3);
    int y2 = (int)luaL_checknumber(L, 4);
    int color = (int)luaL_checknumber(L, 5);

    LineItem line = { .x1 = x1, .y1 = y1, .x2 = x2, .y2 = y2, .color_index = color };
    draw_line(&line);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_rect(x:number, y:number, width:number, height:number, filled:bool, color:number)
//----------------------------------------------------------------------------------
int lua_draw_rect(lua_State *L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);
    bool filled = lua_toboolean(L, 5);
    int color = (int)luaL_checknumber(L, 6);

    RectItem rect = { .x = x, .y = y, .width = width, .height = height, .filled = filled, .color_index = color };
    draw_rect(&rect);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.rect(x1, y1, x2, y2, color)
//----------------------------------------------------------------------------------
int lua_rect(lua_State *L) {
    int x1 = (int)luaL_checknumber(L, 1);
    int y1 = (int)luaL_checknumber(L, 2);
    int x2 = (int)luaL_checknumber(L, 3);
    int y2 = (int)luaL_checknumber(L, 4);
    int color = (int)luaL_checknumber(L, 5);

    RectItem rect = { .x = x1, .y = y1, .width = x2 - x1, .height = y2 - y1, .filled = false, .color_index = color };
    draw_rect(&rect);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.rectfill(x1, y1, x2, y2, color)
//----------------------------------------------------------------------------------
int lua_rectfill(lua_State *L) {
    int x1 = (int)luaL_checknumber(L, 1);
    int y1 = (int)luaL_checknumber(L, 2);
    int x2 = (int)luaL_checknumber(L, 3);
    int y2 = (int)luaL_checknumber(L, 4);
    int color = (int)luaL_checknumber(L, 5);

    RectItem rect = { .x = x1, .y = y1, .width = x2 - x1, .height = y2 - y1, .filled = true, .color_index = color };
    draw_rect(&rect);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_circle(center_x:number, center_y:number, radius:number, filled:bool, color:number, border:bool, border_color:number)
//----------------------------------------------------------------------------------
int lua_draw_circle(lua_State *L) {
    int center_x = (int)luaL_checknumber(L, 1);
    int center_y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);
    bool filled = lua_toboolean(L, 4);
    int color = (int)luaL_checknumber(L, 5);
    bool border = lua_toboolean(L, 6);
    int border_color = (int)luaL_checknumber(L, 7);

    CircleItem circle = {
        .center_x = center_x, .center_y = center_y, .radius = radius,
        .filled = filled, .color_index = color,
        .has_border = border, .border_color_index = border_color,
    };
    draw_circle(&circle);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.circfill(x, y, radius, color)
//----------------------------------------------------------------------------------
int lua_circfill(lua_State *L) {
    int center_x = (int)luaL_checknumber(L, 1);
    int center_y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);
    int color = (int)luaL_checknumber(L, 4);

    CircleItem circle = {
        .center_x = center_x, .center_y = center_y, .radius = radius,
        .filled = true, .color_index = color,
        .has_border = true, .border_color_index = color,
    };
    draw_circle(&circle);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.trisfill(p1_x:int, p1_y:int, p2_x:int, p2_y:int, p3_x:int, p3_y:int, color:int)
//----------------------------------------------------------------------------------
int lua_trisfill(lua_State *L) {
    int p1_x = (int)luaL_checknumber(L, 1);
    int p1_y = (int)luaL_checknumber(L, 2);
    int p2_x = (int)luaL_checknumber(L, 3);
    int p2_y = (int)luaL_checknumber(L, 4);
    int p3_x = (int)luaL_checknumber(L, 5);
    int p3_y = (int)luaL_checknumber(L, 6);
    int color = (int)luaL_checknumber(L, 7);

    TriangleItem triangle = {
        .p1_x = p1_x, .p1_y = p1_y,
        .p2_x = p2_x, .p2_y = p2_y,
        .p3_x = p3_x, .p3_y = p3_y,
        .color_index = color,
    };
    draw_triangle(&triangle);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.palset(position:number, color:number)
//----------------------------------------------------------------------------------
int lua_palset(lua_State *L) {
    int position = (int)luaL_checknumber(L, 1);
    int color = (int)luaL_checknumber(L, 2);

    palset(position, color);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.tile(spritesheet:table, tile_index:number, x:number, y:number)
// tile_index can have bit 10 (1024) set to flip horizontally
//----------------------------------------------------------------------------------
int lua_tile(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "path");
    const char *path = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "width");
    int width = (int)luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    int height = (int)luaL_checknumber(L, -1);
    lua_pop(L, 1);

    int tile_index_with_flags = (int)luaL_checknumber(L, 2);
    int x = (int)luaL_checknumber(L, 3);
    int y = (int)luaL_checknumber(L, 4);

    bool flipped = (tile_index_with_flags & 1024) != 0;
    int tile_index = tile_index_with_flags & ~1024;

    draw_tile(path, width, height, tile_index, x, y, flipped);
    return 0;
}

//----------------------------------------------------------------------------------
// ui.spr(spritesheet:table, x:number, y:number, flipped:bool = false)
//----------------------------------------------------------------------------------
int lua_spr(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);

    lua_getfield(L, 1, "path");
    const char *path = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "width");
    int width = (int)luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, 1, "height");
    int height = (int)luaL_checknumber(L, -1);
    lua_pop(L, 1);

    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);

    bool flipped = false;
    if (lua_gettop(L) > 3) {
        flipped = lua_toboolean(L, 4);
    }

    draw_spr(path, width, height, x, y, flipped);
    return 0;
}

//----------------------------------------------------------------------------------
// Helper function to get the keyboard key corresponding to a gamepad button
//----------------------------------------------------------------------------------
static KeyboardKeyData get_keyboard_key_data_for_button(int pad, int button) {
    KeyboardKeyData kbd = {-1, -1};

    if (pad != -1 && pad != keyboard_pad) { return kbd; }

    switch (button) {
        // D-pad / AWSD keys / arrow keys
        case GAMEPAD_BUTTON_LEFT_FACE_UP:
            kbd.key = KEY_W;
            kbd.alt_key = KEY_UP;
            break;
        case GAMEPAD_BUTTON_LEFT_FACE_DOWN:
            kbd.key = KEY_S;
            kbd.alt_key = KEY_DOWN;
            break;
        case GAMEPAD_BUTTON_LEFT_FACE_LEFT:
            kbd.key = KEY_A;
            kbd.alt_key = KEY_LEFT;
            break;
        case GAMEPAD_BUTTON_LEFT_FACE_RIGHT:
            kbd.key = KEY_D;
            kbd.alt_key = KEY_RIGHT;
            break;

        // Action buttons
        case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT:
            kbd.key = KEY_J;
            break;
        case GAMEPAD_BUTTON_RIGHT_FACE_DOWN:
            kbd.key = KEY_K;
            break;
        case GAMEPAD_BUTTON_RIGHT_FACE_UP:
            kbd.key = KEY_L;
            break;
        case GAMEPAD_BUTTON_RIGHT_FACE_LEFT:
            kbd.key = KEY_M;
            break;
        case GAMEPAD_BUTTON_LEFT_TRIGGER_1:
            kbd.key = KEY_G;
            break;
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_1:
            kbd.key = KEY_H;
            break;
    }
    return kbd;
}

//----------------------------------------------------------------------------------
// ui.btn(button:number, pad:number) -> bool
// Checks both gamepad and keyboard input
//----------------------------------------------------------------------------------
int lua_btn(lua_State *L) {
    int button = (int)luaL_checknumber(L, 1);
    int pad = (int)luaL_optnumber(L, 2, 0);

    bool is_down = IsGamepadButtonDown(pad, button);

    if (!is_down) {
        KeyboardKeyData key_data = get_keyboard_key_data_for_button(pad, button);
        if (key_data.key != -1) {
            is_down = IsKeyDown(key_data.key);
            if (!is_down && key_data.alt_key != -1) {
                is_down = IsKeyDown(key_data.alt_key);
            }
        }
    }

    lua_pushboolean(L, is_down);

    return 1;
}

//----------------------------------------------------------------------------------
// ui.btnp(button:number, pad:number) -> bool
// Checks both gamepad and keyboard input (pressed this frame)
//----------------------------------------------------------------------------------
int lua_btnp(lua_State *L) {
    int button = (int)luaL_checknumber(L, 1);
    int pad = (int)luaL_optnumber(L, 2, 0);

    bool is_pressed = IsGamepadButtonPressed(pad, button);

    if (!is_pressed) {
        KeyboardKeyData key_data = get_keyboard_key_data_for_button(pad, button);
        if (key_data.key != -1) {
            is_pressed = IsKeyPressed(key_data.key);
            if (!is_pressed && key_data.alt_key != -1) {
                is_pressed = IsKeyPressed(key_data.alt_key);
            }
        }
    }

    lua_pushboolean(L, is_pressed);

    return 1;
}

//----------------------------------------------------------------------------------
// ui.set_kbd(pad:number)
// Sets keyboard pad number
//----------------------------------------------------------------------------------
int lua_set_kbd(lua_State *L) {
    int pad = (int)luaL_checknumber(L, 1);
    set_keyboard_pad(pad);
    return 0;
}

//----------------------------------------------------------------------------------
// ui.cls(color:int)
//----------------------------------------------------------------------------------
int lua_cls(lua_State *L) {
    int color = (int)luaL_checknumber(L, 1);
    ClearItem clear = { .color_index = color };
    draw_clear(&clear);

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

    uint8_t bytes[8] = {0};
    for (int i = 0; i < nargs && i < 8; i++) {
        bytes[i] = (uint8_t)(int)luaL_checknumber(L, i + 1);
    }

    set_fillp(bytes, nargs);
    return 0;
}

//----------------------------------------------------------------------------------
// Helper: Recursively search for a tileset by name in the Sprites table
// Returns 1 if found (table is on top of stack), 0 otherwise
//----------------------------------------------------------------------------------
static int find_tileset_in_sprites(lua_State *L, int sprites_idx, const char *tileset_name) {
    lua_pushnil(L);
    while (lua_next(L, sprites_idx) != 0) {
        if (lua_istable(L, -1)) {
            // Check if this table has a "path" field (it's a tileset)
            lua_getfield(L, -1, "path");
            bool has_path = lua_isstring(L, -1);
            lua_pop(L, 1);

            if (has_path) {
                // This is a tileset entry, check the key name
                lua_pushvalue(L, -2);  // Push the key
                const char *key = lua_tostring(L, -1);
                lua_pop(L, 1);

                if (key && strcmp(key, tileset_name) == 0) {
                    // Found it! Move to top and return success
                    lua_insert(L, -2);  // Move found table below the key
                    lua_pop(L, 1);      // Remove the key
                    return 1;
                }
            } else {
                // This is a subdirectory, recurse into it
                if (find_tileset_in_sprites(L, lua_gettop(L), tileset_name)) {
                    lua_insert(L, -3);  // Move result below current key/value
                    lua_pop(L, 2);      // Pop key and subdirectory table
                    return 1;
                }
            }
        }
        lua_pop(L, 1);  // Pop value, keep key for next iteration
    }
    return 0;
}

//----------------------------------------------------------------------------------
// ui.map(map_data, cam_x, cam_y)
// map_data is a layer table, e.g. M.map.BG1:
//   lupi_metadata = { width, height, tile_size }
//   ["scene_a"]   = { [1]=tile_id, [2]=tile_id, ... }  (sparse, 1-indexed)
// The tileset key (e.g. "scene_a") is looked up dynamically anywhere in the
// Sprites table hierarchy to get the sprite path and dimensions.
// cam_x/cam_y: screen-space offset where the map is drawn.
//----------------------------------------------------------------------------------
int lua_map(lua_State *L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    int arg_cam_x = (int)luaL_optnumber(L, 2, 0);
    int arg_cam_y = (int)luaL_optnumber(L, 3, 0);

    // --- find metadata ---
    int meta_idx = 0;
    lua_getfield(L, 1, "metadata");
    if (lua_istable(L, -1)) {
        meta_idx = lua_absindex(L, -1);
    } else {
        lua_pop(L, 1);
        lua_getfield(L, 1, "lupi_metadata");
        if (lua_istable(L, -1)) {
            meta_idx = lua_absindex(L, -1);
        } else {
            lua_pop(L, 1);
            printf("MAP - NOT FOUND!\n");
            return 0;
        }
    }

    lua_getfield(L, meta_idx, "width");
    int map_width = (int)lua_tonumber(L, -1); lua_pop(L, 1);

    lua_getfield(L, meta_idx, "height");
    int map_height = (int)lua_tonumber(L, -1); lua_pop(L, 1);

    lua_getfield(L, meta_idx, "tile_size");
    int tile_size = (int)lua_tonumber(L, -1); lua_pop(L, 1);

    lua_pop(L, 1); // pop metadata table

    if (map_width <= 0 || map_height <= 0 || tile_size <= 0) return 0;

    // --- get Sprites table ---
    lua_getglobal(L, "Sprites");
    if (!lua_istable(L, -1)) { lua_pop(L, 1); return 0; }
    int sprites_idx = lua_absindex(L, -1);

    int tile_count = map_width * map_height;

    // --- iterate tileset keys in map_data ---
    int map_data_idx = 1;
    lua_pushnil(L);
    while (lua_next(L, map_data_idx) != 0) {
        int key_idx   = lua_absindex(L, -2);
        int value_idx = lua_absindex(L, -1);

        if (lua_type(L, key_idx) != LUA_TSTRING || !lua_istable(L, value_idx)) {
            lua_pop(L, 1); // pop value, keep key
            continue;
        }

        const char *layer_key = lua_tostring(L, key_idx);
        if (strcmp(layer_key, "metadata") == 0 ||
            strcmp(layer_key, "lupi_metadata") == 0 ||
            strcmp(layer_key, "tilesets") == 0) {
            lua_pop(L, 1); // pop value, keep key
            continue;
        }

        // resolve tileset name via tilesets indirection
        const char *tileset_name = layer_key;
        lua_getfield(L, map_data_idx, "tilesets");
        if (lua_istable(L, -1)) {
            int tilesets_idx = lua_absindex(L, -1);
            lua_getfield(L, tilesets_idx, layer_key);
            if (lua_isstring(L, -1)) {
                tileset_name = lua_tostring(L, -1);
            }
            lua_pop(L, 1); // pop resolved name
        }
        lua_pop(L, 1); // pop tilesets table

        // find tileset in Sprites
        if (!find_tileset_in_sprites(L, sprites_idx, tileset_name)) {
            lua_pop(L, 1); // pop value, keep key
            continue;
        }
        int tileset_tbl_idx = lua_absindex(L, -1);

        lua_getfield(L, tileset_tbl_idx, "path");
        const char *path = lua_tostring(L, -1); lua_pop(L, 1);

        lua_getfield(L, tileset_tbl_idx, "width");
        int tile_w = (int)lua_tonumber(L, -1); lua_pop(L, 1);

        lua_getfield(L, tileset_tbl_idx, "height");
        int tile_h = (int)lua_tonumber(L, -1); lua_pop(L, 1);

        lua_pop(L, 1); // pop tileset table

        if (!path || tile_w <= 0 || tile_h <= 0) {
            lua_pop(L, 1); // pop value, keep key
            continue;
        }

        // build tile_ids array from the Lua layer table
        int *tile_ids = (int *)malloc(tile_count * sizeof(int));
        if (!tile_ids) { lua_pop(L, 1); continue; }

        for (int i = 0; i < tile_count; i++) tile_ids[i] = -1;

        lua_pushnil(L);
        while (lua_next(L, value_idx) != 0) {
            if (lua_type(L, -2) == LUA_TNUMBER) {
                int idx = (int)lua_tonumber(L, -2) - 1;
                if (idx >= 0 && idx < tile_count) {
                    tile_ids[idx] = lua_isnil(L, -1) ? -1 : (int)lua_tonumber(L, -1);
                }
            }
            lua_pop(L, 1);
        }

        MapLayerData data = {
            .path     = path,
            .tile_w   = tile_w,
            .tile_h   = tile_h,
            .tile_ids = tile_ids,
            .count    = tile_count,
        };

        draw_map_layer(&data, map_width, tile_size, arg_cam_x, arg_cam_y);
        free(tile_ids);

        lua_pop(L, 1); // pop value, keep key
    }

    lua_pop(L, 1); // pop Sprites table
    return 0;
}

//----------------------------------------------------------------------------------
// ui.camera(x, y) — offset all drawing; ui.camera() resets to (0, 0)
//----------------------------------------------------------------------------------
int lua_camera(lua_State *L) {
    if (lua_gettop(L) == 0) {
        reset_camera();
    } else {
        int x = (int)luaL_checknumber(L, 1);
        int y = (int)luaL_checknumber(L, 2);
        set_camera(x, y);
    }
    return 0;
}

//----------------------------------------------------------------------------------
// ui.clip(x, y, w, h) — restrict drawing to region; ui.clip() resets
//----------------------------------------------------------------------------------
int lua_clip(lua_State *L) {
    if (lua_gettop(L) == 0) {
        reset_clip();
    } else {
        int x = (int)luaL_checknumber(L, 1);
        int y = (int)luaL_checknumber(L, 2);
        int w = (int)luaL_checknumber(L, 3);
        int h = (int)luaL_checknumber(L, 4);
        set_clip(x, y, w, h);
    }
    return 0;
}

//----------------------------------------------------------------------------------
// ui.preload_spritesheet(spritesheet)
//----------------------------------------------------------------------------------
int lua_preload_spritesheet(lua_State *L) {
    int spritesheet = (int)luaL_checknumber(L, 1);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.draw_sprite(x, y, sprite_index, size)
//----------------------------------------------------------------------------------
int lua_draw_sprite(lua_State *L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int sprite_index = (int)luaL_checknumber(L, 3);
    int size = (int)luaL_checknumber(L, 4);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.print(text, x, y, color)
//----------------------------------------------------------------------------------
int lua_print(lua_State *L) {
    const char *text = luaL_checkstring(L, 1);
    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);
    int color = (int)luaL_checknumber(L, 4);

    draw_print(text, x, y, color);

    return 0;
}

//----------------------------------------------------------------------------------
// ui.set_pallet(start_index, count, palette_table)
//----------------------------------------------------------------------------------
int lua_set_pallet(lua_State *L) {
    int start_index = (int)luaL_checknumber(L, 1);
    int count = (int)luaL_checknumber(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);  // Check that argument 3 is a table

    return 0;
}

//----------------------------------------------------------------------------------
// ui.mid(x, y, z) -> number
// Returns the middle value of three numbers (useful for clamping)
//----------------------------------------------------------------------------------
int lua_mid(lua_State *L) {
    lua_Number x = luaL_checknumber(L, 1);
    lua_Number y = luaL_checknumber(L, 2);
    lua_Number z = luaL_checknumber(L, 3);

    lua_Number result;
    if ((x <= y && y <= z) || (z <= y && y <= x)) {
        result = y;
    } else if ((y <= x && x <= z) || (z <= x && x <= y)) {
        result = x;
    } else {
        result = z;
    }

    lua_pushnumber(L, result);
    return 1;
}

//----------------------------------------------------------------------------------
// Global Lua State Functions
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Parse lupi_manifest.txt and inject a nested `Sprites` Lua global.
// Each bitmap entry becomes: Sprites[namespace][key] = { path, width, height, ntiles }
//----------------------------------------------------------------------------------
void inject_sprites_global(lua_State *L, const char *manifest_path, const char *game_dir) {
    FILE *f = fopen(manifest_path, "r");
    if (!f) {
        printf("Warning: Could not open manifest %s\n", manifest_path);
        return;
    }

    lua_newtable(L); // Sprites table

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        long id;
        int size;
        char path[256];
        char json[256];

        if (sscanf(line, "%ld %d %255s %255[^\n]", &id, &size, path, json) < 4)
            continue;

        if (!strstr(json, "\"type\":\"bitmap\""))
            continue;

        int width = 0, height = 0, ntiles = 1;
        char *p;

        p = strstr(json, "\"width\":");
        if (p) sscanf(p + 8, "%d", &width);

        p = strstr(json, "\"height\":");
        if (p) sscanf(p + 9, "%d", &height);

        p = strstr(json, "\"tiles\":");
        if (p) sscanf(p + 8, "%d", &ntiles);

        // Create nested table structure from path (e.g., "img/sprites/star/star_a")
        // First, duplicate path since strtok modifies it
        char path_copy[256];
        strncpy(path_copy, path, sizeof(path_copy) - 1);
        path_copy[sizeof(path_copy) - 1] = '\0';

        // Build full path for the file system
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", game_dir, path);

        // Navigate/create nested tables for each path component except the last
        char *token = strtok(path_copy, "/");
        int depth = 0;

        while (token != NULL) {
            char *next_token = strtok(NULL, "/");

            if (next_token == NULL) {
                // Last token - create the entry table
                lua_newtable(L);
                lua_pushstring(L, full_path);
                lua_setfield(L, -2, "path");
                lua_pushinteger(L, width);
                lua_setfield(L, -2, "width");
                lua_pushinteger(L, height);
                lua_setfield(L, -2, "height");
                lua_pushinteger(L, ntiles);
                lua_setfield(L, -2, "ntiles");
                lua_setfield(L, -2, token);
            } else {
                // Intermediate token - get or create sub-table
                lua_getfield(L, -1, token);
                if (!lua_istable(L, -1)) {
                    lua_pop(L, 1);
                    lua_newtable(L);
                    lua_pushvalue(L, -1);
                    lua_setfield(L, -3, token);
                }
                depth++;
            }

            token = next_token;
        }

        // Pop all nested tables to get back to Sprites
        for (int i = 0; i < depth; i++) {
            lua_pop(L, 1);
        }
    }

    fclose(f);
    lua_setglobal(L, "Sprites");
}

//----------------------------------------------------------------------------------
// Sprites preload loader — called when Lua does require("sprites")
//----------------------------------------------------------------------------------
static int lua_sprites_loader(lua_State *L) {
    char manifest_path[512];
    snprintf(manifest_path, sizeof(manifest_path), "%s/lupi_manifest.txt", current_game_dir);
    inject_sprites_global(L, manifest_path, current_game_dir);
    lua_getglobal(L, "Sprites");
    return 1;
}

//----------------------------------------------------------------------------------
// lua_api_init — create Lua state, bind ui.* table, expose button constants
//----------------------------------------------------------------------------------
void lua_api_init(void) {
    frame_counter = 0;
    globalLuaState = luaL_newstate();
    luaL_openlibs(globalLuaState);

    lua_newtable(globalLuaState);

    lua_pushcfunction(globalLuaState, lua_draw_line);
    lua_setfield(globalLuaState, -2, "line");

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

    lua_pushcfunction(globalLuaState, lua_set_kbd);
    lua_setfield(globalLuaState, -2, "set_kbd");

    lua_pushcfunction(globalLuaState, lua_log);
    lua_setfield(globalLuaState, -2, "log");

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

    lua_pushcfunction(globalLuaState, lua_map);
    lua_setfield(globalLuaState, -2, "map");

    lua_pushcfunction(globalLuaState, lua_set_pallet);
    lua_setfield(globalLuaState, -2, "set_pallet");

    lua_pushcfunction(globalLuaState, lua_fillp);
    lua_setfield(globalLuaState, -2, "fillp");

    lua_pushcfunction(globalLuaState, lua_mid);
    lua_setfield(globalLuaState, -2, "mid");

    lua_setglobal(globalLuaState, "ui");

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

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
    lua_setglobal(globalLuaState, "BTN_F");

    lua_pushinteger(globalLuaState, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);
    lua_setglobal(globalLuaState, "BTN_G");
}

//----------------------------------------------------------------------------------
// lua_api_setup_game — configure package path, register sprites preload, load game
//----------------------------------------------------------------------------------
void lua_api_setup_game(const char *game_dir) {
    snprintf(current_game_dir, sizeof(current_game_dir), "%s", game_dir);

    lua_getglobal(globalLuaState, "package");

    lua_getfield(globalLuaState, -1, "path");
    const char *current_path = lua_tostring(globalLuaState, -1);
    lua_pop(globalLuaState, 1);
    lua_pushfstring(globalLuaState, "%s;%s/?.lua", current_path, game_dir);
    lua_setfield(globalLuaState, -2, "path");

    lua_getfield(globalLuaState, -1, "preload");
    lua_pushcfunction(globalLuaState, lua_sprites_loader);
    lua_setfield(globalLuaState, -2, "sprites");
    lua_pop(globalLuaState, 2); // pop preload and package

    char game_path[512];
    snprintf(game_path, sizeof(game_path), "%s/game.lua", game_dir);

    if (luaL_dofile(globalLuaState, game_path) != LUA_OK) {
        printf("Error loading game: %s\n", lua_tostring(globalLuaState, -1));
        lua_pop(globalLuaState, 1);
    }
}

//----------------------------------------------------------------------------------
// lua_api_call_update — call the Lua update() function if it exists
//----------------------------------------------------------------------------------
void lua_api_call_update(void) {
    if (globalLuaState == NULL) return;

    lua_getglobal(globalLuaState, "update");
    if (lua_isfunction(globalLuaState, -1)) {
        lua_pushnumber(globalLuaState, frame_counter);
        frame_counter++;
        if (lua_pcall(globalLuaState, 1, 0, 0) != LUA_OK) {
            printf("Error in update(): %s\n", lua_tostring(globalLuaState, -1));
            lua_pop(globalLuaState, 1);
        }
    } else {
        lua_pop(globalLuaState, 1);
    }
}

//----------------------------------------------------------------------------------
// lua_api_close — shut down the Lua state
//----------------------------------------------------------------------------------
void lua_api_close(void) {
    lua_close(globalLuaState);
    globalLuaState = NULL;
}
