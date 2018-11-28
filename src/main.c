#include         <SDL.h>
#include       <stdio.h>
#include     <stdbool.h>
#include     <SDL_ttf.h>
#include        "main.h"
#include       "maths.h"
#include      "memory.h"
#include "inputWindow.h"

//
//      GLOBAL VARIABLES
//

// Tracks whether the program should be running or not
bool running = false;
// Whether the program should redraw
bool redraw = true;

// The pointer to the program window
SDL_Window* window = NULL;
// The renderer for graphics
SDL_Renderer* renderer = NULL;
// The window where the user can input information
InputWindow* inputWindow = NULL;
// The vector pointing to the view and the axes
PolarVector* view = NULL;

// The array of colours for graph items
SDL_Color colors[MAX_ITEMS];

// Whether the left mouse button is held down
bool leftMouseDown;
// The last examined x, y of the mouse with the LMB down
int leftMouseDown_x, leftMouseDown_y;

//
//      HELPER FUNCTIONS
// 

// Checks if the window has focus for the keyboard
bool isKeyFocused(SDL_Window* w) {
    return (SDL_GetWindowFlags(w) & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS;
}

// Checks if the window has focus for the mouse
bool isMouseFocused(SDL_Window* w) {
    return (SDL_GetWindowFlags(w) & SDL_WINDOW_MOUSE_FOCUS) == SDL_WINDOW_MOUSE_FOCUS;
}

//
//      RENDERING FUNCTIONS
//

// Gets the projected x, y screen coordinates of a 3D point
void coordinatesTo(Tuple2* t, Point* p) {
    Vector v, u, w;
    Plane viewPlane;
    Tuple3 coeffs;

    // Gets the cartesian vector from the view point
    vectorFromPolar(&v, view);
    // Gets the plane normal to the view vector and moves it to the end
    // of the vector
    normalPlane(&viewPlane, &v);
    viewPlane.constant = view->r;

    // Gets the orthonormals of the plane
    // one of which is in the plane spanned by the z axis and the view
    getOrthonormals(&viewPlane, &u, &w);

    // Find out how to use said vectors to reach the point
    directionTo(&coeffs, &v, &u, &w, p);

    // Put the coefficients into the tuple
    t->a = coeffs.c;
    t->b = -coeffs.b;
}

//
//      INPUT FUNCTIONS
//

// React to mouse motion
void mouseMotionEvent(SDL_MouseMotionEvent e) {
    // If the LMB is down
    if (leftMouseDown) {
        int dx = leftMouseDown_x - e.x;
        int dy = leftMouseDown_y - e.y;
        // Change the view angle based on the change in x and y
        view->phi += (double) dx * MOUSE_DRAG_FACTOR;
        view->theta += (double) dy * MOUSE_DRAG_FACTOR;
        // Adjust theta as integrals of PI cause problems
        if (view->theta < 0) view->theta = 0.001;
        if (view->theta > PI) view->theta = PI - 0.001;        
        // Tell the component to redraw
        redraw = true;
        // Update the last examined x, y
        leftMouseDown_x = e.x;
        leftMouseDown_y = e.y;
    }
}

// React to mouse button presses
void mouseButtonEvent(SDL_MouseButtonEvent e) {
    // Check when the LMB is down and update the x, y of it
    if (e.type == SDL_MOUSEBUTTONUP && e.button == SDL_BUTTON_LEFT) {
        leftMouseDown = false;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button == SDL_BUTTON_LEFT) {
        leftMouseDown = true;
        leftMouseDown_x = e.x;
        leftMouseDown_y = e.y;
    }
}

void mouseWheelEvent(SDL_MouseWheelEvent e) {
    // Change the zoom based on how much the mouse scrolls
    view->r += (double) e.y * -0.01;
    if (view->r < 0.01) view->r = 0.01;
    redraw = true;
}

// React to key presses
void keyEvent(SDL_KeyboardEvent e) {
}

//
//      DRAWING FUNCTIONS
//

// Draws a point
void drawPoint(Point* p) {
    // Get the screen coords of the point
    Tuple2 coords;
    coordinatesTo(&coords, p);
    // Scale the point for the zoom level
    tuple2Multiply(&coords, 1 / view->r);
    // Get the points x and y values
    int c_a = WINDOW_WIDTH_MID + coords.a;
    int c_b = WINDOW_HEIGHT_MID + coords.b;
    // Draw an X centred at the point
    SDL_RenderDrawLine(renderer, c_a - 5, c_b - 5,
                                 c_a + 5, c_b + 5);
    SDL_RenderDrawLine(renderer, c_a + 5, c_b - 5,
                                 c_a - 5, c_b + 5);
}

// Draws a line
void drawLine(Line* line) {
    Point v;
    Tuple2 t_p, t_v;
    // Get the vector of the line into a point
    v.x = line->v->x;
    v.y = line->v->y;
    v.z = line->v->z; 
    // Gets the screen coordinates of the vector and the point
    coordinatesTo(&t_p, line->p);
    coordinatesTo(&t_v, &v);
    // Scales the points by the zoom level
    tuple2Multiply(&t_p, 1 / view->r);
    tuple2Multiply(&t_v, 1 / view->r);
    // Render the line from the point to the point + vector
    SDL_RenderDrawLine(renderer, WINDOW_WIDTH_MID + t_p.a, WINDOW_HEIGHT_MID + t_p.b,
                                 WINDOW_WIDTH_MID + t_p.a + t_v.a, WINDOW_HEIGHT_MID + t_p.b + t_v.b);
}

// Draws a plane
void drawPlane(Plane* p) {
    // If our coefficients are zero, return
    if (p->x_coeff == 0 && p->y_coeff == 0 && p->z_coeff == 0) return;
    // Get the normal verctor to the plane and make sure it extends
    // such that it just touches the plane
    Vector norm;
    normalVector(&norm, p);
    reduceToUnit(&norm);
    vectorMultiply(&norm, p->constant);

    Line l;
    Vector vector;
    Point point;
    Vector e[4];
    Vector v1, v2;
    // Get the orthonormals to the plane
    getOrthonormals(p, &v1, &v2);
    // Multiply them so they are as large as we want the plane
    vectorMultiply(&v1, PLANE_SCALE * AXIS_LENGTH);
    vectorMultiply(&v2, PLANE_SCALE * AXIS_LENGTH);
    // Get the four corners of the plane as points
    vectorSum(&e[0], &v1, &v2);
    vectorMultiply(&v2, -1);
    vectorSum(&e[1], &v1, &v2);
    vectorMultiply(&v1, -1);
    vectorSum(&e[2], &v1, &v2);
    vectorMultiply(&v2, -1);
    vectorSum(&e[3], &v1, &v2);
    // Draw the line between the points
    for (int i = 1; i < 4; i++) {
        point = (Point) { e[i - 1].x + norm.x, e[i - 1].y + norm.y, e[i - 1].z + norm.z };
        vector = (Vector) { e[i].x - e[i - 1].x, e[i].y - e[i - 1].y, e[i].z - e[i - 1].z };
        l.p = &point;
        l.v = &vector;
        drawLine(&l);
    }
    point = (Point) { e[3].x + norm.x, e[3].y + norm.y, e[3].z + norm.z };
    vector = (Vector) { e[0].x - e[3].x, e[0].y - e[3].y, e[0].z - e[3].z };
    l.p = &point;
    l.v = &vector;
    drawLine(&l);
    // Get the orthonormals again and resize appropriately
    getOrthonormals(p, &v1, &v2);
    vectorMultiply(&v1, PLANE_LINES_SCALE);
    vectorMultiply(&v2, PLANE_LINES_SCALE);
    // Get the points we want to draw between
    Point p1 = (Point) { e[3].x + norm.x, e[3].y + norm.y, e[3].z + norm.z };
    Point p2 = (Point) { e[3].x + norm.x, e[3].y + norm.y, e[3].z + norm.z };
    // Get the vectors of the lines we want to draw
    Vector u1 = (Vector) { e[0].x - e[3].x, e[0].y - e[3].y, e[0].z - e[3].z };
    Vector u2 = (Vector) { e[2].x - e[3].x, e[2].y - e[3].y, e[2].z - e[3].z };
    for (int i = 1; i < 2 * (PLANE_SCALE * AXIS_LENGTH) / PLANE_LINES_SCALE; i++) {
        // Move the points along
        p1.x += v1.x;
        p1.y += v1.y;
        p1.z += v1.z;
        p2.x -= v2.x;
        p2.y -= v2.y;
        p2.z -= v2.z;
        // Draw both lines
        l.p = &p1;
        l.v = &u2;
        drawLine(&l);
        l.p = &p2;
        l.v = &u1;
        drawLine(&l);
    }
}

// Draws an axis
void drawAxis(int x, int y, int z) {
    Line l;
    // Gets the vector to the end of the axis
    Vector l_v = { AXIS_LENGTH * x, AXIS_LENGTH * y, AXIS_LENGTH * z };
    Point l_p = { 0, 0, 0 };
    l.v = &l_v;
    l.p = &l_p;
    // Draw the main axis line
    drawLine(&l);
    // Sets up variables for future calculation
    int mod_x = 0, mod_y = 0, mod_z = 0;
    if (x == 0) {
        mod_x = 1;
    } else if (y == 0) {
        mod_y = 1;
    } else {
        mod_z = 1;
    }
    // Draw the axis arrows
    l_p.x = l_v.x;
    l_p.y = l_v.y;
    l_p.z = l_v.z;
    l_v.x = AXIS_ARROW_SIZE * (mod_x - x);
    l_v.y = AXIS_ARROW_SIZE * (mod_y - y);
    l_v.z = AXIS_ARROW_SIZE * (mod_z - z);
    drawLine(&l);
    l_v.x = AXIS_ARROW_SIZE * (-mod_x - x);
    l_v.y = AXIS_ARROW_SIZE * (-mod_y - y);
    l_v.z = AXIS_ARROW_SIZE * (-mod_z - z);
    drawLine(&l);

    Tuple2 coords;
    Point p = { (AXIS_LENGTH + 20) * x, (AXIS_LENGTH + 20) * y, (AXIS_LENGTH + 20) * z };
    coordinatesTo(&coords, &p);
    tuple2Multiply(&coords, 1 / view->r);

    int c_a = WINDOW_WIDTH_MID + coords.a;
    int c_b = WINDOW_HEIGHT_MID + coords.b;
    if (x > 0 && y == 0 && z == 0) {
        SDL_RenderDrawLine(renderer, c_a - 5, c_b - 5,
                                     c_a + 5, c_b + 5);
        SDL_RenderDrawLine(renderer, c_a + 5, c_b - 5,
                                     c_a - 5, c_b + 5);
    }
    if (x == 0 && y > 0 && z == 0) {
        SDL_RenderDrawLine(renderer, c_a - 5, c_b - 5,
                                     c_a, c_b);
        SDL_RenderDrawLine(renderer, c_a + 5, c_b - 5,
                                     c_a - 5, c_b + 5);
    }
    if (x == 0 && y == 0 && z > 0) {
        SDL_RenderDrawLine(renderer, c_a - 5, c_b - 5,
                                     c_a + 5, c_b - 5);
        SDL_RenderDrawLine(renderer, c_a + 5, c_b - 5,
                                     c_a - 5, c_b + 5);
        SDL_RenderDrawLine(renderer, c_a - 5, c_b + 5,
                                     c_a + 5, c_b + 5);    
    }
}

// Draws to the window
void draw() {
    // Attempt to draw the input window (it handles its own redraws)
    drawInputWindow(inputWindow);
    // Check if we can draw
    if (!redraw) return;
    redraw = false;
    // Clear the renderer
    SDL_RenderClear(renderer);
    // Draws the background
    SDL_SetRenderDrawColor(renderer, BG_R, BG_G, BG_B, 255);
    SDL_RenderFillRect(renderer, NULL);
    
    // Start drawing the graph
    // Draw the axes
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawAxis(1, 0, 0);
    drawAxis(0, 1, 0);
    drawAxis(0, 0, 1);
    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    drawAxis(-1, 0, 0);
    drawAxis(0, -1, 0);
    drawAxis(0, 0, -1);
    // Sift through the input for anything worthwhile
    char s[MAX_EQUATION_LEN];
    float a, b, c, d;
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strncmp(inputWindow->input[i], "", 1) == 0) continue;
        sscanf(inputWindow->input[i], "%s %f %f %f %f", s, &a, &b, &c, &d);
        SDL_SetRenderDrawColor(renderer, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        if (strncmp("plane", s, 5) == 0) {
            Plane p = { a, b, c, d };
            drawPlane(&p);
        }
        if (strncmp("point", s, 5) == 0) {
            Point p = { a, b, c };
            drawPoint(&p);
        }
    }

    // ----------------

    SDL_RenderPresent(renderer);
}

