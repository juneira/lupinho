# 🎮 Lupinho

**Lupinho** is a web-based emulator for **Lupi**, a Brazilian game console. It runs directly in your browser using WebAssembly, allowing you to write and play games using a simple Lua scripting API.

### 🕹️ [Try the Demo](https://lupinho.kaninde.app/)

## ✨ Features

- 🌐 **Browser-based** — Runs on WebAssembly, no installation required
- 📜 **Lua scripting** — Write your games in simple, easy-to-learn Lua
- 🎨 **2D Graphics** — Draw text, lines, rectangles, circles, and triangles
- ⚡ **60 FPS** — Smooth gameplay at 60 frames per second
- 🔧 **Raylib powered** — Built on the lightweight Raylib graphics library

## 🛠️ Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C99 |
| Compiler | Emscripten (emcc) |
| Graphics | Raylib |
| Scripting | Lua 5.4 |
| Platform | WebAssembly |

## 📁 Project Structure

```
lupinho/
├── dist/               # Compiled WebAssembly output
│   ├── index.html
│   ├── index.js
│   ├── index.wasm
│   └── index.data
├── src/
│   ├── webassembly.c   # Main entry point
│   ├── lua_api.c       # Lua bindings for drawing
│   ├── drawlist.c/h    # Rendering system
│   ├── types.h         # Type definitions
│   ├── script.lua      # Your game code goes here!
│   ├── Makefile
│   └── libs/
│       ├── lua-web/    # Lua compiled for WebAssembly
│       └── raylib-web/ # Raylib compiled for WebAssembly
└── README.md
```

## 🚀 Getting Started

### Prerequisites

- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) installed and activated

### Building

```bash
cd src

# Development build (with FPS counter)
make web

# Production build (optimized, no debug)
make production
```

### Running

After building, serve the `dist/` folder with any HTTP server:

```bash
# Using Node.js
npx serve ../dist
```

Then open `http://localhost:8080` in your browser.

## 📖 Lua API

Games are written in `src/script.lua`. Your script should define the `update()` function, which is called every frame (60 times per second).

### `update()`
Called every frame. Update your game logic and draw here.

### Color Palette System

Lupinho uses a palette-based color system with 256 colors in BGR555 format (5 bits per channel, Big Endian).

| Function | Description |
|----------|-------------|
| `ui.palset(index, bgr555)` | Set a palette color at the specified index (0-255) |

### Drawing Functions

All drawing functions use palette indices (0-255) for colors:

| Function | Description |
|----------|-------------|
| `ui.draw_text(text, x, y)` | Draw text at position (x, y) |
| `ui.draw_line(x1, y1, x2, y2, color_index)` | Draw a line between two points |
| `ui.draw_rect(x, y, width, height, filled, color_index)` | Draw a rectangle (filled or outline) |
| `ui.draw_circle(cx, cy, radius, filled, color_index, border, border_color_index)` | Draw a circle with optional border |
| `ui.draw_triangle(p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, color_index)` | Draw a filled triangle with 3 vertices |

### Example Game

```lua
-- Define your palette
Palette = {
    0x0000, 0x1516, 0x25B4, 0x20A6, 0x1DFD, 0x46FE, 0x7FFF, 0x2532
}

-- Set palette colors
for i = 1, #Palette do
    ui.palset(i - 1, Palette[i])
end

-- Game variables
x = 200
t = 0

function update()
    t = t + 0.05
    y = math.sin(t) * 25

    ui.draw_text("Bem-vindo ao Lupi!", 280, 180 + math.floor(y))
    ui.draw_rect(50, 50, 80, 80, true, 1)
    ui.draw_circle(200, 300, 20, true, 3, true, 5)
    ui.draw_triangle(20, 250, 100, 250, 55, 350, 4)
end
```

## 🧹 Cleanup

```bash
make clean
```

## 📝 License

This project is open source.

---

<p align="center">
  Made with ❤️ in Brazil 🇧🇷
</p>
