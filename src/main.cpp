#include <cstdlib>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

enum InitialState {
    InitialState_CENTER_BIT,
    InitialState_CENTER_BYTE,
    InitialState_EVEN,
    InitialState_ODD,
    InitialState_ALL,
    InitialState_NONE,
    InitialState_RANDOM
};

enum InitialState initialStateFromVoidPtr(void *const ptr) {
    if (ptr == (void*)InitialState_CENTER_BIT) {
        return InitialState_CENTER_BIT;
    }
    if (ptr == (void*)InitialState_CENTER_BYTE) {
        return InitialState_CENTER_BYTE;
    }
    if (ptr == (void*)InitialState_EVEN) {
        return InitialState_EVEN;
    }
    if (ptr == (void*)InitialState_ODD) {
        return InitialState_ODD;
    }
    if (ptr == (void*)InitialState_ALL) {
        return InitialState_ALL;
    }
    if (ptr == (void*)InitialState_NONE) {
        return InitialState_NONE;
    }
    if (ptr == (void*)InitialState_RANDOM) {
        return InitialState_RANDOM;
    }

    std::cerr << "InitialState value cannot be identified\n";
    exit(1);
}

enum ColorMode {
    ColorMode_RGB888,
    ColorMode_GRAYSCALE8
};

enum ColorMode colorModeFromVoidPtr(void *const ptr) {
    if (ptr == (void*)ColorMode_RGB888) {
        return ColorMode_RGB888;
    }
    if (ptr == (void*)ColorMode_GRAYSCALE8) {
        return ColorMode_GRAYSCALE8;
    }

    std::cerr << "ColorMode value cannot be identified\n";
    exit(1);
}

struct Config {
    enum InitialState initialState;
    enum ColorMode colorMode;
    bool wrap;
    unsigned char rule;
} config;

Fl_Window *window;
Fl_Box *box;

// Populate first row of buffer
void populateFirstRow(unsigned char *const buf, const int rowLen, const struct Config &config) {
    const int centerByteIndex = rowLen / 2;

    switch (config.initialState) {
        case InitialState_CENTER_BIT:
            for (int i = 0; i < rowLen; i += 1) {
                buf[i] = 0;
            }

            buf[centerByteIndex] = 0b00001000;
            break;
        case InitialState_CENTER_BYTE:
            for (int i = 0; i < rowLen; i += 1) {
                buf[i] = 0;
            }

            buf[centerByteIndex] = 0xff;
            break;
        case InitialState_EVEN:
            for (int i = 0; i < rowLen; i += 1) {
                if ((i % 2) == 0) {
                    buf[i] = 0xff;
                }
                else {
                    buf[i] = 0;
                }
            }
            break;
        case InitialState_ODD:
            for (int i = 0; i < rowLen; i += 1) {
                if ((i % 2) == 0) {
                    buf[i] = 0;
                }
                else {
                    buf[i] = 0xff;
                }
            }
            break;
        case InitialState_ALL:
            for (int i = 0; i < rowLen; i += 1) {
                buf[i] = 0xff;
            }
            break;
        case InitialState_NONE:
            for (int i = 0; i < rowLen; i += 1) {
                buf[i] = 0;
            }
            break;
        case InitialState_RANDOM:
            for (int i = 0; i < rowLen; i += 1) {
                buf[i] = rand() % 256;
            }
            break;
        default:
            std::cerr << __func__ << ": Unknown initialState value. Exiting.\n";
            exit(1);
    }
}

