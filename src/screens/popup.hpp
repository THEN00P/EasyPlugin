#pragma once

#include <psp2/io/dirent.h>
#include <vector>

#include "../utils/filesystem.hpp"

#include "../main.hpp"

class Popup {
    public:
        void draw(SharedData &sharedData, unsigned int button);
        void free();

    private:
        void handleSuprx(SharedData &sharedData, int &currentPlugin, unsigned int button);
        int selected = 0;
        int scrollY = 0;
        int scrollDelay = 0;
        int scrollStage = 0;
        vector<int> selectedApps;
        SceIoDirent dirStruct;
        bool tai = true;
        bool archive = false;
        int currentPlugin = 0;
        int dir;
        int state = 0;
        string plPath;
        string plName = "";
        vector<string> installFiles;
        vita2d_texture *desc = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc2.png");
};