#include <stdlib.h>
#include  "maths.h"
#include "memory.h"

//
//      FUNCTIONS
//

// Creates a Tuple2 and returns its pointer
Tuple2* initTuple2(double a, double b) {
    Tuple2* t = malloc(sizeof(Tuple2));
    t->a = a;
    t->b = b;
    return t;
}

// Creates a Tuple3 and returns its pointer
Tuple3* initTuple3(double a, double b, double c) {
    Tuple3* t = malloc(sizeof(Tuple3));
    t->a = a;
    t->b = b;
    t->c = c;
    return t;
}

// Creates a Point and returns its pointer
Point* initPoint(double x, double y, double z) {
    Point* p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    p->z = z;
    return p;
}

// Creates a PolarVector and returns its pointer
PolarVector* initPolarVector(double r, double theta, double phi) {
    PolarVector* pv = malloc(sizeof(PolarVector));
    pv->r = r;
    pv->theta = theta;
    pv->phi = phi;
    return pv;
}

// Creates a Vector and returns its pointer
Vector* initVector(double x, double y, double z) {
    Vector* v = malloc(sizeof(Vector));
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}

// Creates a Line and returns its pointer
Line* initLine(double v_x, double v_y, double v_z, double x, double y, double z) {
    Line* l = malloc(sizeof(Line));
    l->v = initVector(v_x, v_y, v_z);
    l->p = initPoint(x, y, z);
    return l;
}

// Creates a Plane and returns its pointer
Plane* initPlane(double x, double y, double z, double c) {
    Plane* p = malloc(sizeof(Plane));
    p->x_coeff = x;
    p->y_coeff = y;
    p->z_coeff = z;
    p->constant = c;
    return p;
}

// Deallocates a Tuple2
void freeTuple2(Tuple2* t) {
    free(t);
}

// Deallocates a Tuple3
void freeTuple3(Tuple3* t) {
    free(t);
}

// Deallocates a Point
void freePoint(Point* p) {
    free(p);
}

// Deallocates a PolarVector
void freePolarVector(PolarVector* pv) {
    free(pv);
}

// Deallocates a Vector
void freeVector(Vector* v) {
    free(v);
}

// Deallocates a Line
void freeLine(Line* line) {
    freeVector(line->v);
    freePoint(line->p);
    free(line);
}

// Deallocates a Plane
void freePlane(Plane* p) {
    free(p);
}
