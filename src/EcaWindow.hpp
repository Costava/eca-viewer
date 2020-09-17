#ifndef ECAWINDOW_HPP
#define ECAWINDOW_HPP

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

// This class is currently pointless but is left in place
//  in case Fl_Window needs to be extended in the future

class EcaWindow : public Fl_Window {
public:
    EcaWindow(int width, int height, const char *title);

    // int handle(int e);
};

#endif
