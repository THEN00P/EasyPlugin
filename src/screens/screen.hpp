#pragma once

#include "../utils/input.hpp"
#include "vita2d.h"

int vita2d_draw_text_with_shadow(vita2d_font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...);

class Screen {
    public:
        int zIndex = 0;
        bool transparency = false;
        bool blockInput = true;
        
        virtual void handleInput(Input input);
        virtual void draw();
        virtual void free();
};