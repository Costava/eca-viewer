#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Window.H>

enum InitialState {
    InitialState_CENTER_BIT,
    InitialState_CENTER_BYTE,
    InitialState_EVEN,
    InitialState_ODD,
    InitialState_EVERY_XTH,
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
    if (ptr == (void*)InitialState_EVERY_XTH) {
        return InitialState_EVERY_XTH;
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
    unsigned int xth;
} config;

// Globals: below and config above

Fl_Window *window;
Fl_Box *box;

void populateRowCenterBit(unsigned char *const row, const int rowLen) {
    const int centerByteIndex = rowLen / 2;

    for (int i = 0; i < rowLen; i += 1) {
        row[i] = 0;
    }

    row[centerByteIndex] = 0b00001000;
}

void populateRowCenterByte(unsigned char *const row, const int rowLen) {
    const int centerByteIndex = rowLen / 2;

    for (int i = 0; i < rowLen; i += 1) {
        row[i] = 0;
    }

    row[centerByteIndex] = 0xff;
}

void populateRowEven(unsigned char *const row, const int rowLen) {
    for (int i = 0; i < rowLen; i += 1) {
        if ((i % 2) == 0) {
            row[i] = 0xff;
        }
        else {
            row[i] = 0;
        }
    }
}

void populateRowOdd(unsigned char *const row, const int rowLen) {
    for (int i = 0; i < rowLen; i += 1) {
        if ((i % 2) == 0) {
            row[i] = 0;
        }
        else {
            row[i] = 0xff;
        }
    }
}

void populateRowEveryXth(unsigned char *const row, const int rowLen, const unsigned int xth) {
    for (int i = 0; i < rowLen; i += 1) {
        if (xth == 1) {
            row[i] = 0xff;
        }
        else if (i != 0 && (i % xth) == 0) {
            row[i] = 0xff;
        }
        else {
            row[i] = 0;
        }
    }
}

void populateRowAll(unsigned char *const row, const int rowLen) {
    for (int i = 0; i < rowLen; i += 1) {
        row[i] = 0xff;
    }
}

void populateRowNone(unsigned char *const row, const int rowLen) {
    for (int i = 0; i < rowLen; i += 1) {
        row[i] = 0;
    }
}

void populateRowRandom(unsigned char *const row, const int rowLen) {
    for (int i = 0; i < rowLen; i += 1) {
        row[i] = rand() % 256;
    }
}

// Populate row according to the config
void populateRow(unsigned char *const row, const int rowLen, const struct Config &config) {
    switch (config.initialState) {
        case InitialState_CENTER_BIT:
            populateRowCenterBit(row, rowLen);
            break;
        case InitialState_CENTER_BYTE:
            populateRowCenterByte(row, rowLen);
            break;
        case InitialState_EVEN:
            populateRowEven(row, rowLen);
            break;
        case InitialState_ODD:
            populateRowOdd(row, rowLen);
            break;
        case InitialState_EVERY_XTH:
            populateRowEveryXth(row, rowLen, config.xth);
            break;
        case InitialState_ALL:
            populateRowAll(row, rowLen);
            break;
        case InitialState_NONE:
            populateRowNone(row, rowLen);
            break;
        case InitialState_RANDOM:
            populateRowRandom(row, rowLen);
            break;
        default:
            std::cerr << __func__ << ": Unknown initialState value. Exiting.\n";
            exit(1);
    }
}

void updateEca(const struct Config &config) {
    // Get rid of old image if it exists
    Fl_Image *const oldImage = box->image();
    if (oldImage != NULL) {
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
    populateRow(buf, rowLen, config);

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

void ruleChangeCallback(Fl_Widget *w, void *data) {
    Fl_Hor_Value_Slider *const slider = dynamic_cast<Fl_Hor_Value_Slider*>(w);

    if (slider) {
        const int val = std::floor(slider->value());
        config.rule = val;
        updateEca(config);
    }
}

void xthChangeCallback(Fl_Widget *w, void *data) {
    Fl_Hor_Value_Slider *const slider = dynamic_cast<Fl_Hor_Value_Slider*>(w);

    if (slider) {
        const int val = std::floor(slider->value());
        config.xth = val;
        updateEca(config);
    }
}

int main(int argc, char **argv) {
    // Layout constants
    const int initialWindowWidth = 1000;
    const int initialWindowHeight = 600;
    const int rightColumnWidth = 200;
    const int radioButtonHeight = 30;
    const int headerHeight = 30;
    // const int margin = 15;
    const int imageAreaWidth = initialWindowWidth - rightColumnWidth;
    const double sliderLabelSizeMult = 0.9;
    const Fl_Boxtype headerBoxStyle = FL_PLASTIC_UP_BOX;
    const char *const windowTitle = "Elementary Cellular Automata Viewer";

    window = new Fl_Window(initialWindowWidth, initialWindowHeight, windowTitle);
    Fl_Pack *const windowPack = new Fl_Pack(0, 0, initialWindowWidth, initialWindowHeight);
    windowPack->type(Fl_Pack::HORIZONTAL);
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
    // box->labeltype(_FL_IMAGE_LABEL);// Why does this cause the image not to show?
    windowPack->resizable(box);

    Fl_Pack *const rightPack = new Fl_Pack(0, 0, rightColumnWidth, initialWindowHeight);
    // This is unnecessary/redundant
    //  except that it gets rid of the warning that rightPack variable is unused
    rightPack->type(Fl_Pack::VERTICAL);

    Fl_Box *const initialStateBox = new Fl_Box(0, 0, 0, headerHeight, "Initial State");
    initialStateBox->box(headerBoxStyle);

    // Each set of radio buttons has its own pack because different sets
    //  of radio buttons need to be in different groups
    //  in order to work like radio buttons (only one selected at a time)
    Fl_Pack *const initialStateRadioPack = new Fl_Pack(0, 0, rightColumnWidth, 0);
    Fl_Radio_Round_Button *const initialCenterBitButton =  new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Center bit");
    Fl_Radio_Round_Button *const initialCenterByteButton = new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Center byte");
    Fl_Radio_Round_Button *const initialEvenButton =       new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Even");
    Fl_Radio_Round_Button *const initialOddButton =        new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Odd");
    Fl_Radio_Round_Button *const initialEveryXthButton =   new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Every Xth byte");
    Fl_Radio_Round_Button *const initialAllButton =        new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "All");
    Fl_Radio_Round_Button *const initialNoneButton =       new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "None");
    Fl_Radio_Round_Button *const initialRandomButton =     new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Random");
    initialStateRadioPack->end();

    // Changes nothing
    // initialCenterBitButton->color(fl_rgb_color(255, 255, 0));

    Fl_Box *const colorBox = new Fl_Box(0, 0, 0, headerHeight, "Color");
    colorBox->box(headerBoxStyle);

    Fl_Pack *const colorRadioPack = new Fl_Pack(0, 0, 0, 0);
    Fl_Radio_Round_Button *const rgbButton       = new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "RGB 888");
    Fl_Radio_Round_Button *const grayscaleButton = new Fl_Radio_Round_Button(0, 0, 0, radioButtonHeight, "Grayscale 8");
    colorRadioPack->end();

