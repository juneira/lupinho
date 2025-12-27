#include "sprite_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "raylib.h"

#define MAGIC_GRAY_BGR555 8456  // 0x424242 in RGB

// Global sprite registry
typedef struct {
    char name[128];
    int index;
} SpriteRegistryEntry;

static SpriteRegistryEntry sprite_registry[MAX_SPRITE_SHEETS];
static int sprite_registry_count = 0;

// External references
extern int sprit_current_index;
extern SpriteItem sprite_sheet[MAX_SPRITE_SHEETS];

//----------------------------------------------------------------------------------
// Color Conversion Functions
//----------------------------------------------------------------------------------

int rgb_to_bgr555(int r, int g, int b) {
    return ((r >> 3) & 0x1F) | (((g >> 3) & 0x1F) << 5) | (((b >> 3) & 0x1F) << 10);
}

void bgr555_to_rgb(int bgr555, int *r, int *g, int *b) {
    int r5 = (bgr555 >> 0) & 0x1F;
    int g5 = (bgr555 >> 5) & 0x1F;
    int b5 = (bgr555 >> 10) & 0x1F;

    *r = (r5 << 3) | (r5 >> 2);
    *g = (g5 << 3) | (g5 >> 2);
    *b = (b5 << 3) | (b5 >> 2);
}

//----------------------------------------------------------------------------------
// PNG File Scanning
//----------------------------------------------------------------------------------

static void add_png_to_list(PNGList *list, const char *path, const char *name) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        list->files = realloc(list->files, list->capacity * sizeof(PNGFile));
    }

    strncpy(list->files[list->count].path, path, 255);
    list->files[list->count].path[255] = '\0';
    strncpy(list->files[list->count].name, name, 127);
    list->files[list->count].name[127] = '\0';
    list->count++;
}

static void scan_directory_recursive(const char *dir_path, const char *base_path, PNGList *list) {
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
                // Check if it's a PNG file
                size_t len = strlen(entry->d_name);
                if (len > 4 && strcmp(entry->d_name + len - 4, ".png") == 0) {
                    // Extract just the filename without extension
                    char name[128];
                    strncpy(name, entry->d_name, len - 4);
                    name[len - 4] = '\0';

                    add_png_to_list(list, full_path, name);
                }
            }
        }
    }

    closedir(dir);
}

PNGList* scan_directory_for_pngs(const char *directory) {
    PNGList *list = malloc(sizeof(PNGList));
    list->files = NULL;
    list->count = 0;
    list->capacity = 0;

    scan_directory_recursive(directory, directory, list);

    printf("[SPRITE] Found %d PNG files\n", list->count);
    return list;
}

void free_png_list(PNGList *list) {
    if (list) {
        free(list->files);
        free(list);
    }
}

//----------------------------------------------------------------------------------
// Color Set Management
//----------------------------------------------------------------------------------

static ColorSet* create_color_set() {
    ColorSet *set = malloc(sizeof(ColorSet));
    set->colors = malloc(16 * sizeof(int));
    set->count = 1;
    set->capacity = 16;
    set->colors[0] = 0x0000;  // Always start with black
    return set;
}

static void add_color_to_set(ColorSet *set, int color) {
    // Check if color already exists
    for (int i = 0; i < set->count; i++) {
        if (set->colors[i] == color) return;
    }

    // Add new color
    if (set->count >= set->capacity) {
        set->capacity *= 2;
        set->colors = realloc(set->colors, set->capacity * sizeof(int));
    }

    set->colors[set->count++] = color;
}

void free_color_set(ColorSet *set) {
    if (set) {
        free(set->colors);
        free(set);
    }
}

//----------------------------------------------------------------------------------
// Image Loading
//----------------------------------------------------------------------------------

ImageData* load_png_file(const char *path) {
    Image img = LoadImage(path);
    if (img.data == NULL) {
        printf("[SPRITE] Failed to load: %s\n", path);
        return NULL;
    }

    // Convert to RGBA8 if not already
    if (img.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8) {
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    }

    ImageData *data = malloc(sizeof(ImageData));
    data->width = img.width;
    data->height = img.height;
    data->pixels = malloc(img.width * img.height * sizeof(int));

    unsigned char *pixels = (unsigned char *)img.data;
    for (int i = 0; i < img.width * img.height; i++) {
        int r = pixels[i * 4 + 0];
        int g = pixels[i * 4 + 1];
        int b = pixels[i * 4 + 2];
        int a = pixels[i * 4 + 3];

        if (a < 128) {
            data->pixels[i] = 0x0000;  // Transparent becomes black
        } else {
            data->pixels[i] = rgb_to_bgr555(r, g, b);
        }
    }

    UnloadImage(img);
    return data;
}

