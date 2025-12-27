#include "map_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

//----------------------------------------------------------------------------------
// TMJ File Scanning
//----------------------------------------------------------------------------------

static void add_tmj_to_list(TMJList *list, const char *path, const char *name) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        list->files = realloc(list->files, list->capacity * sizeof(TMJFile));
    }

    strncpy(list->files[list->count].path, path, 255);
    list->files[list->count].path[255] = '\0';
    strncpy(list->files[list->count].name, name, 127);
    list->files[list->count].name[127] = '\0';
    list->count++;
}

static void scan_directory_recursive(const char *dir_path, const char *base_path, TMJList *list) {
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                scan_directory_recursive(full_path, base_path, list);
            } else if (S_ISREG(st.st_mode)) {
                // Check if it's a TMJ file
                size_t len = strlen(entry->d_name);
                if (len > 4 && strcmp(entry->d_name + len - 4, ".tmj") == 0) {
                    // Extract just the filename without extension
                    char name[128];
                    strncpy(name, entry->d_name, len - 4);
                    name[len - 4] = '\0';

                    add_tmj_to_list(list, full_path, name);
                }
            }
        }
    }

    closedir(dir);
}

TMJList* scan_directory_for_tmj_files(const char *directory) {
    char maps_path[512];
    snprintf(maps_path, sizeof(maps_path), "%s/maps", directory);

    TMJList *list = malloc(sizeof(TMJList));
    list->files = NULL;
    list->count = 0;
    list->capacity = 0;

    scan_directory_recursive(maps_path, maps_path, list);

    printf("[MAP] Found %d TMJ files\n", list->count);
    return list;
}

void free_tmj_list(TMJList *list) {
    if (list) {
        free(list->files);
        free(list);
    }
}

//----------------------------------------------------------------------------------
// GID Conversion
//----------------------------------------------------------------------------------

int tiled_id_to_lupi_id(unsigned int gid) {
    int flipped_horizontally = (gid & 0x80000000) != 0;
    int flipped_vertically = (gid & 0x40000000) != 0;
    unsigned int tile_id = gid & 0x1FFFFFFF;

    // lupi id is 10 bits for tile, 1 bit for flip x and 1 bit for flip y
    return (tile_id % 112) + (flipped_horizontally ? 1024 : 0) + (flipped_vertically ? 2048 : 0);
}

//----------------------------------------------------------------------------------
// JSON Parsing Helpers
//----------------------------------------------------------------------------------

static const char* find_json_value(const char *json, const char *key, int *value_len) {
    char search_key[256];
    snprintf(search_key, sizeof(search_key), "\"%s\"", key);
    const char *key_pos = strstr(json, search_key);
    if (!key_pos) return NULL;

    // Find the colon after the key
    const char *colon = strchr(key_pos, ':');
    if (!colon) return NULL;

    // Skip whitespace after colon
    const char *value_start = colon + 1;
    while (*value_start == ' ' || *value_start == '\t' || *value_start == '\n') {
        value_start++;
    }

    // Find the end of the value
    const char *value_end = value_start;
    if (*value_start == '"') {
        // String value
        value_end = strchr(value_start + 1, '"');
        if (value_end) value_end++;
    } else if (*value_start == '[') {
        // Array value - find matching bracket
        int depth = 1;
        value_end = value_start + 1;
        while (*value_end && depth > 0) {
            if (*value_end == '[') depth++;
            else if (*value_end == ']') depth--;
            value_end++;
        }
    } else if (*value_start == '{') {
        // Object value - find matching brace
        int depth = 1;
        value_end = value_start + 1;
        while (*value_end && depth > 0) {
            if (*value_end == '{') depth++;
            else if (*value_end == '}') depth--;
            value_end++;
        }
    } else {
        // Number or primitive - find next comma, }, or ]
        value_end = value_start;
        while (*value_end && *value_end != ',' && *value_end != '}' && *value_end != ']' && *value_end != '\n') {
            value_end++;
        }
    }

    if (value_len) *value_len = value_end - value_start;
    return value_start;
}

static int parse_json_int(const char *json, const char *key) {
    int value_len;
    const char *value = find_json_value(json, key, &value_len);
    if (!value) return 0;
    return atoi(value);
}

static const char* find_layer_by_name(const char *json, const char *layer_name) {
    // Find "layers" array
    const char *layers_start = strstr(json, "\"layers\"");
    if (!layers_start) return NULL;

    // Find opening bracket
    const char *array_start = strchr(layers_start, '[');
    if (!array_start) return NULL;

    // Search for layer with matching name
    char search_name[256];
    snprintf(search_name, sizeof(search_name), "\"name\":\"%s\"", layer_name);

    const char *layer_pos = strstr(array_start, search_name);
    if (!layer_pos) return NULL;

    // Find the opening brace of this layer object (go backwards)
    const char *layer_start = layer_pos;
    while (layer_start > array_start && *layer_start != '{') {
        layer_start--;
    }

    return layer_start;
}