void updateEca(const struct Config &config) {
    // std::cout << "updateEca\n";

    // Get rid of old image if it exists
    Fl_Image *const oldImage = box->image();
    if (oldImage != NULL) {
        // std::cout << "Getting rid of old image\n";

        Fl_RGB_Image *const oldRgbImage = dynamic_cast<Fl_RGB_Image*>(oldImage);

        if (oldRgbImage) {
            const uchar *oldBuf = oldRgbImage->array;

            box->image(NULL);
            delete oldRgbImage;
            delete oldBuf;
        }
        else {
            std::cerr << "box->image() is not Fl_RGB_Image. Bad state.\n";
            exit(1);
        }
    }

    // New size of image
    const int width = box->w();
    const int height = box->h();

    const int multiplier = (config.colorMode == ColorMode_RGB888) ? 3 : 1;

    const int bufLen = multiplier * width * height;
    unsigned char *buf = new unsigned char[bufLen];

    // Populate first row
    const int rowLen = multiplier * width;
    populateFirstRow(buf, rowLen, config);

    // Populate remaining rows
    // Iterate rows
    for (int row = 0; row < height - 1; row += 1) {
        const unsigned char *const rowA = &buf[(row + 0) * rowLen];
        unsigned char *const rowB = &buf[(row + 1) * rowLen];

        // Iterate bytes in row
        for (int i = 0; i < rowLen; i += 1) {
            unsigned char resultByte = 0;

            unsigned char leftWrapBit;
            if (i > 0) {
                leftWrapBit = (rowA[i - 1] & 0b10000000) > 0;
            }
            else if (config.wrap) {
                leftWrapBit = (rowA[rowLen - 1] & 0b10000000) > 0;
            }
            else {
                leftWrapBit = 0;
            }

            unsigned char rightWrapBit;
            if (i < rowLen - 1) {
                rightWrapBit = rowA[i + 1] & 0b1;
            }
            else if (config.wrap) {
                rightWrapBit = rowA[0] & 0b1;
            }
            else {
                rightWrapBit = 0;
            }

            const unsigned char byte = rowA[i];

            // Iterate bits in byte
            for (int b = 0; b < 8; b += 1) {
                const unsigned char leftBit  = (b == 0) ? leftWrapBit  : ((byte >> (b - 1)) & 0b1);
                const unsigned char rightBit = (b == 7) ? rightWrapBit : ((byte >> (b + 1)) & 0b1);
                const unsigned char bit = (byte >> b) & 0b1;

                const unsigned char indexIntoRule = 0 | leftBit | (bit << 1) | (rightBit << 2);

                const unsigned char newBit = (config.rule >> indexIntoRule) & 0b1;
                resultByte |= (newBit << b);
            }

            rowB[i] = resultByte;
        }
    }

    const int D = (config.colorMode == ColorMode_RGB888) ? 3 : 1;

    // We remain responsible for deallocating buf (not the image object)
    Fl_RGB_Image *image = new Fl_RGB_Image(buf, width, height, D, 0);
    box->image(image);
    window->redraw();// Avoid visual artifacts
}

void initialStateChangeCallback(Fl_Widget *w, void *data) {
    enum InitialState initialState = initialStateFromVoidPtr(data);
    config.initialState = initialState;
    updateEca(config);
}

void colorModeChangeCallback(Fl_Widget *w, void *data) {
    enum ColorMode colorMode = colorModeFromVoidPtr(data);
    config.colorMode = colorMode;
    updateEca(config);
}

void wrapChangeCallback(Fl_Widget *w, void *data) {
    Fl_Check_Button *const button = dynamic_cast<Fl_Check_Button*>(w);

    if (button) {
        config.wrap = (button->value() != 0);
        updateEca(config);
    }
}

