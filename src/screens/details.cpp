#include <sstream>
#include <iostream>

#include "../main.hpp"
#include "../utils/format.hpp"
#include "../net/download.hpp"
#include "../utils/filesystem.hpp"
#include "screens.hpp"

#include "details.hpp"

#define maxImageHeight 305

//defined in list.cpp
extern int listCursorY;

//defined in main.cpp
extern Screens screens;
extern json plugins;
extern vita2d_font *font;

vector<std::string> split(std::string strToSplit, char delimeter) {
    stringstream ss(strToSplit);
    string item;
    vector<std::string> splittedStrings;
    while (getline(ss, item, delimeter))
    {
    splittedStrings.push_back(item);
    }
    return splittedStrings;
}

Details::Details() {
    //screen settings
    zIndex = 1;

    desc1 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc1.png");
    screenshots.clear();
    
    if(plugins[listCursorY]["screenshots"].get<string>() != "") {
        vector<string> subPaths;
        if(!screenshots.empty()) {
            for(int i=0;i<screenshots.size();i++) {
                if(screenshots[i])
                vita2d_free_texture(screenshots[i]);
            }

            screenshots.clear();
        }

        subPaths = split(plugins[listCursorY]["screenshots"].get<string>().c_str(), ';');

        for(string subPath : subPaths) {
            Filesystem::mkDir("ux0:data/Easy_Plugins/screenshots");
            curlDownload((imageWebBase+subPath).c_str(), ("ux0:data/Easy_Plugins/"+subPath).c_str());

            vita2d_texture *img;
            string img_file = ("ux0:data/Easy_Plugins/"+subPath);

            img = vita2d_load_PNG_file(img_file.c_str());

            if (!img) {
                img = vita2d_load_JPEG_file(img_file.c_str());
                if (!img) {
                    img = vita2d_load_BMP_file(img_file.c_str());
                }
            }

            if (img) {
                screenshots.push_back(img);
            }
        }
    }

    cycleCounter = 0;
    imageCycles = 0;
    
    longDescription = formatLongDesc(plugins[listCursorY]["long_description"].get<string>(), font, 920, 32);
}

void Details::handleInput(Input input) {
        if(input.newButtonsPressed(SCE_CTRL_CIRCLE)) {
            screens.removeScreen(this);
        }
        if(input.newButtonsPressed(SCE_CTRL_CROSS)) {
            popupClass = Popup();

            screens.addScreen(popup);
        }
}

void Details::draw() {
    cycleCounter++;
    if(cycleCounter == 300) {
        cycleCounter = 0;
        if(imageCycles < screenshots.size()-1) imageCycles++;
        else imageCycles = 0;
    }

    if(!screenshots.empty()) {        
        if (screenshots[imageCycles]) {
            unsigned int height = vita2d_texture_get_height(screenshots[imageCycles]);
            
            float size = (float) maxImageHeight / height;

            vita2d_draw_texture_scale(screenshots[imageCycles], 410, 10, size, size);
        }
    }

    vita2d_draw_text_with_shadow(font, 20, 45, RGBA8(255,255,255,255), 35, "%s", (plugins[listCursorY]["name"].get<string>() + " " + plugins[listCursorY]["version"].get<string>()).c_str());
    vita2d_draw_text_with_shadow(font, 390 - vita2d_font_text_width(font, 20, plugins[listCursorY]["date"].get<string>().c_str()), 80, RGBA8(220,220,220,255), 20, "%s", plugins[listCursorY]["date"].get<string>().c_str());
    vita2d_draw_text_with_shadow(font, 20, 80, RGBA8(220,220,220,255), 28, "by %s", plugins[listCursorY]["author"].get<string>().c_str());
    vita2d_draw_text_with_shadow(font, 20, 310, RGBA8(220,220,220,255), 28, "Global downloads count: %s", plugins[listCursorY]["downloads"].get<string>().c_str());
    
    vita2d_draw_text_with_shadow(font, 20, 350, RGBA8(255,255,255,255), 32, "%s", longDescription.c_str());

    vita2d_draw_texture(desc1, 0, 504);
}

void Details::free() {
    vita2d_free_texture(desc1);
    
    for(int i=0;i<screenshots.size();i++) {
        if(screenshots[i] != NULL)
        vita2d_free_texture(screenshots[i]);
    }
}