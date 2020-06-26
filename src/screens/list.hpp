#pragma once

#include <vita2d.h>
#include "../main.hpp"
#include "screen.hpp"
#include "sidebar.hpp"
#include "details.hpp"

class List : public Screen {
    public:
        List();
        void free() override;
        void draw() override;
        void handleInput(Input input) override;

    private:
        Details detailsClass;
        Screen *details = &detailsClass;
        Sidebar sidebarClass;
        Screen *sidebar = &sidebarClass;
        double scrollY = 0;
        double scrollPercent;
        double scrollThumbHeight;
        double scrollThumbY = 0;
        int arrayLength;
        int scrollDelay = 0;
        int scrollStage = 0;
        string searchResult = "";
        vita2d_texture *desc;
};