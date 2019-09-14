#include <algorithm>

#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>

#include "../main.hpp"
#include "../utils/vhbb/zip.h"
#include "../net/download.hpp"
#include "popup.hpp"

#define columnHeight 168

extern vector<AppInfo> appData;
extern vita2d_font *font;
extern string taiConfigPath;
extern string taiConfig;
extern json plugins;
extern int listCursorY;

string toUppercase(string strToConvert) {
    transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

Popup::Popup() {
    desc = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc2.png");
    desc2 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc3.png");
    desc3 = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/desc4.png");

}

void Popup::handleInput(Input input) {
    if(installFiles[currentPlugin].find(".skprx") != string::npos) {
        if(pluginEntryPos == string::npos && input.newButtonsPressed(SCE_CTRL_START)) {
            Filesystem::copyFile(plPath+installFiles[currentPlugin], taiConfigPath+installFiles[currentPlugin]);
            taiConfig += pluginEntry;
            currentPlugin++;
        }
        else if(input.newButtonsPressed(SCE_CTRL_CROSS)) {
            taiConfig.erase(pluginEntryPos, pluginEntry.length());
            
            if(taiConfig.find(taiConfigPath+installFiles[currentPlugin]) != string::npos)
                sceIoRemove((taiConfigPath+installFiles[currentPlugin]).c_str());

            currentPlugin++;
        }

        if(input.newButtonsPressed(SCE_CTRL_CIRCLE)) {
            currentPlugin++;
        }
    }
    if(installFiles[currentPlugin].find(".suprx") != string::npos) {
        if(scrollDelay >= 0) scrollDelay--;
        if(!input.buttonsPressed(SCE_CTRL_UP | SCE_CTRL_DOWN)) {
            scrollDelay = 0; 
            scrollStage = 0;
        }
    
        if(scrollDelay <= 1) {
            if(scrollDelay == 0) scrollStage = 0;
                if(input.newButtonsPressed(SCE_CTRL_START)){
                    for(int index : selectedApps) {
                        pluginEntry = "\n\n*"+appData[index].appID+"\n"+taiConfigPath+installFiles[currentPlugin];
                        pluginEntryPos = taiConfig.find(pluginEntry);

                        if(pluginEntryPos == string::npos)
                            taiConfig += pluginEntry;
                        else 
                            taiConfig.erase(pluginEntryPos, pluginEntry.length());
                    }

                    if(taiConfig.find(taiConfigPath+installFiles[currentPlugin]) != string::npos) sceIoRemove((taiConfigPath+installFiles[currentPlugin]).c_str());
                    
                    selected = 0;
                    selectedApps.clear();
                    currentPlugin++;
                }
                if(input.newButtonsPressed(SCE_CTRL_CIRCLE)) {
                    selected = 0;
                    selectedApps.clear();
                    currentPlugin++;
                }
                if(input.newButtonsPressed(SCE_CTRL_CROSS)){
                    auto found = find(selectedApps.begin(), selectedApps.end(), selected);
                    if(found != selectedApps.end()) {
                        auto index = distance(selectedApps.begin(), found);
                        selectedApps.erase(selectedApps.begin()+index);
                    }
                    else {
                        selectedApps.push_back(selected);
                    }
                }
                if(input.buttonsPressed(SCE_CTRL_DOWN) && selected < appData.size()-1){
                    if(scrollStage <= 10) {
                        if(scrollDelay == 1) scrollStage++;
                        scrollDelay = 10;
                    }
                    else {
                        scrollDelay = 3;
                    }
                    selected++;
                }
                if(input.buttonsPressed(SCE_CTRL_UP) && selected > 0){
                    if(scrollStage <= 10) {
                        if(scrollDelay == 1) scrollStage++;
                        scrollDelay = 10;
                    }
                    else {
                        scrollDelay = 3;
                    }
                    selected--;
                }
        }
    }
}

void Popup::handleSkprx() {
    pluginEntry = "\n\n*Kernel\n"+taiConfigPath+installFiles[currentPlugin];
    pluginEntryPos = taiConfig.find(pluginEntry);

    int textWidth = vita2d_font_text_width(font, 40, (pluginEntryPos != string::npos ? installFiles[currentPlugin]+" is already installed" : "Install "+installFiles[currentPlugin]+"?").c_str());

    vita2d_font_draw_textf(font, 480 - (textWidth/2), 272, pluginEntryPos != string::npos ? RGBA8(255,100,100,255) : RGBA8(255,255,255,255), 40, pluginEntryPos != string::npos ? "%s is already installed." : "Install %s?", installFiles[currentPlugin].c_str());
    vita2d_draw_texture(pluginEntryPos != string::npos ? desc2 : desc3, 0, 504);
}

void Popup::handleSuprx() {
    if(selected*columnHeight > scrollY+374) scrollY += columnHeight;
    if(selected*columnHeight < scrollY) scrollY -= columnHeight;

    int scrollThumbY = scrollY*scrollPercent;

    vita2d_draw_rectangle(0, (selected*columnHeight)-scrollY, 960, columnHeight, RGBA8(0,0,0,60));

    for(int i=0;i<appData.size();i++) {
        vita2d_draw_rectangle(950, scrollThumbY, 10, scrollThumbHeight, RGBA8(0,0,0,150));

        if((i*columnHeight)-scrollY>544) break;
        if((i*columnHeight)+178-scrollY<0) continue;

        if(appData[i].icon != NULL)
        vita2d_draw_texture(appData[i].icon, 40, (i*columnHeight)+20-scrollY);
        else vita2d_draw_rectangle(40, (i*columnHeight)+20-scrollY, 128, 128, RGBA8(255,255,255,255));
        
        vita2d_font_draw_textf(font, 190, (i*columnHeight)+90-scrollY, RGBA8(255,255,255,255), 45, "%s", appData[i].title.c_str());

        if(taiConfig.find("\n\n*"+appData[i].appID+"\n"+taiConfigPath+installFiles[currentPlugin]) == string::npos)
            vita2d_draw_rectangle(870, (i*columnHeight)+55-scrollY, 42, 42, RGBA8(255,255,255,255));
        else
            vita2d_draw_rectangle(870, (i*columnHeight)+55-scrollY, 42, 42, RGBA8(255,125,125,255));

        if(find(selectedApps.begin(), selectedApps.end(), i) != selectedApps.end()) {
            if(taiConfig.find("\n\n*"+appData[i].appID+"\n"+taiConfigPath+installFiles[currentPlugin]) != string::npos)
                vita2d_draw_rectangle(875, (i*columnHeight)+60-scrollY, 32, 32, RGBA8(255,10,0,255));
            else
                vita2d_draw_rectangle(875, (i*columnHeight)+60-scrollY, 32, 32, RGBA8(38,166,242,255));
        }
    }

    vita2d_draw_texture(desc, 0, 504);
    vita2d_font_draw_textf(font, 20, 535, RGBA8(255,255,255,255), 30, "%s", installFiles[currentPlugin].c_str());
}

void Popup::draw() {
    if(state == 0) {
        plName = curlDownloadKeepName(plugins[listCursorY]["url"].get<string>().c_str(), taiConfigPath);
        installFiles.clear();
        archive = false;
        plPath = taiConfigPath;
        currentPlugin = 0;
        scrollPercent = 504.0 / (appData.size()*columnHeight);
        scrollThumbHeight = 504*scrollPercent;

        if(plName.find(".zip") != string::npos) {
            archive = true;

            Zipfile zip = Zipfile(taiConfigPath+plName);
            zip.Unzip(taiConfigPath+"unzipped/");

            plPath += "unzipped/";

            if( (dir = sceIoDopen( (plPath).c_str() )) != NULL) {
                while (sceIoDread(dir, &dirStruct) != NULL) {
                    if(static_cast<string>(dirStruct.d_name).find("game.txt") != string::npos) {
                        installFiles.clear();
                        tai = false;
                        state = 1;
                        break;
                    }
                    if(
                    static_cast<string>(dirStruct.d_name).find(".suprx") != string::npos ||
                    static_cast<string>(dirStruct.d_name).find(".skprx") != string::npos ||
                    (static_cast<string>(dirStruct.d_name).find("data") != string::npos &&
                    dirStruct.d_stat.st_attr == SCE_SO_IFDIR) ||
                    static_cast<string>(dirStruct.d_name).find(".cfg") != string::npos ||
                    (static_cast<string>(dirStruct.d_name).find(".txt") != string::npos &&
                    toUppercase(static_cast<string>(dirStruct.d_name)).find("INSTALL.TXT") == string::npos &&
                    toUppercase(static_cast<string>(dirStruct.d_name)).find("README.TXT") == string::npos)) {
                        if(taiConfig.find("\n"+static_cast<string>(dirStruct.d_name)) == string::npos)
                        installFiles.push_back(static_cast<string>(dirStruct.d_name));
                    }
                }
            }
            sceIoDclose(dir);
        }
        else if(
        plName.find(".suprx") != string::npos ||
        plName.find(".skprx") != string::npos ||
        plName.find(".vpk") != string::npos) {
            if(taiConfig.find("\n"+plName) == string::npos)
            installFiles.push_back(plName);
        }

        state = 1;
    }

    if(state == 1) {
        if(installFiles[currentPlugin].find(".txt") != string::npos ||
        installFiles[currentPlugin].find(".cfg") != string::npos) {
            Filesystem::copyFile(plPath+installFiles[currentPlugin], taiConfigPath+installFiles[currentPlugin]);
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".skprx") != string::npos) {
            handleSkprx();
        }
        else if(installFiles[currentPlugin].find(".suprx") != string::npos) {
            handleSuprx();
        }
        else if(installFiles[currentPlugin].find(".vpk") != string::npos) {
            // TODO

            // TEMPORARY SOLUTION:
            Filesystem::mkDir(taiConfigPath+"VPKS/");
            Filesystem::copyFile(plPath+installFiles[currentPlugin], taiConfigPath+"VPKS/"+installFiles[currentPlugin]);

            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find("data") != string::npos) {
            Filesystem::copyPath(plPath+"/data", "ux0:data");
            currentPlugin++;
        }

        if(currentPlugin == installFiles.size()) state = 2;
    }

    if(state == 2) {
        Filesystem::writeFile(taiConfigPath+"config.txt", taiConfig);

        if(archive) {
            Filesystem::removePath(plPath);
            sceIoRemove((taiConfigPath+plName).c_str());
        }

        state = 0;
    }
}

Popup::~Popup() {
    vita2d_free_texture(desc);
    vita2d_free_texture(desc2);
}