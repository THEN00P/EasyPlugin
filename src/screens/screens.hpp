#pragma once

#include "vector"
#include "screen.hpp"

class Screens {
    public:
        void addScreen(Screen *screen);
        void removeScreen(Screen *screen);
        void update(Input input);
    private:
        std::vector<Screen*> components;
};