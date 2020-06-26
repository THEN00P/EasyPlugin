#include "../main.hpp"
#include "sidebar.hpp"
#include "screens.hpp"
#include <psp2/kernel/clib.h>

extern Screens screens;

//TODO: Search, Sort, Settings, About Screen

Sidebar::Sidebar() {
    transparency = true;
    zIndex = 9;
}

void Sidebar::draw() {
    vita2d_draw_rectangle(720+drawOffset, 0, 240, 544, RGBA8(0,0,0,234));
    if(drawOffset > 0) drawOffset -= 30;
}

void Sidebar::handleInput(Input input) {
    if(input.newButtonsPressed(SCE_CTRL_TRIANGLE)) screens.removeScreen(this);
}