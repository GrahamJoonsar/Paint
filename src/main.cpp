#include "tigr.h"
#include "slider.hpp"
#include "button.hpp"
#include <math.h>
#include <iostream>

const TPixel BG_COLOR = tigrRGB(200, 200, 200);

enum DRAWING_MODE{
    LINE,
    CIRCLE,
    RECT,
    FILL
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

constexpr bool eq(TPixel t1, TPixel t2){
    return t1.r == t2.r && t1.g == t2.g && t1.b == t2.b && t1.a == t2.a;
}

TPixel currentColor;
TPixel newColor;
void spreadFill(Tigr * bmp, int x, int y){
    if (!eq(bmp->pix[y*bmp->w + x], currentColor)) return;
    bmp->pix[y*bmp->w + x] = newColor;
    
    if (y > 0) spreadFill(bmp, x, y-1);
    if (y < bmp->h) spreadFill(bmp, x, y+1);
    if (x < bmp->w) spreadFill(bmp, x+1, y);
    if (x > 0) spreadFill(bmp, x-1, y);
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
        case DRAWING_MODE::RECT: {
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
        case DRAWING_MODE::FILL: {
            if ((buttons & 1) && (0 < mx && mx < 320 && 0 < my && my < 320)){
                currentColor = painting->pix[(my*painting->w) + mx];
                newColor = color;
                if (eq(currentColor, newColor)) return;
                spreadFill(painting, mx, my);
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

    Button lineModeButton(330, 80, "LINE");
    lineModeButton.setFunction([](std::vector<void*> args){
        *static_cast<DRAWING_MODE*>(args[0]) = DRAWING_MODE::LINE;
    });
    Button rectModeButton(370, 80, "RECT");
    rectModeButton.setFunction([](std::vector<void*> args){
        *static_cast<DRAWING_MODE*>(args[0]) = DRAWING_MODE::RECT;
    });
    Button circleModeButton(410, 80, "CIRCLE");
    circleModeButton.setFunction([](std::vector<void*> args){
        *static_cast<DRAWING_MODE*>(args[0]) = DRAWING_MODE::CIRCLE;
    });
    Button fillModeButton(464, 80, "FILL");
    fillModeButton.setFunction([](std::vector<void*> args){
        *static_cast<DRAWING_MODE*>(args[0]) = DRAWING_MODE::FILL;
    });

    DRAWING_MODE d = DRAWING_MODE::LINE;

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

        lineModeButton.update(screen, {&d});
        lineModeButton.draw(screen);
        rectModeButton.update(screen, {&d});
        rectModeButton.draw(screen);
        circleModeButton.update(screen, {&d});
        circleModeButton.draw(screen);
        fillModeButton.update(screen, {&d});
        fillModeButton.draw(screen);

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
