#ifndef INTHORVALUESLIDER_HPP
#define INTHORVALUESLIDER_HPP

#include <FL/Fl.H>
#include <FL/Fl_Hor_Value_Slider.H>

// A Fl_Hor_Value_Slider that increments/decrements by 1 using left/right keys

class IntHorValueSlider : public Fl_Hor_Value_Slider {
public:
    IntHorValueSlider(int X, int Y, int W, int H, const char *l);

    // If within bounds, set value to val and trigger callback
    void setValue(const double val);

    int handle(int e);
};

#endif