void free_image_data(ImageData *img) {
    if (img) {
        free(img->pixels);
        free(img);
    }
}

//----------------------------------------------------------------------------------
// Tile Size Detection
//----------------------------------------------------------------------------------

void find_base_tile_sizing(ImageData *img, int *tile_width, int *tile_height) {
    *tile_width = img->width;
    *tile_height = img->height;

    // Check if top-left pixel is magic color
    if (img->pixels[0] != MAGIC_GRAY_BGR555) {
        return;
    }

    // Count consecutive magic pixels in first row
    for (int x = 0; x < img->width; x++) {
        if (img->pixels[x] != MAGIC_GRAY_BGR555) {
            *tile_width = x;
            break;
        }
    }

    // Count consecutive magic pixels in first column
    for (int y = 0; y < img->height; y++) {
        if (img->pixels[y * img->width] != MAGIC_GRAY_BGR555) {
            *tile_height = y;
            break;
        }
    }
}

//----------------------------------------------------------------------------------
// Color Extraction
//----------------------------------------------------------------------------------

ColorSet* extract_unique_colors_from_pngs(PNGList *png_list) {
    ColorSet *colors = create_color_set();

    for (int i = 0; i < png_list->count; i++) {
        ImageData *img = load_png_file(png_list->files[i].path);
        if (!img) continue;

        for (int j = 0; j < img->width * img->height; j++) {
            add_color_to_set(colors, img->pixels[j]);
        }

        free_image_data(img);
    }

    printf("[SPRITE] Extracted %d unique colors\n", colors->count);
    return colors;
}

//----------------------------------------------------------------------------------
// Palette File Management
//----------------------------------------------------------------------------------

bool load_existing_palette(const char *path, ColorSet *palette) {
    FILE *file = fopen(path, "r");
    if (!file) return false;

    char line[4096];
    bool found = false;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Palette") && strstr(line, "{")) {
            found = true;
            char *ptr = strchr(line, '{');
            if (ptr) {
                ptr++;
                char *token = strtok(ptr, ",}");
                while (token) {
                    int color;
                    if (sscanf(token, "%i", &color) == 1 || sscanf(token, "0x%x", &color) == 1) {
                        add_color_to_set(palette, color);
                    }
                    token = strtok(NULL, ",}");
                }
            }
            break;
        }
    }

    fclose(file);
    return found;
}

void merge_palettes(ColorSet *existing, ColorSet *new_colors) {
    for (int i = 0; i < new_colors->count; i++) {
        add_color_to_set(existing, new_colors->colors[i]);
    }
}

void generate_palette_file(const char *path, ColorSet *palette) {
    if (palette->count > 256) {
        printf("[SPRITE] Warning: Palette has %d colors, limiting to 256\n", palette->count);
        palette->count = 256;
    }

    FILE *file = fopen(path, "w");
    if (!file) {
        printf("[SPRITE] Failed to create palette file: %s\n", path);
        return;
    }

    fprintf(file, "Palette = {");
    for (int i = 0; i < palette->count; i++) {
        if (i > 0) fprintf(file, ", ");
        fprintf(file, "0x%04X", palette->colors[i]);
    }
    fprintf(file, "}\n");

    fclose(file);
    printf("[SPRITE] Generated palette with %d colors at %s\n", palette->count, path);
}

//----------------------------------------------------------------------------------
// Sprite Indexing
//----------------------------------------------------------------------------------

