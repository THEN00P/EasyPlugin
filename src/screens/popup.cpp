#include <algorithm>

#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>

#include "../main.hpp"
#include "../utils/vhbb/zip.h"
#include "../net/download.hpp"
#include "popup.hpp"

#define columnHeight 168

string toUppercase(string strToConvert) {
    transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

void Popup::handleSuprx(SharedData &sharedData, int &currentPlugin, unsigned int button) {
    if(scrollDelay >= 0) scrollDelay--;
    if(button == NULL) {
        scrollDelay = 0; 
        scrollStage = 0;
    }
    
    if(selected*columnHeight > scrollY+374) scrollY += columnHeight;
    if(selected*columnHeight < scrollY) scrollY -= columnHeight;

    vita2d_draw_rectangle(0, (selected*columnHeight)-scrollY, 960, columnHeight, RGBA8(0,0,0,60));

    for(int i=0;i<sharedData.appData.size();i++) {

        if((i*columnHeight)-scrollY>544) break;
        if((i*columnHeight)+178-scrollY<0) continue;

        if(sharedData.appData[i].icon != NULL)
        vita2d_draw_texture(sharedData.appData[i].icon, 40, (i*columnHeight)+20-scrollY);
        else vita2d_draw_rectangle(40, (i*columnHeight)+20-scrollY, 128, 128, RGBA8(255,255,255,255));
        
        vita2d_font_draw_textf(sharedData.font, 190, (i*columnHeight)+90-scrollY, RGBA8(255,255,255,255), 45, "%s", sharedData.appData[i].title.c_str());

        vita2d_draw_rectangle(870, (i*columnHeight)+55-scrollY, 42, 42, RGBA8(255,255,255,255));

        if(find(selectedApps.begin(), selectedApps.end(), i) != selectedApps.end()) {
            vita2d_draw_rectangle(875, (i*columnHeight)+60-scrollY, 32, 32, RGBA8(38,166,242,255));
        }
    }

    vita2d_draw_texture(desc, 0, 504);
    vita2d_font_draw_textf(sharedData.font, 20, 535, RGBA8(255,255,255,255), 30, "%s", installFiles[currentPlugin].c_str());

    if(scrollDelay <= 1) {
        if(scrollDelay == 0) scrollStage = 0;
        switch(button) {
            case SCE_CTRL_START:
                for(int index : selectedApps) {
                    if(sharedData.taiConfig.find("\n\n*"+sharedData.appData[index].appID+"\n"+sharedData.taiConfigPath+installFiles[currentPlugin]) == string::npos)
                    sharedData.taiConfig += "\n\n*"+sharedData.appData[index].appID+"\n"+sharedData.taiConfigPath+installFiles[currentPlugin];
                }
                
                selected = 0;
                sharedData.blockStart = true;
                selectedApps.clear();
                currentPlugin++;
                break;
            case SCE_CTRL_CIRCLE:
                selected = 0;
                selectedApps.clear();
                currentPlugin++;
                break;
            case SCE_CTRL_CROSS:
                if(!sharedData.blockCross) {
                    sharedData.blockCross = true;

                    auto found = find(selectedApps.begin(), selectedApps.end(), selected);
                    if(found != selectedApps.end()) {
                        auto index = distance(selectedApps.begin(), found);
                        selectedApps.erase(selectedApps.begin()+index);
                    }
                    else {
                        selectedApps.push_back(selected);
                    }
                }
                break;
            case SCE_CTRL_DOWN:
                if(selected >= sharedData.appData.size()-1) break;

                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 10;
                }
                else {
                    scrollDelay = 3;
                }
                selected++;
                break;
            case SCE_CTRL_UP:
                if(selected <= 0) break;

                if(scrollStage <= 10) {
                    if(scrollDelay == 1) scrollStage++;
                    scrollDelay = 10;
                }
                else {
                    scrollDelay = 3;
                }
                selected--;
                break;
        }
    }
}

void Popup::draw(SharedData &sharedData, unsigned int button) {
    if(state == 0) {
        plName = curlDownloadKeepName(sharedData.plugins[sharedData.cursorY]["url"].get<string>().c_str(), sharedData.taiConfigPath);
        installFiles.clear();
        archive = false;
        plPath = sharedData.taiConfigPath;
        currentPlugin = 0;

        if(plName.find(".zip") != string::npos) {
            archive = true;

            Zipfile zip = Zipfile(sharedData.taiConfigPath+plName);
            zip.Unzip(sharedData.taiConfigPath+"unzipped/");

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
                        if(sharedData.taiConfig.find("\n"+static_cast<string>(dirStruct.d_name)) == string::npos)
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
            if(sharedData.taiConfig.find("\n"+plName) == string::npos)
            installFiles.push_back(plName);
        }

        state = 1;
    }

    if(state == 1) {
        if(installFiles[currentPlugin].find(".txt") != string::npos ||
        installFiles[currentPlugin].find(".cfg") != string::npos) {
            Filesystem::copyFile(plPath+installFiles[currentPlugin], sharedData.taiConfigPath+installFiles[currentPlugin]);
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".skprx") != string::npos) {
            Filesystem::copyFile(plPath+installFiles[currentPlugin], sharedData.taiConfigPath+installFiles[currentPlugin]);
            sharedData.taiConfig += "\n\n*Kernel\n"+sharedData.taiConfigPath+installFiles[currentPlugin];
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".vpk") != string::npos) {
            // TODO

            // TEMPORARY SOLUTION:
            Filesystem::mkDir(sharedData.taiConfigPath+"VPKS/");
            Filesystem::copyFile(plPath+installFiles[currentPlugin], sharedData.taiConfigPath+"VPKS/"+installFiles[currentPlugin]);

            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".suprx") != string::npos) {
            handleSuprx(sharedData, currentPlugin, button);
        }
        else if(installFiles[currentPlugin].find("data") != string::npos) {
            Filesystem::copyPath(plPath+"/data", "ux0:data");
            currentPlugin++;
        }

        if(currentPlugin == installFiles.size()) state = 2;
    }

    if(state == 2) {
        Filesystem::writeFile(sharedData.taiConfigPath+"config.txt", sharedData.taiConfig);
        if(archive) {
            Filesystem::removePath(plPath);
            sceIoRemove((sharedData.taiConfigPath+plName).c_str());
        }

        sharedData.scene = 0;
    }
}

void Popup::free() {
    vita2d_free_texture(desc);
}