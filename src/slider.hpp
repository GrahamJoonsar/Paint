#pragma once

#include "tigr.h"

class Slider{
    private:
        static bool selected;
        bool this_selected;
        int x, y, w;
        double min, max;
        double step;
        double current;
        TPixel BG;
        TPixel KNOB_COLOR;

        double clamp(double c) const;

        double roundToStep(double c) const;

    public:
        Slider(int x, int y, int w, double min, double max, double step);
        void draw (Tigr * screen) const;
        void update(Tigr * screen);
        
        inline void setKnobColor(const TPixel c){
            KNOB_COLOR = c;
        }
        inline void setBGColor(const TPixel c){
            BG = c;
        }

        inline bool isSelected() const {
            return this_selected;
        }
        
        inline double getVal() const {
            return current;
        }
};