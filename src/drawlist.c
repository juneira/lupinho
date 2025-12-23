#include "types.h"
#include "drawlist.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdlib.h>

/*
Global vars
*/
extern Drawlist drawlist;

void add_text(char *text_s, int x, int y);
void draw_text(TextItem *text);

/*
Drawable Functions
*/
void draw(NodeDrawable *node) {
    if(node->type == 't') {
        draw_text((TextItem *) node->drawable);
    }
}

void add_drawable(void *drawable, char type) {
    if(drawlist.count == 0) {
        drawlist.count++;

        NodeDrawable *node = (NodeDrawable *) malloc(sizeof(NodeDrawable));
        drawlist.root = node;
        node->drawable = drawable;
        node->type = type;
        node->next = NULL;
        return;
    }

    drawlist.count++;

    NodeDrawable *current = drawlist.root;
    for(; current->next != NULL; current = current->next) {}

    NodeDrawable *node = (NodeDrawable *) malloc(sizeof(NodeDrawable));
    current->next = node;
    node->next = NULL;
    node->drawable = drawable;
    node->type = type;
}

/**
Text Functions
**/
void add_text(char *text_s, int x, int y) {
    TextItem *text = (TextItem *) malloc(sizeof(TextItem));
    text->text = text_s;
    text->x = x;
    text->y = y;
    text->fontSize = 20;
    text->color = DARKGRAY;

    add_drawable(text, 't');
}

void draw_text(TextItem *text) {
    DrawText(text->text, text->x, text->y, text->fontSize, text->color);
}

//----------------------------------------------------------------------------------
// ui.draw_text(texto:string, x:int, y:int)
//----------------------------------------------------------------------------------
int lua_escreva(lua_State *L) {
    char *text = luaL_checkstring(L, 1);
    int x = luaL_optinteger(L, 2, 10);
    int y = luaL_optinteger(L, 3, 10);

    add_text(text, x, y);

    return 0;
}
