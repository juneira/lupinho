# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What This Is

**Lupinho** is a WebAssembly emulator for **Lupi**, a Brazilian game console. It runs Lua games in the browser via WebAssembly. The emulator is written in C99, compiled with Emscripten, and uses Raylib for graphics and Lua for scripting.

- Screen resolution: 480×270 px at 60 FPS
- Color system: 256-color palette in BGR555 format
- Live demo: https://lupinho.kaninde.app/

## Build Commands

All build commands are run from the `src/` directory. Emscripten SDK must be installed and activated.

```bash
cd src

# Development build (includes FPS counter via -DDEBUG_MODE)
make web

# Production build (optimized, no debug overlay)
make production

# Clean build artifacts
make clean
```

Output is written to `dist/game.{html,js,wasm,data}`.

To serve the output locally:
```bash
npx serve dist/
```

## Architecture

### C Engine (`src/`)

| File | Role |
|------|------|
| `main.c` | Entry point. Initializes Lua API, opens Raylib window, runs `UpdateDrawFrame` via `emscripten_set_main_loop` on Web. |
| `lua_api.c` | All Lua C bindings. Exposes the `ui.*` table. Parses `lupi_manifest.txt` to build the `Sprites` global. |
| `ui.c` / `ui.h` | Rendering backend. Owns the frame buffer, palette, camera/clip state, bitmap font, tile/sprite drawing. |
| `types.h` | Shared draw item structs (`LineItem`, `RectItem`, `CircleItem`, `TriangleItem`, `MapLayerData`). |
| `font.h` | Embedded bitmap font data. |

**Rendering pipeline:** Every frame, Lua calls drawing functions → C writes palette indices into `frame_buffer[270][480]` → `draw_frame_buffer()` maps indices to Raylib `Color` and blits to screen → `clear_frame_buffer()` resets for next frame.

Pre-built static libraries live in `src/libs/`:
- `lua-web/liblua.web.a` — Lua compiled with Emscripten
- `raylib-web/libraylib.web.a` — Raylib compiled with Emscripten

### Game Structure

Each game is a directory (e.g., `game-example/`) with:
- `lupi_manifest.txt` — declares all assets (bitmaps and Lua files) with IDs, sizes, paths, and JSON metadata.
- `game.lua` — entry point, loaded by `lua_api_setup_game()`. Must define a global `update()` function called every frame.
- `palette.lua` — color palette definition.
- `img/` — raw bitmap sprites (not PNG; custom format read by the engine).
- `map/` — tilemap data.
- `scene/` — scene-specific Lua modules.

The manifest bitmap format per line: `<id> <size> <path> <json>` where json includes `"type":"bitmap"`, `"width"`, `"height"`, and optionally `"tiles"` for sprite sheets.

`inject_sprites_global()` in `lua_api.c` parses the manifest and populates a `Sprites` Lua global, namespaced by the first path segment (e.g., `Sprites.img.ninja_a = { path, width, height, ntiles }`).

### Lua API (`ui.*`)

All drawing APIs are exposed under the `ui` global table:

**Shapes:** `ui.rect`, `ui.rectfill`, `ui.draw_rect`, `ui.circ`, `ui.circfill`, `ui.draw_circle`, `ui.trisfill`, `ui.draw_line`

**Sprites/Tiles:** `ui.spr(spritesheet_table, x, y, [flipped])`, `ui.tile(spritesheet_table, tile_index, x, y)` — tile_index bit 10 (1024) triggers horizontal flip.

**Text:** `ui.print(text, x, y, color)`, `ui.draw_text(text, x, y)`

**Screen:** `ui.cls(color)`, `ui.camera([x, y])`, `ui.clip([x, y, w, h])`, `ui.fillp([bytes...])`

**Map:** `ui.draw_map(map_data, map_width, map_height, tile_size, cam_x, cam_y)`

**Palette:** `ui.palset(index, bgr555)`, `ui.set_pallet(start, count, table)`

**Input:** `ui.btn(button, pad)` (held), `ui.btnp(button, pad)` (just pressed). Button constants like `BTN_Z`, `UP`, `DOWN`, `LEFT`, `RIGHT` are injected as Lua globals.

### Site (`site/`)

`site/index.html` is the web player UI that embeds the compiled game. `site/dist/` contains a distributable zip. The site is separate from the game build output in `dist/`.

## Key Constants

- `SCREEN_WIDTH = 480`, `SCREEN_HEIGHT = 270` (defined in `ui.h`)
- `PALETTE_SIZE = 256`
- Total WebAssembly memory: 64 MB (`TOTAL_MEMORY=67108864`)
- Tile flip flag: bit 10 (`1024`) in tile index
