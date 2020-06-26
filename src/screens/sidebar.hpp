#include "screen.hpp"

class Sidebar : public Screen {
    public:
        Sidebar();
        void draw() override;
        void handleInput(Input input) override;
    private:
        int drawOffset = 240;
};