//
//      MAIN FUNCTIONS
//

// Updates components
void update() {
    updateInputWindow(inputWindow);
    redraw |= InputWindow_dropRedraw(inputWindow);

    while (view->phi > 2 * PI) {
        view->phi -= 2 * PI;
    }
    while (view->phi < -2 * PI) {
        view->phi += 2 * PI;
    }
}

// Deals with a window event 
void windowEvent(SDL_Event* e) {
    switch(e->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            e->type = SDL_QUIT;
            SDL_PushEvent(e);
        break;
        case SDL_WINDOWEVENT_LEAVE:
            if (e->window.windowID == SDL_GetWindowID(window)) leftMouseDown = false;
        break;
    }
}

// Poll for SDL events
void pollEvents() {
    // If there's no events, return
    if (SDL_PollEvent(NULL) == 0) return;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch(e.type) {
            // Quitting the game (on exit, ALT+F4, etc.)
            case SDL_QUIT:
                running = false;
            return;
            // When a key is pressed or released
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                if (isKeyFocused(window)) keyEvent(e.key);
                if (isKeyFocused(inputWindow->window)) {
                    InputWindow_keyEvent(inputWindow, e.key);
                }
            break;
            // When mouse motion occurs
            case SDL_MOUSEMOTION:
                if (isMouseFocused(window)) mouseMotionEvent(e.motion);
                if (isMouseFocused(inputWindow->window)) {
                    InputWindow_mouseMotionEvent(inputWindow, e.motion);
                }
            break;
            // When the mouse buttons are pressed
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                if (isMouseFocused(window)) mouseButtonEvent(e.button);
                if (isMouseFocused(inputWindow->window)) {
                    InputWindow_mouseButtonEvent(inputWindow, e.button);
                }
            break;
            // When the mouse wheel is moved
            case SDL_MOUSEWHEEL:
                if (isMouseFocused(window)) mouseWheelEvent(e.wheel);
            break;
            // When a window event occurs
            case SDL_WINDOWEVENT:
                windowEvent(&e);
            break;
        }
    }
}