static int* parse_data_array(const char *layer_json, int *out_count) {
    const char *data_start = strstr(layer_json, "\"data\"");
    if (!data_start) return NULL;

    const char *array_start = strchr(data_start, '[');
    if (!array_start) return NULL;

    // Count elements by counting commas + 1
    int count = 1;  // At least one element
    const char *p = array_start + 1;
    while (*p && *p != ']') {
        if (*p == ',') count++;
        p++;
    }

    // Allocate and parse
    int *data = malloc(count * sizeof(int));
    if (!data) return NULL;

    p = array_start + 1;
    int idx = 0;
    char num_buf[32];
    int num_idx = 0;
    int in_number = 0;

    while (*p && *p != ']' && idx < count) {
        if ((*p >= '0' && *p <= '9') || *p == '-') {
            if (num_idx < 31) {
                num_buf[num_idx++] = *p;
                in_number = 1;
            }
        } else if (*p == ',' || *p == ']') {
            if (in_number) {
                num_buf[num_idx] = '\0';
                data[idx++] = (int)strtol(num_buf, NULL, 10);
                num_idx = 0;
                in_number = 0;
            }
        }
        // Skip whitespace
        p++;
    }

    // Handle last number if no trailing comma
    if (in_number && idx < count) {
        num_buf[num_idx] = '\0';
        data[idx++] = (int)strtol(num_buf, NULL, 10);
    }

    if (out_count) *out_count = idx;
    return data;
}

//----------------------------------------------------------------------------------
// Map Parsing
//----------------------------------------------------------------------------------

MapData* parse_tiled_json(const char *json_content, const char *map_name) {
    MapData *map = malloc(sizeof(MapData));
    if (!map) return NULL;

    strncpy(map->name, map_name, 127);
    map->name[127] = '\0';
    map->tiles_data = NULL;
    map->colision_data = NULL;
    map->pois_data = NULL;
    map->overlay_data = NULL;

    // Find tiles layer
    const char *tiles_layer = find_layer_by_name(json_content, "tiles");
    if (!tiles_layer) {
        printf("[MAP] Warning: No tiles layer found in %s\n", map_name);
        free(map);
        return NULL;
    }

    // Get width and height from tiles layer
    map->width = parse_json_int(tiles_layer, "width");
    map->height = parse_json_int(tiles_layer, "height");

    if (map->width <= 0 || map->height <= 0) {
        printf("[MAP] Warning: Invalid dimensions in %s\n", map_name);
        free(map);
        return NULL;
    }

    // Parse tiles data
    int tiles_count;
    map->tiles_data = parse_data_array(tiles_layer, &tiles_count);
    if (!map->tiles_data || tiles_count != map->width * map->height) {
        printf("[MAP] Warning: Invalid tiles data in %s (expected %d, got %d)\n",
               map_name, map->width * map->height, tiles_count);
        if (map->tiles_data) free(map->tiles_data);
        free(map);
        return NULL;
    }

    // Find and parse optional layers
    const char *colision_layer = find_layer_by_name(json_content, "colision");
    if (colision_layer) {
        int colision_count;
        map->colision_data = parse_data_array(colision_layer, &colision_count);
        if (colision_count != map->width * map->height) {
            printf("[MAP] Warning: Colision data size mismatch in %s\n", map_name);
            if (map->colision_data) {
                free(map->colision_data);
                map->colision_data = NULL;
            }
        }
    }

    const char *pois_layer = find_layer_by_name(json_content, "pois");
    if (pois_layer) {
        int pois_count;
        map->pois_data = parse_data_array(pois_layer, &pois_count);
        if (pois_count != map->width * map->height) {
            printf("[MAP] Warning: POIs data size mismatch in %s\n", map_name);
            if (map->pois_data) {
                free(map->pois_data);
                map->pois_data = NULL;
            }
        }
    }

    const char *overlay_layer = find_layer_by_name(json_content, "overlay");
    if (overlay_layer) {
        int overlay_count;
        map->overlay_data = parse_data_array(overlay_layer, &overlay_count);
        if (overlay_count != map->width * map->height) {
            printf("[MAP] Warning: Overlay data size mismatch in %s\n", map_name);
            if (map->overlay_data) {
                free(map->overlay_data);
                map->overlay_data = NULL;
            }
        }
    }

    printf("[MAP] Parsed map %s: %dx%d\n", map_name, map->width, map->height);
    return map;
}

void free_map_data(MapData *map) {
    if (map) {
        if (map->tiles_data) free(map->tiles_data);
        if (map->colision_data) free(map->colision_data);
        if (map->pois_data) free(map->pois_data);
        if (map->overlay_data) free(map->overlay_data);
        free(map);
    }
}

//----------------------------------------------------------------------------------
// Lua Table Building
//----------------------------------------------------------------------------------

