#include <psp2/io/dirent.h>
#include <psp2/io/stat.h>

#include "../main.hpp"
#include "../utils/lazy/zip.h"
#include "popup.hpp"

string toUppercase(string strToConvert) {
    std::transform(strToConvert.begin(), strToConvert.end(), strToConvert.begin(), ::toupper);

    return strToConvert;
}

void Popup::draw(SharedData &sharedData, unsigned int button) {
    if(state == 0) {
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
                    static_cast<string>(dirStruct.d_name).find("data") != string::npos ||
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
        }
    }

    if(state == 2) {
        ofstream tat(sharedData.taiConfigPath+"config.txt");
        tat << sharedData.taiConfig;
        tat.close();

        sharedData.scene = 0;
    }
}