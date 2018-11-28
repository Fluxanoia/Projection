#ifndef MAIN_H_
#define MAIN_H_

//
//      DEFINTIONS
//

// The program title
#define TITLE         "Projections"
// The width of the window in pixels
#define WINDOW_WIDTH     640
#define WINDOW_WIDTH_MID (WINDOW_WIDTH >> 1)
// The height of the window in pixels
#define WINDOW_HEIGHT     480
#define WINDOW_HEIGHT_MID (WINDOW_HEIGHT >> 1)
// The length from the origin to the end of the axis
#define AXIS_LENGTH 100
// The size of the axis arrow
#define AXIS_ARROW_SIZE (AXIS_LENGTH / 20)
// How large the planes are rendered with respect to the axis length
#define PLANE_SCALE 0.75
// A number inversely proportional to how many lines appear inside a plane
#define PLANE_LINES_SCALE 10.0
// Defines how pixel movement of the mouse relates to rotation
#define MOUSE_DRAG_FACTOR (1.0 / 200.0)


// Defines the games updates per second
#define UPDATES_PER_SECOND 50
// Calculates the time inbetween updates in milliseconds
#define UPDATE_DELTA_TIME  (1000 / UPDATES_PER_SECOND)

// Defines the RGB values for the background
#define BG_R 40
#define BG_G 40
#define BG_B 40

#endif
