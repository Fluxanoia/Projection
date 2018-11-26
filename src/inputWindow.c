#include         <SDL.h>
#include      <string.h>
#include     <stdbool.h>
#include     <SDL_ttf.h>
#include        "main.h"
#include "inputWindow.h"

InputWindow *initialiseInputWindow(SDL_Point p) {
    InputWindow* iw = malloc(sizeof(InputWindow));
    iw->window = SDL_CreateWindow(
            "Input Window",
            p.x - INPUTWIN_WIDTH - 20,
            p.y, 
            INPUTWIN_WIDTH,
            INPUTWIN_ITEM_HEIGHT * MAX_ITEMS, 
            0);

    iw->renderer = SDL_CreateRenderer(iw->window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(iw->renderer, SDL_BLENDMODE_BLEND);

    iw->ticks = 0;
    iw->redraw = true;
    iw->component_redraw = true;

    iw->font = TTF_OpenFont("res/nk57.ttf", 24);

    iw->caret_index = 0;
    iw->caret_location = 0;
    iw->caret_show = true;
    for (int i = 0; i < MAX_ITEMS; i++) iw->input[i][0] = '\0';
    
    return iw;
}

void collapseCaret(InputWindow* iw) {
    while (iw->caret_location > strlen(iw->input[iw->caret_index])) {
        iw->caret_location--;
    }
}

bool isDigit(int i) {
    if (strlen(SDL_GetKeyName(i)) != 1) return false;
    return (SDL_GetKeyName(i)[0] - '0') <= ('9' - '0');
}

bool isChar(int c) {
    if (strncmp(SDL_GetKeyName(c), "Space", 5) == 0) return true;
    if (strlen(SDL_GetKeyName(c)) != 1) return false;
    return (tolower(SDL_GetKeyName(c)[0]) - 'a') <= ('z' - 'a');
}

// React to mouse motion
void InputWindow_mouseMotionEvent(InputWindow* iw, SDL_MouseMotionEvent e) {
}

// React to mouse button presses
void InputWindow_mouseButtonEvent(InputWindow* iw, SDL_MouseButtonEvent e) {
}

// React to key presses
void InputWindow_keyEvent(InputWindow* iw, SDL_KeyboardEvent e) {
    if (e.type == SDL_KEYDOWN) {
        if (isChar(e.keysym.sym) || isDigit(e.keysym.sym)) {
            if (strlen(iw->input[iw->caret_index]) < MAX_EQUATION_LEN - 1) {
                iw->input[iw->caret_index][strlen(iw->input[iw->caret_index]) + 1] = '\0';
                for (int i = strlen(iw->input[iw->caret_index]); i > iw->caret_location; i--) {
                    iw->input[iw->caret_index][i] = iw->input[iw->caret_index][i - 1];
                }
                iw->input[iw->caret_index][iw->caret_location] = e.keysym.sym;
                iw->caret_location++;
                iw->component_redraw = true;
            }
        }
    }
    switch (e.keysym.sym) {
        case SDLK_LEFT:
            if (e.type == SDL_KEYDOWN) {
                if (iw->caret_location > 0) {
                    collapseCaret(iw);
                    iw->caret_location--;
                    iw->redraw = true;
                }
            }  
        break;
        case SDLK_RIGHT:
            if (e.type == SDL_KEYDOWN) {
                if (iw->caret_location < strlen(iw->input[iw->caret_index])) {
                    iw->caret_location++;
                    iw->redraw = true;
                }
            }
        break;
        case SDLK_UP:
            if (e.type == SDL_KEYDOWN) {
                if (iw->caret_index > 0) {
                    iw->caret_index--;
                    collapseCaret(iw);
                    iw->redraw = true;
                }
            }
        break;
        case SDLK_DOWN:
        case SDLK_RETURN:
            if (e.type == SDL_KEYDOWN) {
                if (iw->caret_index < MAX_ITEMS - 1) {
                    iw->caret_index++;
                    collapseCaret(iw);
                    iw->redraw = true;
                }
            }
        break;
        case SDLK_BACKSPACE:
            if (iw->caret_location == 0) break;
            if (e.type == SDL_KEYDOWN) {
                int new_len = strlen(iw->input[iw->caret_index]);
                char sub[new_len];
                for (int i = 0; i < new_len; i++) {
                    if (i == iw->caret_location - 1) continue;
                    if (i < iw->caret_location - 1) {
                        sub[i] = iw->input[iw->caret_index][i];
                    } else {
                        sub[i - 1] = iw->input[iw->caret_index][i];
                    }
                }
                sub[new_len - 1] = '\0';
                strcpy(iw->input[iw->caret_index], sub);
                iw->caret_location--;
                iw->component_redraw = true;
            }
        break;
    }
}

SDL_Texture* createText(char* title, int* w, int* h, SDL_Color c, InputWindow* iw) {
    SDL_Surface* surface = TTF_RenderText_Solid(iw->font, title, c);
    SDL_Texture* t = SDL_CreateTextureFromSurface(iw->renderer, surface);
    SDL_FreeSurface(surface);
    SDL_QueryTexture(t, NULL, NULL, w, h);
    return t;
}

int caretLocation(char* title, int caret_location, InputWindow* iw) {
    if (caret_location == 0) return 0;
    if (caret_location > strlen(title)) caret_location = strlen(title);
    int loc;
    char sub[caret_location + 1];
    for (int i = 0; i < caret_location; i++) sub[i] = title[i];
    sub[caret_location] = '\0';
    SDL_Texture* t = createText(sub, &loc, NULL, (SDL_Color) { 0, 0, 0, 0 }, iw);
    SDL_DestroyTexture(t);
    return loc;
}

void drawInputWindow(InputWindow* iw) {
    SDL_RenderClear(iw->renderer);

    SDL_SetRenderDrawColor(iw->renderer, BG_R, BG_G, BG_B, 255);
    SDL_RenderFillRect(iw->renderer, NULL);

    int w, h;
    int loc;
    int caret_buffer = (INPUTWIN_ITEM_HEIGHT - CARET_HEIGHT) / 2;
    SDL_Texture* text;
    SDL_Color c = { 255, 255, 255, 255 };
    SDL_Rect srcrect = { 0, 0, 0, 0 };
    SDL_Rect dstrect = { ITEM_BUFFER, 0, INPUTWIN_WIDTH, 0 };
    SDL_SetRenderDrawColor(iw->renderer, 255, 255, 255, 128);
    for (int i = 0; i < MAX_ITEMS; i++) {
        
        dstrect.y = i * INPUTWIN_ITEM_HEIGHT;
        if (iw->caret_index == i && iw->caret_show) {
            loc = caretLocation(iw->input[i], iw->caret_location, iw);
            SDL_RenderDrawLine(iw->renderer,
                    dstrect.x + loc,
                    dstrect.y + caret_buffer,
                    dstrect.x + loc,
                    dstrect.y + caret_buffer + CARET_HEIGHT);
        }

        if (strncmp(iw->input[i], "", 1) == 0) continue;
        text = createText(iw->input[i], &w, &h, c, iw);
        if (w < INPUTWIN_WIDTH) { 
            srcrect.w = w;
        } else { srcrect.w = INPUTWIN_WIDTH; }
        dstrect.w = srcrect.w;
        srcrect.h = h;
        dstrect.h = h;
        dstrect.y += (INPUTWIN_ITEM_HEIGHT - h) / 2;
        SDL_RenderCopy(iw->renderer, text, &srcrect, &dstrect);
        SDL_DestroyTexture(text);
    }

    int line_end_x = INPUTWIN_WIDTH - SEP_BUFFER;
    for (int i = 1; i < MAX_ITEMS; i++) {
        SDL_RenderDrawLine(iw->renderer, SEP_BUFFER, i * INPUTWIN_ITEM_HEIGHT,
                line_end_x, i * INPUTWIN_ITEM_HEIGHT);
    }

    SDL_RenderPresent(iw->renderer);
}

void updateInputWindow(InputWindow* iw) {
    iw->ticks++;
    if (iw->ticks % 25 == 0) {
        iw->redraw = true;
        iw->caret_show = !iw->caret_show;
    }
}

bool InputWindow_dropRedraw(InputWindow* iw) {
    if (iw->component_redraw) {
        iw->component_redraw = false;
        return true;
    }
    return false;
}

void freeInputWindow(InputWindow* iw) {
    TTF_CloseFont(iw->font);
    SDL_DestroyRenderer(iw->renderer);
    SDL_DestroyWindow(iw->window);
    free(iw);
}
