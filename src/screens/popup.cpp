#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/clib.h>

#include "../main.hpp"
#include "../utils/vhbb/zip.h"
#include "../net/download.hpp"
#include "popup.hpp"

string toUppercase(string strToConvert) {
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

void handleSuprx(SharedData &SharedData, unsigned int button) {

}

void Popup::draw(SharedData &sharedData, unsigned int button) {
    if(state == 0) {
        plName = curlDownloadKeepName(sharedData.plugins[sharedData.cursorY]["url"].get<string>().c_str());
        installFiles.clear();
        plPath = sharedData.taiConfigPath;
        currentPlugin = 0;

        if(plName.find(".zip") != string::npos) {
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
        if(installFiles[currentPlugin].find("data") != string::npos) {
            fs.copyPath(plPath+"/data", "ux0:data");
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".txt") != string::npos ||
        installFiles[currentPlugin].find(".cfg") != string::npos) {
            fs.copyFile(plPath+installFiles[currentPlugin], "ux0:tai/"+installFiles[currentPlugin]);
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".skprx") != string::npos) {
            fs.copyFile(plPath+installFiles[currentPlugin], "ux0:tai/"+installFiles[currentPlugin]);
            sharedData.taiConfig += "*Kernel\n"+sharedData.taiConfigPath+installFiles[currentPlugin];
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".vpk") != string::npos) {
            // TODO
            currentPlugin++;
        }
        else if(installFiles[currentPlugin].find(".suprx") != string::npos) {
            // TODO
            currentPlugin++;
        }

        if(currentPlugin == installFiles.size()) state = 2;
    }

    if(state == 2) {
        fs.writeFile(sharedData.taiConfigPath+"config.txt", sharedData.taiConfig);

        sharedData.scene = 0;
    }
}