void build_lua_map_table(lua_State *L, MapData *map) {
    // Create the main table: {[y] = {[x] = {t = tile_id, c = colision, p = poi, o = overlay}}}
    lua_newtable(L);

    for (int y = 1; y <= map->height; y++) {
        lua_newtable(L);  // Row table

        for (int x = 1; x <= map->width; x++) {
            int tile_index = (y - 1) * map->width + (x - 1);

            int tile_value = map->tiles_data[tile_index];
            int tile_id = tiled_id_to_lupi_id(tile_value);

            int colision_value = map->colision_data ? map->colision_data[tile_index] : 0;
            int poi_value = map->pois_data ? map->pois_data[tile_index] : 0;
            int overlay_value = map->overlay_data ? map->overlay_data[tile_index] : 0;

            if (overlay_value != 0) {
                overlay_value = tiled_id_to_lupi_id(overlay_value);
            }

            // Only create cell if it has data (matching encoder logic)
            // Encoder checks: (tile_id ~= 0 and tile_id ~= nil) or (poi_id ~= 0 and poi_id ~= nil)
            int tile_id_base = tile_id & 0x3FF;
            int poi_id_base = (poi_value != 0) ? (poi_value & 0x3FF) : 0;
            int overlay_id_base = overlay_value & 0x3FF;

            if ((tile_id_base != 0 && tile_id != 0) || (poi_id_base != 0 && poi_value != 0)) {
                lua_newtable(L);  // Cell table

                // [1] = tile_id (always first element, even if 0)
                // Encoder converts 0 to "nil" string, but we'll use integer 0
                lua_pushinteger(L, tile_id);
                lua_rawseti(L, -2, 1);

                // [2] = colision (if non-zero)
                if (colision_value != 0) {
                    lua_pushinteger(L, colision_value);
                    lua_rawseti(L, -2, 2);
                }

                // [3] = poi (if non-zero)
                if (poi_id_base != 0) {
                    lua_pushinteger(L, poi_value);
                    lua_rawseti(L, -2, 3);
                }

                // [4] = overlay (if non-zero)
                if (overlay_id_base != 0) {
                    lua_pushinteger(L, overlay_value);
                    lua_rawseti(L, -2, 4);
                }

                // Set cell in row: row[x] = cell
                lua_rawseti(L, -2, x);
            }
        }

        // Set row in main table: main[y] = row
        lua_rawseti(L, -2, y);
    }
}

//----------------------------------------------------------------------------------
// Map Registration
//----------------------------------------------------------------------------------

// Lua loader function for a specific map
static int lua_loader_map(lua_State *L) {
    // Get the map name from the closure
    const char *map_name = lua_tostring(L, lua_upvalueindex(1));

    // Get the map data from the registry
    lua_getfield(L, LUA_REGISTRYINDEX, "map_data");
    lua_getfield(L, -1, map_name);
    MapData *map = (MapData*)lua_touserdata(L, -1);
    lua_pop(L, 2);  // Remove map_data table and userdata

    if (!map) {
        lua_pushstring(L, "Map data not found");
        lua_error(L);
        return 0;
    }

    // Build and return the Lua table
    build_lua_map_table(L, map);
    return 1;
}

void register_map_in_lua(lua_State *L, const char *map_name, MapData *map) {
    // Store map data in registry
    lua_getfield(L, LUA_REGISTRYINDEX, "map_data");
    if (lua_isnil(L, -1)) {
        // Create map_data table in registry
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setfield(L, LUA_REGISTRYINDEX, "map_data");
        lua_getfield(L, LUA_REGISTRYINDEX, "map_data");
    }

    // Store map as userdata
    MapData **ud = (MapData**)lua_newuserdata(L, sizeof(MapData*));
    *ud = map;
    lua_setfield(L, -2, map_name);
    lua_pop(L, 1);  // Remove map_data table

    // Register loader in package.preload
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");

    // Create closure with map name
    lua_pushstring(L, map_name);
    lua_pushcclosure(L, lua_loader_map, 1);
    lua_setfield(L, -2, map_name);

    lua_pop(L, 2);  // Remove package and preload tables

    printf("[MAP] Registered map '%s' in package.preload\n", map_name);
}

//----------------------------------------------------------------------------------
// Main Initialization
//----------------------------------------------------------------------------------

void initialize_map_system(lua_State *L, const char *game_dir) {
    printf("[MAP] Initializing map system for: %s\n", game_dir);

    // Step 1: Scan for TMJ files
    TMJList *tmj_list = scan_directory_for_tmj_files(game_dir);
    if (tmj_list->count == 0) {
        printf("[MAP] No TMJ files found\n");
        free_tmj_list(tmj_list);
        return;
    }

    // Step 2: Parse and register each map
    for (int i = 0; i < tmj_list->count; i++) {
        // Read file
        FILE *file = fopen(tmj_list->files[i].path, "r");
        if (!file) {
            printf("[MAP] Failed to open: %s\n", tmj_list->files[i].path);
            continue;
        }

        // Get file size
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Read content
        char *content = malloc(file_size + 1);
        if (!content) {
            fclose(file);
            continue;
        }

        size_t read_size = fread(content, 1, file_size, file);
        content[read_size] = '\0';
        fclose(file);

        // Parse JSON
        MapData *map = parse_tiled_json(content, tmj_list->files[i].name);
        free(content);

        if (map) {
            // Register in Lua
            register_map_in_lua(L, tmj_list->files[i].name, map);
        }
    }

    // Cleanup
    free_tmj_list(tmj_list);

    printf("[MAP] Map system initialized\n");
}
