#include "../main.hpp"
#include "../utils/search.hpp"

#include "list.hpp"

void List::draw(SharedData &sharedData, unsigned int button) {
    arrayLength = static_cast<int>(sharedData.plugins.size());     
    scrollPercent = 504.0 / (arrayLength*85);   
    scrollThumbHeight = 504*scrollPercent;

    scrollThumbY = scrollY*scrollPercent;
    
    if(button == NULL) {
        scrollDelay = 0; 
        scrollStage = 0;
    }
    
    if(scrollDelay >= 0) scrollDelay--;

    vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));

    if(sharedData.cursorY*85>scrollY+374) scrollY += 85;
    if(sharedData.cursorY*85<scrollY) scrollY -= 85;

    // special iterator member functions for objects
    for(int i=0;i<arrayLength;i++) {

        //if it isn't visible, skip render
        if((i*85)-scrollY>544) break;
        if((i*85)+80-scrollY<0) continue;

        vita2d_font_draw_textf(sharedData.font, 20, (i*85)+48-scrollY, RGBA8(255,255,255,255), 32, "%s", sharedData.plugins[i]["name"].get<string>().c_str());
        vita2d_font_draw_textf(sharedData.font, 20, (i*85)+80-scrollY, RGBA8(255,255,255,255), 32, "%s", sharedData.plugins[i]["description"].get<string>().c_str());
    }

    vita2d_draw_rectangle(10, (sharedData.cursorY*85)+14-scrollY, 940, 80, RGBA8(0,0,0,80));
    
    vita2d_draw_texture(desc, 0, 504);
    
    if(scrollDelay <= 1) {
        if(scrollDelay == 0) scrollStage = 0;
        switch(button) {
            case SCE_CTRL_CROSS:
                if(sharedData.blockCross) break;
                sharedData.initDetail = true;
                sharedData.blockCross = true;

                sharedData.scene = 1;
                break;
            case SCE_CTRL_TRIANGLE:
                searchResult = initImeDialog("Search", "Enter a query", 28);

                if(searchResult == "" || searchResult == "*") sharedData.plugins = sharedData.original;
                else if(searchResult != "❌") sharedData.plugins = sortJson(searchResult, sharedData.original);

                sharedData.cursorY = 0;
                break;
            case SCE_CTRL_DOWN:
                if(sharedData.cursorY >= arrayLength-1) break;

                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 8;
                }
                else {
                    scrollDelay = 3;
                }
                sharedData.cursorY++;
                break;
            case SCE_CTRL_UP:
                if(sharedData.cursorY <= 0) break;

                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 8;
                }
                else {
                    scrollDelay = 3;
                }
                sharedData.cursorY--;
                break;
        }
    }
}

void List::free() {
    vita2d_free_texture(desc);
}