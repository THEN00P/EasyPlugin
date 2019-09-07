#pragma once

#include <vita2d.h>
#include "screen.hpp"
#include "../main.hpp"

class Details : public Screen {
    public:
        Details();
        ~Details();
        void draw() override;
        void handleInput(Input input) override;
        int zIndex = 1;

    private:
        string longDescription;
        vector<vita2d_texture *> screenshots;
        int lastNum = 0;
        int imageCycles = 0;
        int cycleCounter = 0;
        vita2d_texture *desc1;
};