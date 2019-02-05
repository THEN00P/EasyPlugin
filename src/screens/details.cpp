#include "../main.hpp"
#include "../utils/format.hpp"

#include "details.hpp"

void Details::draw(SharedData &sharedData, unsigned int button) {
    if(sharedData.initDetail) {
        longDescription = formatLongDesc(sharedData.plugins[sharedData.cursorY]["long_description"].get<string>());

        sharedData.initDetail = false;
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