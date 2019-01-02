#include <vita2d.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/io/fcntl.h>
#include <fstream>
#include <string>

using namespace std;

#include "utils/json.hpp"
#include "net/download.hpp"
#include "utils/search.hpp"

using json = nlohmann::json;

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

SceCtrlData pad;

int main() {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(255,255,255,255));

    vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);
    vita2d_texture *bgIMG = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/bg.png");

    // netInit();
    // httpInit();
    // curlDownload("http://rinnegatamante.it/vitadb/list_plugins_json.php", "ux0:json.json");
    // httpTerm();
    // netTerm();
    
    int scene = 0;

    int cursorY = 0;
    string searchResult = "";
    double scrollY = 0;
    int scrollDelay = 0;
    int scrollStage = 0;

    ifstream i("ux0:data/Easy_Plugins/plugins.json");
    json plugins;
    i >> plugins;

    json original = plugins;

    int arrayLength;

    double scrollPercent;
    double scrollThumbHeight;
    double scrollThumbY = 0;

    while(1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        vita2d_start_drawing();
        vita2d_clear_screen();

        vita2d_draw_texture(bgIMG, 0, 0);

        if(scene == 0) {
            arrayLength = static_cast<int>(plugins.size());     
            scrollPercent = 544.0 / (arrayLength*85);   
            scrollThumbHeight = 544*scrollPercent;

            scrollThumbY = scrollY*scrollPercent;
            if(scrollDelay >= 0) scrollDelay--;

            vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));

            if(cursorY*85>scrollY+459) scrollY += 85;
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

            vita2d_font_draw_textf(font, 18, 42, RGBA8(0,0,0,255), 36, "%s", searchResult.c_str());
            vita2d_font_draw_textf(font, 20, 40, RGBA8(255,255,255,255), 32, "%s", searchResult.c_str());
            
            if(scrollDelay <= 1) {
                if(scrollDelay == 0) scrollStage = 0;
                switch(pad.buttons) {
                    case SCE_CTRL_TRIANGLE:
                        searchResult = initImeDialog("Search", "Enter a query", 28);

                        if(searchResult == "" || searchResult == "*") plugins = original;

                        plugins = sortJson(searchResult, original);
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

        vita2d_end_drawing();
        vita2d_swap_buffers();

        if(pad.buttons == SCE_CTRL_SELECT) {
            break;
        }
    }

    vita2d_free_font(font);
    vita2d_free_texture(bgIMG);
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}
