#include "../main.hpp"
#include "../utils/search.hpp"

#include "screens.hpp"
#include "details.hpp"
#include "list.hpp"

//defined in main.cpp
extern json plugins;
extern json originalPlugins;
extern Screens screens;
extern vita2d_font *font;

int listCursorY = 0;

void List::handleInput(Input input) {
    if(input.newButtonsPressed(SCE_CTRL_DOWN | SCE_CTRL_UP)) {
        scrollDelay = 0; 
        scrollStage = 0;
    }

    if(scrollDelay <= 1) {
        if(scrollDelay == 0) scrollStage = 0;

        if(input.newButtonsPressed(SCE_CTRL_CROSS)) {
            Details *details;

            screens.addScreen(details);
        }
        if(input.newButtonsPressed(SCE_CTRL_TRIANGLE)) {
            initImeDialog("Search for a plugin", "", 28);

            listCursorY = 0;
        }
        if(input.newButtonsPressed(SCE_CTRL_UP)) {
            if(listCursorY >= 0) {
                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 8;
                }
                else {
                    scrollDelay = 3;
                }
                listCursorY--;
            }
        }
        if(input.newButtonsPressed(SCE_CTRL_DOWN)) {
            if(listCursorY <= arrayLength-1) {
                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 8;
                }
                else {
                    scrollDelay = 3;
                }
                listCursorY++;
            }
        }
    }
}

void List::draw() {
    sceClibPrintf("List \n");

    if(updateImeDialog() == IME_DIALOG_RESULT_FINISHED) {
        searchResult = string((char *)getImeDialogInputTextUTF8());

        if(searchResult == "" || searchResult == "*") plugins = originalPlugins;
        else plugins = sortJson(searchResult, originalPlugins);
    }

    arrayLength = static_cast<int>(plugins.size());
    scrollPercent = 504.0 / (arrayLength*85);   
    scrollThumbHeight = 504*scrollPercent;

    scrollThumbY = scrollY*scrollPercent;
    
    if(scrollDelay >= 0) scrollDelay--;

    vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));

    if(listCursorY*85>scrollY+374) scrollY += 85;
    if(listCursorY*85<scrollY) scrollY -= 85;

    // special iterator member functions for objects
    for(int i=0;i<arrayLength;i++) {

        //if it isn't visible, skip render
        if((i*85)-scrollY>544) break;
        if((i*85)+80-scrollY<0) continue;

        vita2d_font_draw_textf(font, 20, (i*85)+48-scrollY, RGBA8(255,255,255,255), 32, "%s", plugins[i]["name"].get<string>().c_str());
        vita2d_font_draw_textf(font, 20, (i*85)+80-scrollY, RGBA8(255,255,255,255), 32, "%s", plugins[i]["description"].get<string>().c_str());
    }

    vita2d_draw_rectangle(10, (listCursorY*85)+14-scrollY, 940, 80, RGBA8(0,0,0,80));
    
    vita2d_draw_texture(desc, 0, 504);
}

List::~List() {
    vita2d_free_texture(desc);
}