int main(int argc, char **argv) {
    // Layout constants
    const int initialWindowWidth = 1000;
    const int initialWindowHeight = 500;
    const int rightColumnWidth = 200;
    const int radioButtonHeight = 30;
    const int headerHeight = 30;
    // const int margin = 15;
    const int imageAreaWidth = initialWindowWidth - rightColumnWidth;
    const char *const windowTitle = "Elementary Cellular Automata Viewer";

    window = new Fl_Window(initialWindowWidth, initialWindowHeight, windowTitle);
    Fl_Pack *const windowPack = new Fl_Pack(0, 0, initialWindowWidth, initialWindowHeight);
    windowPack->type(Fl_Pack::HORIZONTAL);
    window->add(windowPack);
    window->resizable(windowPack);

    // You can sometimes get away with setting the width of children
    //  in a vertical pack to 0 or the height of children to 0
    //  in a horizontal pack since the children should/will take the full width/height of the pack,
    //  but sometimes that seems to cause segfaults or other strange fatal errors.
    // So specify both width and height for children of packs when things seem broken.

    // If in the future you want to add a margin, do not try a pack inside a pack.
    //  It worked but the margins got inconsistent at larger (approaching 1080p screen size) resolutions (why??)
    //  For margins, try using Tile instead (untested): https://www.fltk.org/doc-1.3/classFl__Tile.html

    // This box will use an image label
    box = new Fl_Box(0, 0, imageAreaWidth, initialWindowHeight);
    // box->box(FL_BORDER_BOX);// For debugging
    // Inexplicably, this causes the image not to show
    // box->labeltype(_FL_IMAGE_LABEL);
    windowPack->add(box);
    windowPack->resizable(box);

    Fl_Pack *const rightPack = new Fl_Pack(0, 0, rightColumnWidth, initialWindowHeight);
    windowPack->add(rightPack);

    Fl_Box *const initialStateBox = new Fl_Box(0, 0, 0, headerHeight, "Initial State");
    initialStateBox->box(FL_PLASTIC_UP_BOX);
    rightPack->add(initialStateBox);

    Fl_Pack *const initialStateRadioPack = new Fl_Pack(0, 0, rightColumnWidth, 0);
    rightPack->add(initialStateRadioPack);

    // Each set of radio buttons has its own pack because different sets
    //  of radio buttons need to be in different groups
    //  in order to work like radio buttons (only one selected at a time)
    initialStateRadioPack->begin();
    Fl_Radio_Round_Button *const initialCenterBitButton =  new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Center bit");
    Fl_Radio_Round_Button *const initialCenterByteButton = new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Center byte");
    Fl_Radio_Round_Button *const initialEvenButton =       new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Even");
    Fl_Radio_Round_Button *const initialOddButton =        new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Odd");
    Fl_Radio_Round_Button *const initialAllButton =        new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "All");
    Fl_Radio_Round_Button *const initialNoneButton =       new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "None");
    Fl_Radio_Round_Button *const initialRandomButton =     new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Random");
    initialStateRadioPack->end();

    // Changes nothing
    // initialCenterBitButton->color(fl_rgb_color(255, 255, 0));

    Fl_Box *const colorBox = new Fl_Box(0, 0, 0, headerHeight, "Color");
    colorBox->box(FL_PLASTIC_UP_BOX);
    rightPack->add(colorBox);

    Fl_Pack *const colorRadioPack = new Fl_Pack(0, 0, 0, 0);
    rightPack->add(colorRadioPack);

    colorRadioPack->begin();
    Fl_Radio_Round_Button *const rgbButton = new Fl_Radio_Round_Button(
        0, 0, 0, radioButtonHeight, "RGB 888");
    Fl_Radio_Round_Button *const grayscaleButton = new Fl_Radio_Round_Button(
        0, 0, 0, radioButtonHeight, "Grayscale 8");
    colorRadioPack->end();

    Fl_Box *const optionsBox = new Fl_Box(0, 0, 0, headerHeight, "Options");
    optionsBox->box(FL_PLASTIC_UP_BOX);
    rightPack->add(optionsBox);

    Fl_Check_Button *const wrapButton = new Fl_Check_Button(0, 0, 0, radioButtonHeight, "Wrap");
    rightPack->add(wrapButton);

    // Initialize the config and the UI
    config.initialState = InitialState_CENTER_BIT;
    config.colorMode = ColorMode_RGB888;
    config.wrap = true;
    config.rule = 18;
    initialCenterBitButton->setonly();
    rgbButton->setonly();
    wrapButton->set();
    updateEca(config);

    // Casting enum value to void ptr is not ideal but it works
    // The radio button callbacks are only called when the button is set (never when unset)
    initialCenterBitButton->callback( initialStateChangeCallback, (void*)InitialState_CENTER_BIT);
    initialCenterByteButton->callback(initialStateChangeCallback, (void*)InitialState_CENTER_BYTE);
    initialEvenButton->callback(      initialStateChangeCallback, (void*)InitialState_EVEN);
    initialOddButton->callback(       initialStateChangeCallback, (void*)InitialState_ODD);
    initialAllButton->callback(       initialStateChangeCallback, (void*)InitialState_ALL);
    initialNoneButton->callback(      initialStateChangeCallback, (void*)InitialState_NONE);
    initialRandomButton->callback(    initialStateChangeCallback, (void*)InitialState_RANDOM);

    rgbButton->callback(colorModeChangeCallback, (void*)ColorMode_RGB888);
    grayscaleButton->callback(colorModeChangeCallback, (void*)ColorMode_GRAYSCALE8);

    // Checkbox callback gets triggered for both set and unset
    wrapButton->callback(wrapChangeCallback, NULL);

    window->show(argc, argv);
    return Fl::run();
}