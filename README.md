# 🎮 Lupinho

**Lupinho** is a web-based emulator for **Lupi**, a Brazilian game console. It runs directly in your browser using WebAssembly, allowing you to write and play games using a simple Lua scripting API.

## ✨ Features

- 🌐 **Browser-based** — Runs on WebAssembly, no installation required
- 📜 **Lua scripting** — Write your games in simple, easy-to-learn Lua
- 🎨 **2D Graphics** — Draw text, lines, rectangles, and circles
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

Games are written in `src/script.lua`. Your script should define two functions:

### `init()`
Called once when the game starts. Initialize your game state here.

### `update()`
Called every frame (60 times per second). Update your game logic and draw here.

### Drawing Functions

All drawing functions are available under the `ui` table:

| Function | Description |
|----------|-------------|
| `ui.draw_text(text, x, y)` | Draw text at position (x, y) |
| `ui.draw_line(x1, y1, x2, y2, color)` | Draw a line between two points |
| `ui.draw_rect(x, y, width, height, filled, color)` | Draw a rectangle |
| `ui.draw_circle(cx, cy, radius, filled, color, border, border_color)` | Draw a circle |

### Example Game

```lua
function init()
    x = 200
    t = 0
end

function update()
    t = t + 0.05
    y = math.sin(t) * 25

    ui.draw_text("Bem-vindo ao Lupi!", 280, 180 + math.floor(y))
    ui.draw_rect(50, 50, 80, 80, true, 1)
    ui.draw_circle(200, 300, 20, true, 1, true, 0)
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
