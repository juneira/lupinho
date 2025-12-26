#ifndef SPRITE_LOADER_H
#define SPRITE_LOADER_H

#include <stdbool.h>
#include "types.h"

// Color conversion
int rgb_to_bgr555(int r, int g, int b);
void bgr555_to_rgb(int bgr555, int *r, int *g, int *b);

// PNG file scanning
typedef struct {
    char path[256];
    char name[128];
} PNGFile;

typedef struct {
    PNGFile *files;
    int count;
    int capacity;
} PNGList;

PNGList* scan_directory_for_pngs(const char *directory);
void free_png_list(PNGList *list);

// Color extraction
typedef struct {
    int *colors;
    int count;
    int capacity;
} ColorSet;

ColorSet* extract_unique_colors_from_pngs(PNGList *png_list);
void free_color_set(ColorSet *set);

// Palette management
bool load_existing_palette(const char *path, ColorSet *palette);
void merge_palettes(ColorSet *existing, ColorSet *new_colors);
void generate_palette_file(const char *path, ColorSet *palette);

// Sprite indexing
typedef struct {
    int width;
    int height;
    int base_tile_width;
    int base_tile_height;
    int *pixels;  // BGR555 pixels
} ImageData;

ImageData* load_png_file(const char *path);
void free_image_data(ImageData *img);
void find_base_tile_sizing(ImageData *img, int *tile_width, int *tile_height);
void index_sprite(ImageData *img, ColorSet *palette, int sprite_index);

// Main initialization
void initialize_sprite_system(const char *game_dir);

// Sprite registry access (for Lua)
void register_sprite(const char *name, int index);
int get_sprite_index(const char *name);
int get_sprite_count();
const char* get_sprite_name_at(int idx);
int get_sprite_index_at(int idx);

#endif
