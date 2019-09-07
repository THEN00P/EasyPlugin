#pragma once

#include <psp2/io/dirent.h>
#include <vector>

#include "../utils/filesystem.hpp"

#include "../main.hpp"
#include "screen.hpp"

class Popup : public Screen {
    public:
        ~Popup();
        void draw() override;
        void handleInput(Input input) override;

    private:
        void handleSuprx();
        void handleSkprx();
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
        int scrollThumbHeight = 0;
        double scrollPercent = 0;
        string plPath;
        string plName = "";
        string pluginEntry;
        size_t pluginEntryPos;
        vector<string> installFiles;
        vita2d_texture *desc = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc2.png");
        vita2d_texture *desc2 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc3.png");
        vita2d_texture *desc3 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc4.png");
};