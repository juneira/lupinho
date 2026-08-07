#ifndef UI_H
#define UI_H

#include <stdint.h>
#include "types.h"

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 270
#define PALETTE_SIZE  256

//----------------------------------------------------------------------------------
// Draw Functions
//----------------------------------------------------------------------------------
void draw_line(LineItem *line);
void draw_rect(RectItem *rect);
void draw_circle(CircleItem *circle);
void draw_clear(ClearItem *clear);
void draw_triangle(TriangleItem *triangle);

//----------------------------------------------------------------------------------
// Palette Functions
//----------------------------------------------------------------------------------
void palset(int position, int bgr555);
Color get_palette_color(int index);

//----------------------------------------------------------------------------------
// Fill Pattern Functions
//----------------------------------------------------------------------------------
extern uint8_t fill_pattern[8];

//----------------------------------------------------------------------------------
// Tile / Sprite / Map Draw Functions
//----------------------------------------------------------------------------------
void draw_tile(const char *path, int width, int height, int tile_index, int x, int y, bool flipped);
void draw_spr(const char *path, int width, int height, int x, int y, bool flipped);
void set_fillp(uint8_t *bytes, int n);
void draw_map_layer(MapLayerData *data, int map_width, int tile_size, int cam_x, int cam_y);

//----------------------------------------------------------------------------------
// Camera Functions
//----------------------------------------------------------------------------------
extern int camera_x;
extern int camera_y;
void set_camera(int x, int y);
void reset_camera(void);

//----------------------------------------------------------------------------------
// Clip Functions
//----------------------------------------------------------------------------------
void set_clip(int x, int y, int w, int h);
void reset_clip(void);

//----------------------------------------------------------------------------------
// Frame Buffer Functions
//----------------------------------------------------------------------------------
extern uint8_t frame_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
void fb_set(int x, int y, int color, bool check_pattern);
void clear_frame_buffer();
void draw_frame_buffer();

//----------------------------------------------------------------------------------
// Print (bitmap font) Functions
//----------------------------------------------------------------------------------
void draw_print(const char *text, int x, int y, int color_index);

//----------------------------------------------------------------------------------
// UI Reset
//----------------------------------------------------------------------------------
void reset_ui_state(void);

//----------------------------------------------------------------------------------
// Keyboard
//----------------------------------------------------------------------------------
extern int keyboard_pad;
void set_keyboard_pad(int pad);

#endif // UI_H
