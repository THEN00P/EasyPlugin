#include "../main.hpp"
#include "../utils/format.hpp"

#include "details.hpp"

#define maxImageHeight 305

void Details::draw(SharedData &sharedData, unsigned int button) {
    if(sharedData.initDetail) {
        cycleCounter = 0;
        imageCycles = 0;
        
        longDescription = formatLongDesc(sharedData.plugins[sharedData.cursorY]["long_description"].get<string>());

        sharedData.initDetail = false;
    }

    cycleCounter++;
    if(cycleCounter == 300) {
        cycleCounter = 0;
        if(imageCycles < sharedData.screenshots.size()-1) imageCycles++;
        else imageCycles = 0;
    }

    if(!sharedData.screenshots.empty()) {        
        if (sharedData.screenshots[imageCycles]) {
            unsigned int height = vita2d_texture_get_height(sharedData.screenshots[imageCycles]);
            
            float size = (float) maxImageHeight / height;

            vita2d_draw_texture_scale(sharedData.screenshots[imageCycles], 410, 10, size, size);
        }
    }

    vita2d_font_draw_textf(sharedData.font, 20, 45, RGBA8(255,255,255,255), 35, "%s", (sharedData.plugins[sharedData.cursorY]["name"].get<string>() + " " + sharedData.plugins[sharedData.cursorY]["version"].get<string>()).c_str());
    
    vita2d_font_draw_textf(sharedData.font, 20, 350, RGBA8(255,255,255,255), 32, "%s", longDescription.c_str());

    vita2d_draw_texture(desc1, 0, 504);

    if(sharedData.scene == 1) {
        switch(button) {
            case SCE_CTRL_CIRCLE:
                sharedData.scene = 0;

                break;
            case SCE_CTRL_CROSS:
                if(sharedData.blockCross) break;

                sharedData.scene = 2;
                
                sharedData.blockCross = true;
                break;
        }
    }
}

void Details::free() {
    vita2d_free_texture(desc1);
}