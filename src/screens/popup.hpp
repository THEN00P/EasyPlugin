#pragma once

#include <psp2/io/dirent.h>
#include <vector>

#include "../main.hpp"

class Popup {
    public:
        void draw(SharedData &sharedData, unsigned int button);
        void free();

    private:
        SceIoDirent dirStruct;
        bool tai = true;
        int currentPlugin = 0;
        int dir;
        int state = 0;
        string plPath;
        string plName = "AnalogsEnhancer.zip";
        vector<string> installFiles;
};