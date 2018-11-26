#ifndef MEMORY_H_
#define MEMORY_H_

//
//      FUNCTIONS
//

Tuple2* initTuple2(double a, double b);
Tuple3* initTuple3(double a, double b, double c);
Point* initPoint(double x, double y, double z);
PolarVector* initPolarVector(double r, double theta, double phi);
Vector* initVector(double x, double y, double z);
Line* initLine(double v_x, double v_y, double v_z, double x, double y, double z);
Plane* initPlane(double x, double y, double z, double c);

void freeTuple2(Tuple2* t);
void freeTuple3(Tuple3* t);
void freePoint(Point* p);
void freePolarVector(PolarVector* pv);
void freeVector(Vector* v);
void freeLine(Line* line);
void freePlane(Plane* p);

#endif