// Runs the main program loop
void run() {
    // Allow the program to run
    running = true;
    // Keep track of when the last update was
    unsigned int lastUpdate = SDL_GetTicks();
    while (running) {
        // Poll for events
        pollEvents();
        // Update and draw as many times as necessary
        while (SDL_GetTicks() - lastUpdate >= UPDATE_DELTA_TIME) {
            lastUpdate += UPDATE_DELTA_TIME;
            update();
            draw();
        }
    }
}

void test() {
}

void init() {
    // Initialise important variables
    view = initPolarVector(0.5, PI / 4, PI / 4);
    for (int i = 0; i < MAX_ITEMS; i++) {
        SDL_Color c;
        c.r = 120 + 120 * (i & 0x1);
        c.g = 160 + 80 * ((i >> 1) & 0x1);
        c.b = 200 + 40 * ((i >> 2) & 0x1);
        c.a = 255;
        colors[i] = c;
    }
}

// Frees memory
int free_() {
    printf("Freeing memory...\n");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    freePolarVector(view);
    freeInputWindow(inputWindow);

    printf("Quitting SDL...\n");
    TTF_Quit();
    SDL_Quit();

    printf("Exiting...\n\n");
    return 0;
}

// The entry point for the program
int main(int n, char const *args[]) {
    printf("\n%s, starting up...\n", TITLE);
    
    // Read in the command line arguments
    bool run_tests = false;
    for (int i = 1; i < n; i++) {
        if (strncmp(args[i], "-test", 5) == 0) run_tests = true;
    }

    printf("Initialising SDL...\n");
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    printf("Creating a window...\n");
    window = SDL_CreateWindow(
            TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, 
            WINDOW_WIDTH,
            WINDOW_HEIGHT, 
            0);

    printf("Creating the renderer from the window...\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    printf("Creating the input window...\n");
    int wx, wy;
    SDL_GetWindowPosition(window, &wx, &wy);
    inputWindow = initialiseInputWindow((SDL_Point) { wx, wy });

    init();
    if (run_tests) {
        printf("Start up complete, running the tests...\n");      
        test();  
        printf("\n --- ALL TESTS PASSED --- \n\n");
    } else {
        printf("Start up complete, running the program...\n");
        run();
    }

    return free_();
}
