#include "tigr.h"
#include "slider.hpp"
#include "button.hpp"

const TPixel BG_COLOR = tigrRGB(200, 200, 200);

void paint(Tigr * screen, Tigr * painting, TPixel color){
    int mx, my, buttons;
    tigrMouse(screen, &mx, &my, &buttons);
    static int lx = mx, ly = my;
    if ((buttons & 1) && (mx > 0 && mx < painting->w) && (my > 0 && my < painting->h)){
        tigrLine(painting, lx, ly, mx, my, color);
    }
    lx = mx;
    ly = my;
}

const TPixel RED = tigrRGB(255, 0, 0);
const TPixel GREEN = tigrRGB(0, 255, 0);
const TPixel BLUE = tigrRGB(0, 0, 255);

int main(void){
    Tigr * painting = tigrBitmap(320, 320);
    Tigr * screen = tigrWindow(700, 320, "PAINT", 0);

    Slider redSlider(330, 5, 260, 0, 255, 1);
    Slider greenSlider(330, 20, 260, 0, 255, 1);
    Slider blueSlider(330, 35, 260, 0, 255, 1);

    Button saveColorButton(610, 50, "CHOOSE");
    saveColorButton.setFunction([](std::vector<void*> args){
        TPixel * color = static_cast<TPixel*>(args[0]);
        *color = tigrRGB(*static_cast<char*>(args[1]), *static_cast<char*>(args[2]), *static_cast<char*>(args[3]));
    });

    TPixel currentColor = tigrRGB(127, 127, 127);

    while (!tigrClosed(screen)){
        tigrClear(screen, BG_COLOR);

        redSlider.update(screen);
        redSlider.draw(screen);
        greenSlider.update(screen);
        greenSlider.draw(screen);
        blueSlider.update(screen);
        blueSlider.draw(screen);

        unsigned char r = redSlider.getVal();
        unsigned char g = greenSlider.getVal();
        unsigned char b = blueSlider.getVal();

        saveColorButton.update(screen, {&currentColor, &r, &g, &b});
        saveColorButton.draw(screen);

        tigrPrint(screen, tfont, 610, 5, RED, "RED: %d", r);
        tigrPrint(screen, tfont, 610, 20, GREEN, "GREEN: %d", g);
        tigrPrint(screen, tfont, 610, 35, BLUE, "BLUE: %d", b);

        tigrFill(screen, 330, 50, 260, 14, tigrRGB(r, g, b));
        tigrFill(screen, 330, 64, 260, 14, currentColor);

        paint(screen, painting, currentColor);
        tigrBlit(screen, painting, 0, 0, 0, 0, 320, 320);

        tigrUpdate(screen);
    }
}
