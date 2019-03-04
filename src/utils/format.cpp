#pragma once

#include <psp2/io/fcntl.h>
#include <psp2/io/dirent.h> 
#include <string>
#include <fstream>

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