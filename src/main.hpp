#pragma once

#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/clib.h>

#include "utils/json.hpp"

using namespace std;

using json = nlohmann::json;

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

struct AppInfo {
    string appID;
    string title;
    vita2d_texture *icon;
};

struct SharedData {
    int scene = 0;
    int cursorY = 0;
    bool blockCross = false;
    bool initDetail = true;
    string taiConfig = "";
    string taiConfigPath = "";
    json plugins;
    json original;
    vector<AppInfo> appData;
    vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);
};