    Fl_Box *const optionsBox = new Fl_Box(0, 0, 0, headerHeight, "Options");
    optionsBox->box(headerBoxStyle);

    Fl_Check_Button *const wrapButton = new Fl_Check_Button(0, 0, 0, radioButtonHeight, "Wrap");

    Fl_Hor_Value_Slider *const ruleSlider = new Fl_Hor_Value_Slider(0, 0, 0, radioButtonHeight, "Rule");
    ruleSlider->bounds(0, 255);
    ruleSlider->step(1);
    ruleSlider->labelsize(ruleSlider->labelsize() * sliderLabelSizeMult);

    // Dumb and easy way to put space between the sliders
    // Necessary because FLTK does not allot space for the slider label (why is that a good decision, FLTK?)
    Fl_Box *const sliderSpacer = new Fl_Box(0, 0, 0, 20);
    sliderSpacer->box(FL_NO_BOX);

    Fl_Hor_Value_Slider *const xthSlider = new Fl_Hor_Value_Slider(0, 0, 0, radioButtonHeight, "X");
    xthSlider->bounds(1, 999);
    xthSlider->step(1);
    xthSlider->labelsize(ruleSlider->labelsize() * sliderLabelSizeMult);

    // Initialize config and UI
    {
        config.initialState = InitialState_RANDOM;
        initialRandomButton->setonly();

        config.colorMode = ColorMode_RGB888;
        rgbButton->setonly();

        config.wrap = true;
        wrapButton->set();

        std::srand(std::time(NULL));
        config.rule = std::rand() % 256;
        ruleSlider->value(config.rule);

        config.xth = 80;
        xthSlider->value(config.xth);

        // The initial run
        updateEca(config);
    }

    // Add change callbacks to widgets
    {
        // Casting enum value to void ptr is not ideal but it works
        // The radio button callbacks are only called when the button is set (never when unset)
        initialCenterBitButton->callback( initialStateChangeCallback, (void*)InitialState_CENTER_BIT);
        initialCenterByteButton->callback(initialStateChangeCallback, (void*)InitialState_CENTER_BYTE);
        initialEvenButton->callback(      initialStateChangeCallback, (void*)InitialState_EVEN);
        initialOddButton->callback(       initialStateChangeCallback, (void*)InitialState_ODD);
        initialEveryXthButton->callback(  initialStateChangeCallback, (void*)InitialState_EVERY_XTH);
        initialAllButton->callback(       initialStateChangeCallback, (void*)InitialState_ALL);
        initialNoneButton->callback(      initialStateChangeCallback, (void*)InitialState_NONE);
        initialRandomButton->callback(    initialStateChangeCallback, (void*)InitialState_RANDOM);

        rgbButton->callback(      colorModeChangeCallback, (void*)ColorMode_RGB888);
        grayscaleButton->callback(colorModeChangeCallback, (void*)ColorMode_GRAYSCALE8);

        // Checkbox callback gets triggered for both set and unset
        wrapButton->callback(wrapChangeCallback, NULL);
        ruleSlider->callback(ruleChangeCallback, NULL);
        xthSlider->callback(xthChangeCallback, NULL);
    }

    window->end();
    window->show(argc, argv);
    return Fl::run();
}
