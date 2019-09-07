#include "input.hpp"

void Input::updateLoop() {
    padOld = pad;
    
    sceCtrlPeekBufferPositive(0, &pad, 1);
}

bool Input::buttonsPressed(int buttons) {
    return (pad.buttons & buttons);
}

bool Input::newButtonsPressed(int buttons) {
    return (pad.buttons & buttons) && !(padOld.buttons & buttons);
}