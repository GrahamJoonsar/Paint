#include "tigr.h"
#include "slider.hpp"
#include "button.hpp"
#include <math.h>

const TPixel BG_COLOR = tigrRGB(200, 200, 200);

enum DRAWING_MODE{
    LINE,
    CIRCLE,
    SQUARE
};

constexpr int clamp(int min, int max, int n){
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

constexpr float dist(int x0, int y0, int x1, int y1){
    return sqrtf((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
}

void fillRectange(Tigr * bmp, int x0, int y0, int x1, int y1, TPixel c){
    int x = x0 < x1 ? x0 : x1;
    int y = y0 < y1 ? y0 : y1;
    int w = abs(x0-x1);
    int h = abs(y0-y1);
    tigrFill(bmp, x, y, w, h, c);
}

void paint(Tigr * screen, Tigr * painting, TPixel color, DRAWING_MODE d, int mx, int my, int buttons){
    switch(d){
        case DRAWING_MODE::LINE: {
                static int lx = mx, ly = my;
                if ((buttons & 1)){
                    tigrLine(painting, lx, ly, mx, my, color);
                }
                lx = mx;
                ly = my;
                break;
        }
        case DRAWING_MODE::SQUARE: {
            static int px, py;
            static bool selected = false;
            if ((buttons & 1) && !selected){
                px = clamp(0, 320, mx);
                py = clamp(0, 320, my);
                selected = true;
            } else if ((buttons & 1) && selected){
                fillRectange(screen, px, py, clamp(0, 320, mx), clamp(0, 320, my), color);
            } else if (!(buttons & 1) && selected){
                selected = false;
                fillRectange(painting, px, py, clamp(0, 320, mx), clamp(0, 320, my), color);
            }
            break;
        }
        case DRAWING_MODE::CIRCLE: {
            static int px, py;
            static bool selected = false;
            if ((buttons & 1) && !selected){
                px = clamp(0, 320, mx);
                py = clamp(0, 320, my);
                selected = true;
            } else if ((buttons & 1) && selected){
                mx = clamp(0, 320, mx);
                my = clamp(0, 320, my);
                tigrFillCircle(screen, (mx+px)/2, (my+py)/2, 
                (int)dist(px, py, mx, my), color);
            } else if (!(buttons & 1) && selected){
                selected = false;
                mx = clamp(0, 320, mx);
                my = clamp(0, 320, my);
                tigrFillCircle(painting, (mx+px)/2, (my+py)/2, 
                (int)dist(px, py, mx, my), color);
            }
            break;
        }
    }
}

const TPixel RED = tigrRGB(255, 0, 0);
const TPixel GREEN = tigrRGB(0, 255, 0);
const TPixel BLUE = tigrRGB(0, 0, 255);
const TPixel BLACK = tigrRGB(0, 0, 0);

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

    DRAWING_MODE d = DRAWING_MODE::CIRCLE;

    TPixel currentColor = tigrRGB(127, 127, 127);

    int mx, my, buttons;

    while (!tigrClosed(screen)){
        tigrClear(screen, BG_COLOR);

        tigrMouse(screen, &mx, &my, &buttons);

        unsigned char r = redSlider.getVal();
        unsigned char g = greenSlider.getVal();
        unsigned char b = blueSlider.getVal();

        redSlider.setKnobColor(  tigrRGB(r, 0, 0));
        greenSlider.setKnobColor(tigrRGB(0, g, 0));
        blueSlider.setKnobColor( tigrRGB(0, 0, b));

        redSlider.update(screen);
        redSlider.draw(screen);
        greenSlider.update(screen);
        greenSlider.draw(screen);
        blueSlider.update(screen);
        blueSlider.draw(screen);

        saveColorButton.update(screen, {&currentColor, &r, &g, &b});
        saveColorButton.draw(screen);

        tigrPrint(screen, tfont, 610, 5, RED, "RED: %d", r);
        tigrPrint(screen, tfont, 610, 20, GREEN, "GREEN: %d", g);
        tigrPrint(screen, tfont, 610, 35, BLUE, "BLUE: %d", b);

        tigrFill(screen, 330, 50, 260, 14, tigrRGB(r, g, b));
        tigrLine(screen, 330, 64, 590, 64, BLACK);
        tigrFill(screen, 330, 65, 260, 14, currentColor);

        tigrBlit(screen, painting, 0, 0, 0, 0, 320, 320);
        if (!redSlider.isSelected() && !greenSlider.isSelected() && !blueSlider.isSelected())
            paint(screen, painting, currentColor, d, mx, my, buttons);

        tigrUpdate(screen);
    }
}
