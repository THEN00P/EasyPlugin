#pragma once

#include <vita2d.h>
#include "../main.hpp"
#include "screen.hpp"

class List : public Screen {
    public:
        List();
        ~List();
        void draw() override;
        void handleInput(Input input) override;
        int zIndex = 0;
        bool transparency = false;

    private:
        double scrollY = 0;
        double scrollPercent;
        double scrollThumbHeight;
        double scrollThumbY = 0;
        int arrayLength;
        int scrollDelay = 0;
        int scrollStage = 0;
        string searchResult = "";
        vita2d_texture *desc;
};