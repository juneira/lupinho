#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <stdbool.h>
#include <lua.h>

// TMJ file scanning
typedef struct {
    char path[256];
    char name[128];
} TMJFile;

typedef struct {
    TMJFile *files;
    int count;
    int capacity;
} TMJList;

TMJList* scan_directory_for_tmj_files(const char *directory);
void free_tmj_list(TMJList *list);

// Map data structure
typedef struct {
    char name[128];
    int width;
    int height;
    int *tiles_data;      // 1D array: tiles[y * width + x]
    int *colision_data;    // Optional, same size as tiles_data
    int *pois_data;        // Optional, same size as tiles_data
    int *overlay_data;     // Optional, same size as tiles_data
} MapData;

// JSON parsing
MapData* parse_tiled_json(const char *json_content, const char *map_name);
void free_map_data(MapData *map);

// GID conversion
int tiled_id_to_lupi_id(unsigned int gid);

// Lua integration
void build_lua_map_table(lua_State *L, MapData *map);
void register_map_in_lua(lua_State *L, const char *map_name, MapData *map);

// Main initialization
void initialize_map_system(lua_State *L, const char *game_dir);

#endif
