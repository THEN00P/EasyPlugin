#include "screen.hpp"
#include <stdio.h>
#include <stdarg.h>

int vita2d_draw_text_with_shadow(vita2d_font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...) {
    char buf[1024];
	va_list argptr;

	va_start(argptr, text);
	vsnprintf(buf, sizeof(buf), text, argptr);
	va_end(argptr);

    vita2d_font_draw_text(font, x+2, y+2, RGBA8(50,50,50,255), size, buf);
    return vita2d_font_draw_text(font, x, y, color, size, buf);
}

void Screen::free() {return;};
void Screen::handleInput(Input input) {return;};
void Screen::draw() {return;};