#include "IntHorValueSlider.hpp"
#include <iostream>

IntHorValueSlider::IntHorValueSlider(int X, int Y, int W, int H, const char *l) : Fl_Hor_Value_Slider(X, Y, W, H, l) {
    // Nothing (base constructor called above)
}

int IntHorValueSlider::value(double v) {
    return Fl_Hor_Value_Slider::value(clamp(round(v)));
}

int IntHorValueSlider::handle(int e) {
    // "To receive FL_KEYBOARD events you must also respond
    //   to the FL_FOCUS and FL_UNFOCUS events by returning 1."
    // via https://www.fltk.org/doc-1.3/events.html
    if (e == FL_FOCUS || e == FL_UNFOCUS) {
        return 1;
    }

    if (e == FL_KEYDOWN && Fl::event_key() == FL_Left) {
        value(Fl_Hor_Value_Slider::value() - step());
        return 1;
    }
    if (e == FL_KEYDOWN && Fl::event_key() == FL_Right) {
        value(Fl_Hor_Value_Slider::value() + step());
        return 1;
    }

    return Fl_Hor_Value_Slider::handle(e);
}
