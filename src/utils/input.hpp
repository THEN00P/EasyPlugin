#pragma once

#include <psp2/ctrl.h>

class Input {
    private:
        SceCtrlData pad;
        SceCtrlData padOld;
    public:
        bool newButtonsPressed(int buttons);
        bool buttonsPressed(int buttons);
        void updateLoop();
};