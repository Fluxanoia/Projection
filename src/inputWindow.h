#ifndef INPUTWIN_H_
#define INPUTWIN_H_

#define INPUTWIN_WIDTH       320
#define INPUTWIN_ITEM_HEIGHT 40

#define MAX_ITEMS            16
#define MAX_EQUATION_LEN     512

#define CARET_HEIGHT         24
#define ITEM_BUFFER          10
#define SEP_BUFFER           25

typedef struct InputWindow_ {

    unsigned long ticks;
    bool redraw;
    bool component_redraw;

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    int caret_index;
    int caret_location;
    bool caret_show;
    char input[MAX_ITEMS][MAX_EQUATION_LEN];

} InputWindow;

InputWindow* initialiseInputWindow(SDL_Point p);
void InputWindow_mouseMotionEvent(InputWindow* iw, SDL_MouseMotionEvent e);
void InputWindow_mouseButtonEvent(InputWindow* iw, SDL_MouseButtonEvent e);
void InputWindow_keyEvent(InputWindow* iw, SDL_KeyboardEvent e);
void drawInputWindow(InputWindow* iw);
void updateInputWindow(InputWindow* iw);
bool InputWindow_dropRedraw(InputWindow* iw);
void freeInputWindow(InputWindow* iw);

#endif
