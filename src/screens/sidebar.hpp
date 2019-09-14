#include "screen.hpp"

class Sidebar : public Screen {
    public:
        void draw() override;
        void handleInput(Input input) override;
        int zIndex = 10;
    private:
        int drawOffset = 240;
};