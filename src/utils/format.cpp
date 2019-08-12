#pragma once

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h> 
#include <psp2/kernel/clib.h>
#include <vita2d.h>
#include <string>
#include <fstream>

using namespace std;

string formatLongDesc(string str, vita2d_font *font, int maxWidth, int size) {
    string description = "";
    int lastSafePos = 0;
    int addedChars = 1;

    for(int i=0; i < str.length(); i++) {        
        description += str[i];

        if(vita2d_font_text_width(font, size, description.c_str()) > maxWidth) {
            description.insert((lastSafePos == 0 ? i : lastSafePos)+addedChars, "\n");

            addedChars++;
        }
        
        if(str[i] == ' ' || str[i] == '.' || str[i] == ',' || str[i] == '-' || str[i] == ')') {
            lastSafePos = i;
        }
    }

    return description;
}