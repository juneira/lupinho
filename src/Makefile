# Emscripten Compiler
CC = emcc

# Source Files
SRC = webassembly.c drawlist.c lua_api.c

# Output File
OUTPUT = ../dist/index.html

# Raylib Directories
RAYLIB_SRC = libs/raylib-web
RAYLIB_INCLUDE = -Ilibs/raylib-web
RAYLIB_LIB = libs/raylib-web/libraylib.web.a

# Lua Configuration
LUA_WEB_INCLUDE = -Ilibs/lua-web
LUA_WEB_LIB = libs/lua-web/liblua.web.a

# Compilation Flags
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

# Emscripten Web Flags
EMFLAGS = -s USE_GLFW=3
# Note: ASYNCIFY removed - not needed and causes ~50% performance overhead
EMFLAGS += -s TOTAL_MEMORY=67108864
EMFLAGS += -s FORCE_FILESYSTEM=1
EMFLAGS += --shell-file libs/raylib-web/minshell.html
EMFLAGS += -DPLATFORM_WEB

# Preload Lua script into the virtual filesystem
EMFLAGS += --preload-file script.lua

# Optimization
OPTIMIZATION = -O2

# Debug/Production Flags
# Production flags without closure compiler (causes issues with Raylib)
DEBUG_FLAGS = -DDEBUG_MODE
PROD_FLAGS = -O3 -s ASSERTIONS=0 -s DISABLE_EXCEPTION_CATCHING=1 -s ELIMINATE_DUPLICATE_FUNCTIONS=1 -DPRODUCTION

# Web Target (development with debug)
web: $(SRC)
	$(CC) $(SRC) -o $(OUTPUT) $(RAYLIB_INCLUDE) $(LUA_WEB_INCLUDE) $(CFLAGS) $(EMFLAGS) $(OPTIMIZATION) $(DEBUG_FLAGS) $(LUA_WEB_LIB) $(RAYLIB_LIB)

# Production Target (optimized, no debug)
production: $(SRC)
	$(CC) $(SRC) -o $(OUTPUT) $(RAYLIB_INCLUDE) $(LUA_WEB_INCLUDE) $(CFLAGS) $(EMFLAGS) $(PROD_FLAGS) $(LUA_WEB_LIB) $(RAYLIB_LIB)

# Clean generated files
clean:
	rm -f $(OUTPUT) index.js index.wasm lupi_emulator

# Help
help:
	@echo "Lupi Emulator Makefile"
	@echo "======================"
	@echo ""
	@echo "Targets:"
	@echo "  make web         - Build for WebAssembly (development mode with debug)"
	@echo "  make production  - Build for WebAssembly (optimized, no debug)"
	@echo "  make clean       - Remove generated files"
	@echo ""
	@echo "Note: For WebAssembly builds, Lua must be compiled with Emscripten."
	@echo "      If you get linking errors, you may need to compile Lua with emcc."

.PHONY: web production native clean help
