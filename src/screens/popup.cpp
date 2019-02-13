#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>

#include "../main.hpp"
#include "../utils/vhbb/zip.h"
#include "../net/download.hpp"
#include "popup.hpp"

string toUppercase(string strToConvert) {
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

int scrollY = 0;

void handleSuprx(SharedData &sharedData, int &currentPlugin, unsigned int button) {
    vita2d_draw_rectangle(0, 0, 960, 544, RGBA8(234,234,234,255));

    for(int i=0;i<sharedData.appData.size();i++) {
        if((i*128)-scrollY>544) break;
        if((i*128)-scrollY<0) continue;

        vita2d_draw_texture(sharedData.appData[i].icon, 10, (i*128)+48-scrollY);
        vita2d_font_draw_textf(sharedData.font, 80, (i*128)+48-scrollY, RGBA8(16,16,16,255), 32, "%s", sharedData.appData[i].title);
    }

    switch(button) {
        case SCE_CTRL_DOWN:
            scrollY++;
            break;
        case SCE_CTRL_UP:
            scrollY--;
            break;
        case SCE_CTRL_CROSS:
            currentPlugin++;
            break;
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

            if( (dir = sceIoDopen( (sharedData.taiConfigPath+"unzipped/").c_str() )) != NULL) {
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
            installFiles.push_back(plName);
        }

        state = 1;
    }

    if(state == 1) {
        if(installFiles[currentPlugin].find(".txt") != string::npos ||
        installFiles[currentPlugin].find(".cfg") != string::npos) {
            fs.copyFile(plPath+installFiles[currentPlugin], sharedData.taiConfigPath+installFiles[currentPlugin]);
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".skprx") != string::npos &&
        sharedData.taiConfig.find(installFiles[currentPlugin]) == string::npos) {
            fs.copyFile(plPath+installFiles[currentPlugin], sharedData.taiConfigPath+installFiles[currentPlugin]);
            sharedData.taiConfig += "\n\n*Kernel\n"+sharedData.taiConfigPath+installFiles[currentPlugin];
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".vpk") != string::npos) {
            // TODO
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".suprx") != string::npos &&
        sharedData.taiConfig.find(installFiles[currentPlugin]) == string::npos) {
            handleSuprx(sharedData, currentPlugin, button);
        }
        else if(installFiles[currentPlugin].find("data") != string::npos) {
            fs.copyPath(plPath+"/data", "ux0:data");
            currentPlugin++;
        }

        if(currentPlugin == installFiles.size()-1) state = 2;
    }

    if(state == 2) {
        fs.writeFile(sharedData.taiConfigPath+"config.txt", sharedData.taiConfig);
        if(archive) {
            fs.removePath(plPath);
            sceIoRemove((sharedData.taiConfigPath+plName).c_str());
        }

        sharedData.scene = 0;
    }
}