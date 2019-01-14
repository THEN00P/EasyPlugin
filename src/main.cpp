#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h> 
#include <psp2/io/fcntl.h>
#include <fstream>
#include <string>

using namespace std;

#include "utils/json.hpp"
#include "net/download.hpp"
#include "utils/search.hpp"
#include "utils/format.hpp"

extern "C" {
    #include "utils/lazy/archive.h"
}

using json = nlohmann::json;

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

SceCtrlData pad;

int main() {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(255,255,255,255));

    vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);
    vita2d_texture *bgIMG = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/bg.png");
    vita2d_texture *desc = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc.png");
    vita2d_texture *desc1 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc1.png");

    // netInit();
    // httpInit();
    // curlDownload("http://rinnegatamante.it/vitadb/list_plugins_json.php", "ux0:json.json");
    // httpTerm();
    // netTerm();
    
    int scene = 0;
    bool initDetail = true;
    bool blockCross = false;


    string searchResult = "";
    string longDescription;
    string plName = "AnalogsEnhancer.skprx";
    string taiConfig = "";
    string taiConfigPath = "";

    int cursorY = 0;
    int scrollDelay = 0;
    int scrollStage = 0;
    int arrayLength;
    int lastNum = 0;

    ifstream plp("ux0:data/Easy_Plugins/plugins.json");
    json plugins;
    plp >> plugins;
    plp.close();

    if(doesDirExist("ux0:tai")) taiConfigPath = "ux0:tai/";
    else if(doesDirExist("ur0:tai")) taiConfigPath = "ur0:tai/";
    else return 0;

    ifstream blb(taiConfigPath+"config.txt");
    blb >> taiConfig;
    blb.close();

    json original = plugins;


    double scrollY = 0;
    double scrollPercent;
    double scrollThumbHeight;
    double scrollThumbY = 0;

    while(1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        vita2d_start_drawing();
        vita2d_clear_screen();

        vita2d_draw_texture(bgIMG, 0, 0);

        if(pad.buttons != SCE_CTRL_CROSS) blockCross = false;

        if(scene == 0) {
            arrayLength = static_cast<int>(plugins.size());     
            scrollPercent = 504.0 / (arrayLength*85);   
            scrollThumbHeight = 504*scrollPercent;

            scrollThumbY = scrollY*scrollPercent;
            if(scrollDelay >= 0) scrollDelay--;

            vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));

            if(cursorY*85>scrollY+374) scrollY += 85;
            if(cursorY*85<scrollY) scrollY -= 85;

            // special iterator member functions for objects
            for(int i=0;i<arrayLength;i++) {

                //if it isn't visible, skip render
                if((i*85)-scrollY>544) break;
                if((i*85)+80-scrollY<0) continue;

                vita2d_font_draw_textf(font, 20, (i*85)+48-scrollY, RGBA8(255,255,255,255), 32, "%s", plugins[i]["name"].get<string>().c_str());
                vita2d_font_draw_textf(font, 20, (i*85)+80-scrollY, RGBA8(255,255,255,255), 32, "%s", plugins[i]["description"].get<string>().c_str());
            }

            vita2d_draw_rectangle(10, (cursorY*85)+14-scrollY, 940, 80, RGBA8(0,0,0,80));
            
            vita2d_draw_texture(desc, 0, 504);
            
            if(scrollDelay <= 1) {
                if(scrollDelay == 0) scrollStage = 0;
                switch(pad.buttons) {
                    case SCE_CTRL_CROSS:
                        if(blockCross) break;
                        initDetail = true;
                        blockCross = true;

                        scene = 1;
                        break;
                    case SCE_CTRL_TRIANGLE:
                        searchResult = initImeDialog("Search", "Enter a query", 28);

                        if(searchResult == "" || searchResult == "*") plugins = original;
                        else if(searchResult != "❌") plugins = sortJson(searchResult, original);

                        cursorY = 0;
                        break;
                    case SCE_CTRL_DOWN:
                        if(cursorY >= arrayLength-1) break;

                        if(scrollStage <= 10) {
                            if(scrollDelay == 1) scrollStage++;
                            scrollDelay = 8;
                        }
                        else {
                            scrollDelay = 3;
                        }
                        cursorY++;
                        break;
                    case SCE_CTRL_UP:
                        if(cursorY <= 0) break;

                        if(scrollStage <= 10) {
                            if(scrollDelay == 1) scrollStage++;
                            scrollDelay = 8;
                        }
                        else {
                            scrollDelay = 3;
                        }
                        cursorY--;
                        break;
                }
            }
        }
        if(scene == 1 || scene == 2) {
            if(initDetail) {
                longDescription = formatLongDesc(plugins[cursorY]["long_description"].get<string>());

                initDetail = false;
            }

            vita2d_font_draw_textf(font, 20, 45, RGBA8(255,255,255,255), 35, "%s", (plugins[cursorY]["name"].get<string>() + " " + plugins[cursorY]["version"].get<string>()).c_str());
            
            vita2d_font_draw_textf(font, 20, 350, RGBA8(255,255,255,255), 32, "%s", longDescription.c_str());

            vita2d_draw_texture(desc1, 0, 504);

            if(scene == 2) {
                if(plName.find(".zip") != string::npos) {
                    extractArchivePath((taiConfigPath+plName).c_str(), (taiConfigPath+"extracted/").c_str(), NULL);
                }
                else if(plName.find(".suprx")) {

                }
                else if(plName.find(".skprx") != string::npos) {
                    taiConfig += ("*KERNEL\n"+taiConfigPath+plName);
                }
                else {
                    
                }

                ofstream tat(taiConfigPath+"config.txt");
                tat << taiConfig;
                tat.close();
            }

            if(scrollDelay <= 1) {
                if(scrollDelay == 0) scrollStage = 0;

                if(scene == 1) {
                    switch(pad.buttons) {
                        case SCE_CTRL_CIRCLE:
                            scene = 0;
                            break;
                        case SCE_CTRL_CROSS:
                            if(blockCross) break;

                            scene = 2;
                            
                            blockCross = true;
                            break;
                    }
                }
                if(scene == 2) {
                    switch(pad.buttons) {
                        case SCE_CTRL_CIRCLE:
                            scene = 1;
                            break;
                        case SCE_CTRL_CROSS:
                            if(blockCross) break;
                            
                            blockCross = true;
                            break;
                    }
                }
            }
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();

        if(pad.buttons == SCE_CTRL_SELECT) {
            break;
        }
        if(pad.buttons == SCE_CTRL_START) {
            scePowerRequestColdReset();
        }
    }

    vita2d_free_font(font);
    vita2d_free_texture(bgIMG);
    vita2d_free_texture(desc);
    vita2d_free_texture(desc1);
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}
