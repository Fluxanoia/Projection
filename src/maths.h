#ifndef MATHS_H_
#define MATHS_H_

#include <stdbool.h>

// Defines PI
#define PI acos(-1.0)

//
//      STRUCTS
//

// Defines a 2-tuple
typedef struct Tuple2_ {
    double a;
    double b;
} Tuple2;

// Defines a 3-tuple
typedef struct Tuple3_ {
    double a;
    double b;
    double c;
} Tuple3;

// Defines a point
typedef struct Point_ {
    double x;
    double y;
    double z;
} Point;

// Defines a polar form vector
typedef struct PolarVector_ {
    double r;
    double theta;
    double phi;
} PolarVector;

// Defines a vector struct
typedef struct Vector_ {
    double x;
    double y;
    double z;
} Vector;

// Defines a line struct
typedef struct Line_ {
    Vector* v;
    Point* p;
} Line;

// Defines a plane struct
typedef struct Plane_ {
    double x_coeff;
    double y_coeff;
    double z_coeff;
    double constant;
} Plane;

//
//      FUNCTIONS
//

void readEquation(unsigned long size, char* eq);

double norm(Vector* v);

void tuple2Multiply(Tuple2* t, double p);
void vectorSum(Vector* o, Vector* v, Vector* u);
void vectorMultiply(Vector* v, double p);

void normalPlane(Plane* p, Vector* v);
void normalVector(Vector* v, Plane* p);

void vectorFromPolar(Vector* v, PolarVector* pv);
void polarVectorFromCart(PolarVector* pv, Vector* v);

void reduceToUnit(Vector* v);

void crossVector(Vector* output, Vector* v, Vector* w);
void crossPlane(Plane* output, Vector* v);

void zeroPlane(Plane* p);
void zeroVector(Vector* v);

void getOrthonormals(Plane* p, Vector* u, Vector* w);

bool directionTo(Tuple3* o, Vector* v, Vector* u, Vector* w, Point* p);

void printPlane(Plane* p);
void printVector(Vector* v);
void printPolarVector(PolarVector* pv);

#endif
