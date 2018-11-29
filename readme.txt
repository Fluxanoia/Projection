A 3D graphing tool in C with SDL2

Usage:
    Use the mouse to rotate the graph and scroll to zoom

Text input:
    Acts how you think it would on the whole, hold left shift when
    backspacing to remove the whole line, or when moving the caret
    to skip to the front/end of the line.

Command line arguments:
    -test : runs tests, mainly of the math functions

Commands:
    plane a b c d    : draws the plane ax + by + cz = d
    point a b c      : draws the point (a, b, c)
    line a b c d e f : draws the line starting at (a, b, c) going in the
                       direction of the vector (d, e, f)

Using:
    The NK57 Monospace font under a desktop license

Required libraries: SDL2, SDL_ttf, math.h
