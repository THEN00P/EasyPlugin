#pragma once

#include <vita2d.h>
#include <psp2/ctrl.h>
#include <fstream>

#include "utils/json.hpp"

using namespace std;

using json = nlohmann::json;

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

struct SharedData {
    int scene = 0;
    int cursorY = 0;
    bool blockCross = false;
    bool initDetail = true;
    string taiConfig = "";
    string taiConfigPath = "";
    json plugins;
    json original;
    vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);
};