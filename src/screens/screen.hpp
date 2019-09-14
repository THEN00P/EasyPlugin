#pragma once

#include "../utils/input.hpp"

class Screen {
    public:
        int zIndex = 0;
        bool transparency = true;
        bool blockInput = true;
        
        virtual ~Screen();

        virtual void handleInput(Input input);
        virtual void draw();
};