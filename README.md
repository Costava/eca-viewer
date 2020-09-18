Elementary Cellular Automata Viewer
===================================

View [elementary cellular automata](https://en.wikipedia.org/wiki/Elementary_cellular_automaton) in a GUI window with UI for parameters and tuning.

There are two rendering options:
- `RGB 888`: Each pixel is 24 bits (8 bits for each of the red, green, and blue components) where each bit is a cell. The least significant bit of the red component of the first (leftmost) pixel is the first cell.
- `Grayscale 8`: Each pixel is an 8-bit grayscale value.

The first row of pixels is the initial state. Each row below is the next iteration of the row above it.

Dependencies:
- C++98
- [Fast Light Toolkit (FLTK)](https://www.fltk.org/) v1.3.5

Feel free to open an issue to ask a question.

How to
------

Install [FLTK](https://www.fltk.org/) through your package manager e.g. `pamac install fltk`

Then, use the `makefile`.

`make run`  
Run/launch the program.
This will build if necessary or if source files have been updated since the last build.

`make build`  
To only build.

`make clean`  
Delete any built files.

The built executable can also be run as usual: `./main`

License
-------

GNU General Public License Version 3. See file `LICENSE`.
