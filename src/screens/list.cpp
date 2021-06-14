#include "../main.hpp"
#include "../utils/search.hpp"

#include "list.hpp"
#include "screens.hpp"

//defined in main.cpp
extern json plugins;
extern json originalPlugins;
extern Screens screens;
extern vita2d_font *font;

int listCursorY = 0;

List::List() {
    desc = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc.png");
}

void List::handleInput(Input input) {
    if(input.newButtonsPressed(SCE_CTRL_DOWN | SCE_CTRL_UP)) {
        scrollDelay = 0; 
        scrollStage = 0;
    }

    if(scrollDelay <= 1) {
        if(scrollDelay == 0) scrollStage = 0;

        if(input.newButtonsPressed(SCE_CTRL_CROSS)) {
            detailsClass = Details();

            screens.addScreen((Screen*) &detailsClass);
        }
        if(input.newButtonsPressed(SCE_CTRL_TRIANGLE)) {      
            sidebarClass = Sidebar();

            screens.addScreen((Screen*) &sidebarClass);
            // initImeDialog("Search for a plugin", "", 28);

            // listCursorY = 0;
        }
        if(input.buttonsPressed(SCE_CTRL_UP)) {
            if(listCursorY > 0) {
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
        if(input.buttonsPressed(SCE_CTRL_DOWN)) {
            if(listCursorY < arrayLength-1) {
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

    if(listCursorY*85>scrollY+374) scrollY += 85;
    if(listCursorY*85<scrollY) scrollY -= 85;

    vita2d_draw_rectangle(10, (listCursorY*85)+10-scrollY, 940, 80, RGBA8(0,0,0,80));    
    vita2d_draw_rectangle(10, (listCursorY*85)+10-scrollY, 5, 80, RGBA8(30,30,30,255));

    // special iterator member functions for objects
    for(int i=0;i<arrayLength;i++) {

        //if it isn't visible, skip render
        if((i*85)-scrollY>544) break;
        if((i*85)+80-scrollY<0) continue;

        vita2d_draw_text_with_shadow(font, 20, (i*85)+45-scrollY, RGBA8(255,255,255,255), 32, "%s", (plugins[i]["name"].get<string>() + " " + plugins[i]["version"].get<string>()).c_str());
        vita2d_draw_text_with_shadow(font, 20, (i*85)+76-scrollY, RGBA8(220,220,220,255), 32, "%s", plugins[i]["description"].get<string>().c_str());;
    }

    vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));
    vita2d_draw_texture(desc, 0, 504);
}

void List::free() {
    vita2d_free_texture(desc);
}
