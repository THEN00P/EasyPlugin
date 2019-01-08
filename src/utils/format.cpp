#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h> 
#include <string>
#include <fstream>

#include "vpkInstaller/package_installer.h"

using namespace std;

string formatLongDesc(string str) {
    string sentance = "";

    int pos = 0;
    int lastSpacePos = 0;
    int linePos = 0;
    int addedChars = 0;

    for (auto x: str) {
        pos++;
        linePos++;
        if (x == ' ' || x == '-' || x == '.' || x == ',') {
            lastSpacePos = pos;
        }

        sentance += x;

        if (linePos >= 55) {
            linePos = 0;
            sentance.insert(lastSpacePos+addedChars, "\n");

            addedChars++;
        }
    }
    return sentance;
}

void addKernelPL(string plName, string &taiConfig, string taiConfigPath) {
    taiConfig += ("\n*KERNEL\n"+taiConfigPath+plName);
}

void installVPK(string vpkPath) {
    char * writable = new char[vpkPath.size() + 1];
    std::copy(vpkPath.begin(), vpkPath.end(), writable);
    writable[vpkPath.size()] = '\0';

    installPackage(writable);

    delete[] writable;
}

void installPL(string plName, string &taiConfig, string taiConfigPath) {
    if(plName.find(".zip") != string::npos) {
    }
    if(plName.find(".skprx") != string::npos) {
        addKernelPL(plName, taiConfig, taiConfigPath);
    }

    ofstream tat(taiConfigPath+"config.txt");
    tat << taiConfig;
    tat.close();
}

int doesDirExist(const char* path) { 
	SceUID dir = sceIoDopen(path); 
 	if (dir >= 0) { 
 		sceIoDclose(dir); 
 		return 1; 
 	} else { 
 		return 0; 
 	} 
} 