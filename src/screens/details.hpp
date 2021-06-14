#pragma once

#include <vita2d.h>
#include "screen.hpp"
#include "popup.hpp"
#include "../main.hpp"

class Details : public Screen {
    public:
        Details();
        void free() override;
        void draw() override;
        void handleInput(Input input) override;

    private:
        Popup popupClass;
        string longDescription;
        vector<vita2d_texture *> screenshots;
        int lastNum = 0;
        int imageCycles = 0;
        int cycleCounter = 0;
        vita2d_texture *desc1;
};