void index_sprite(ImageData *img, ColorSet *palette, int sprite_index) {
    int tile_width, tile_height;
    find_base_tile_sizing(img, &tile_width, &tile_height);

    sprite_sheet[sprite_index].width = tile_width;
    sprite_sheet[sprite_index].height = tile_height;

    bool has_magic = (img->pixels[0] == MAGIC_GRAY_BGR555);

    // Calculate tiles
    int tiles_x = img->width / tile_width;
    int tiles_y = img->height / tile_height;
    int pixel_count = 0;

    // Iterate over tiles
    for (int ty = 0; ty < tiles_y; ty++) {
        for (int tx = 0; tx < tiles_x; tx++) {
            // Skip margin tiles if magic color is present
            if (has_magic && (tx == 0 || ty == 0)) {
                continue;
            }

            // Index each pixel in the tile
            for (int py = 0; py < tile_height; py++) {
                for (int px = 0; px < tile_width; px++) {
                    int img_x = tx * tile_width + px;
                    int img_y = ty * tile_height + py;
                    int color = img->pixels[img_y * img->width + img_x];

                    // Find color in palette
                    int palette_index = 0;
                    bool found = false;
                    for (int i = 0; i < palette->count; i++) {
                        if (palette->colors[i] == color) {
                            palette_index = i;
                            found = true;
                            break;
                        }
                    }

                    if (pixel_count < MAX_TILES_PER_SPRITE) {
                        sprite_sheet[sprite_index].pallet_index[pixel_count++] = palette_index;
                    }
                }
            }
        }
    }

    sprite_sheet[sprite_index].tile_count = pixel_count / (tile_width * tile_height);

    printf("[SPRITE] Indexed %d pixels for sprite %d\n", pixel_count, sprite_index);
}

//----------------------------------------------------------------------------------
// Sprite Registry
//----------------------------------------------------------------------------------

void register_sprite(const char *name, int index) {
    if (sprite_registry_count < MAX_SPRITE_SHEETS) {
        strncpy(sprite_registry[sprite_registry_count].name, name, 127);
        sprite_registry[sprite_registry_count].name[127] = '\0';
        sprite_registry[sprite_registry_count].index = index;
        sprite_registry_count++;
    }
}

int get_sprite_index(const char *name) {
    for (int i = 0; i < sprite_registry_count; i++) {
        if (strcmp(sprite_registry[i].name, name) == 0) {
            return sprite_registry[i].index;
        }
    }
    return -1;
}

int get_sprite_count() {
    return sprite_registry_count;
}

const char* get_sprite_name_at(int idx) {
    if (idx >= 0 && idx < sprite_registry_count) {
        return sprite_registry[idx].name;
    }
    return NULL;
}

int get_sprite_index_at(int idx) {
    if (idx >= 0 && idx < sprite_registry_count) {
        return sprite_registry[idx].index;
    }
    return -1;
}

//----------------------------------------------------------------------------------
// Main Initialization
//----------------------------------------------------------------------------------

void initialize_sprite_system(const char *game_dir) {
    printf("[SPRITE] Initializing sprite system for: %s\n", game_dir);

    // Step 1: Scan for PNG files
    PNGList *png_list = scan_directory_for_pngs(game_dir);
    if (png_list->count == 0) {
        printf("[SPRITE] No PNG files found\n");
        free_png_list(png_list);
        return;
    }

    // Step 2: Extract unique colors
    ColorSet *unique_colors = extract_unique_colors_from_pngs(png_list);

    // Step 3: Load existing palette or create new one
    char palette_path[512];
    snprintf(palette_path, sizeof(palette_path), "%s/palette.lua", game_dir);

    ColorSet *palette = create_color_set();
    bool loaded = false;

    // Try loading palette.lua first, then pallet.lua (for backward compatibility)
    if (load_existing_palette(palette_path, palette)) {
        printf("[SPRITE] Loaded existing palette.lua with %d colors\n", palette->count);
        loaded = true;
    }

    if (loaded) {
        merge_palettes(palette, unique_colors);
        printf("[SPRITE] Merged palette now has %d colors\n", palette->count);
    } else {
        printf("[SPRITE] No existing palette, creating new one\n");
        free_color_set(palette);
        palette = unique_colors;
        unique_colors = NULL;
    }

    // Step 4: Generate palette file
    generate_palette_file(palette_path, palette);

    // Step 5: Index all sprites
    for (int i = 0; i < png_list->count && sprit_current_index < MAX_SPRITE_SHEETS; i++) {
        ImageData *img = load_png_file(png_list->files[i].path);
        if (img) {
            index_sprite(img, palette, sprit_current_index);
            register_sprite(png_list->files[i].name, sprit_current_index);
            printf("[SPRITE] Registered sprite '%s' at index %d\n",
                   png_list->files[i].name, sprit_current_index);
            sprit_current_index++;
            free_image_data(img);
        }
    }

    // Cleanup
    if (unique_colors) free_color_set(unique_colors);
    free_color_set(palette);
    free_png_list(png_list);

    printf("[SPRITE] Sprite system initialized with %d sprites\n", sprit_current_index);
}
