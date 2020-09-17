#include "EcaWindow.hpp"
#include <iostream>

EcaWindow::EcaWindow(int width, int height, const char *title) : Fl_Window(width, height, title) {
    // Nothing (base constructor called above)
}

// int EcaWindow::handle(int e) {
//     // "To receive FL_KEYBOARD events you must also respond
//     //   to the FL_FOCUS and FL_UNFOCUS events by returning 1."
//     // via https://www.fltk.org/doc-1.3/events.html
//     if (e == FL_FOCUS || e == FL_UNFOCUS) {
//         return 1;
//     }
//
//     if (e == FL_KEYDOWN && Fl::event_key() == 'e') {
//         std::cout << "e\n";
//         return 1;
//     }
//
//     return Fl_Window::handle(e);
// }
