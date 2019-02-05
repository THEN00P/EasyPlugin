#pragma once

#include <vita2d.h>
#include "../main.hpp"

class Details {
    public:
        void draw(SharedData &sharedData, unsigned int button);
        void free();

    private:
        string longDescription;
        int lastNum = 0;
        vita2d_texture *desc1 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc1.png");
};