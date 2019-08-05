#pragma once

#include <vita2d.h>
#include <psp2/kernel/clib.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/clib.h>
#include <psp2/sysmodule.h>

#include "utils/json.hpp"

#define imageWebBase "http://rinnegatamante.it/vitadb/"

using namespace std;

using json = nlohmann::json;

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

class AppInfo {
    public:
        AppInfo(string p_appID, string p_title, string fileLocation);
        string appID;
        string title;
        vita2d_texture *icon;
};

struct SharedData {
    int scene = 0;
    int cursorY = 0;
    bool blockCross = false;
    bool blockCircle = false;
    bool blockStart = false;
    bool initDetail = true;
    string taiConfig = "";
    string taiConfigPath = "";
    vector<vita2d_texture *> screenshots;
    json plugins;
    json original;
    vector<AppInfo> appData;
    vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);
};