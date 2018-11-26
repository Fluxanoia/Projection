#include    <math.h>
#include   <stdio.h>
#include  <string.h>
#include <stdbool.h>
#include   "maths.h"

//
//      OUTPUT FUNCTIONS
//

void printPlane(Plane* p) {
    printf("\n --- Plane --- \n");
    printf("   x: %f\n", p->x_coeff);
    printf("   y: %f\n", p->y_coeff);
    printf("   z: %f\n", p->z_coeff);
    printf("   c: %f\n", p->constant);
    printf(" ------------- \n\n");
}

void printVector(Vector* v) {
    printf("\n --- Vector --- \n");
    printf("   x: %f\n", v->x);
    printf("   y: %f\n", v->y);
    printf("   z: %f\n", v->z);
    printf(" -------------- \n\n");
}

void printPolarVector(PolarVector* pv) {
    printf("\n --- Polar Vector --- \n");
    printf("   r: %f\n", pv->r);
    printf("   t: %f\n", pv->theta);
    printf("   p: %f\n", pv->phi);
    printf(" -------------------- \n\n");
}

//
//      MATH FUNCTIONS
//

// Sums two vectors
void vectorSum(Vector* o, Vector* v, Vector* u) {
    o->x = v->x + u->x;
    o->y = v->y + u->y;
    o->z = v->z + u->z;
}

// Multiplies all the values in the vector by the value
void vectorMultiply(Vector* v, double p) {
    v->x *= p;
    v->y *= p;
    v->z *= p;
}

// Multiplies all the values in the tuple by the value
void tuple2Multiply(Tuple2* t, double p) {
    t->a *= p;
    t->b *= p;
}

// Returns the length of a vector
double norm(Vector* v) {
    return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z)); 
}

// Makes the plane that v is normal to
void normalPlane(Plane* p, Vector* v) {
    p->x_coeff = v->x;
    p->y_coeff = v->y;
    p->z_coeff = v->z;
}

// Makes v the normal to a plane
void normalVector(Vector* v, Plane* p) {
    v->x = p->x_coeff;
    v->y = p->y_coeff;
    v->z = p->z_coeff;
}

// Fills all the vectors values with zero
void zeroVector(Vector* v) {
    v->x = 0;
    v->y = 0;
    v->z = 0;
}

// Fills all the plane's values with zero
void zeroPlane(Plane* p) {
    p->x_coeff = 0;
    p->y_coeff = 0;
    p->z_coeff = 0;
    p->constant = 0;
}

// Reduces a vector until its norm is one
void reduceToUnit(Vector* v) {
    double n = norm(v);
    v->x /= n;
    v->y /= n;
    v->z /= n;
}

// Alters atan to be more useful for polar conversions [atan(a / b)]
double polarATan(double a, double b) {
    if (b == 0) {
        return PI / 2;
    } else {
        if (b > 0) {
            return atan(a / b);
        }
        if (b < 0) {
            return PI + atan(a / b);
        }
    }
    return 0;
}

// Fills in the vector with the coordinates corresponding to the input
// polar values
void vectorFromPolar(Vector* v, PolarVector* pv) {
    v->x = pv->r * sin(pv->theta) * cos(pv->phi);
    v->y = pv->r * sin(pv->theta) * sin(pv->phi);
    v->z = pv->r * cos(pv->theta);
}

void polarVectorFromCart(PolarVector* pv, Vector* v) {
    pv->r = norm(v);
    pv->theta = acos(v->z / pv->r);
    pv->phi = polarATan(v->y, v->x);
}

// Cross v and w and puts it in the output
void crossVector(Vector* output, Vector* v, Vector* w) {
    output->x = (v->y) * (w->z) - (v->z) * (w->y);
    output->y = (v->z) * (w->x) - (v->x) * (w->z);
    output->z = (v->x) * (w->y) - (v->y) * (w->x);
}

// Gets the plane perpendicular to v and puts it in the output
void crossPlane(Plane* output, Vector* v) {
    output->x_coeff = v->x;
    output->y_coeff = v->y;
    output->z_coeff = v->z;
    output->constant = 0;
}

// Gets the orthonormals in the plane
void getOrthonormals(Plane* p, Vector* u, Vector* w) {
    // Variables to be used
    Vector v;
    PolarVector v_polar, u_polar;

    // Get the vector normal to the plane
    normalVector(&v, p);
    polarVectorFromCart(&v_polar, &v);

    // Get the first orthonormal
    u_polar.r = 1;
    u_polar.theta = v_polar.theta - (PI / 2);
    u_polar.phi = v_polar.phi;

    // Get the last orthonormal via cross product
    vectorFromPolar(u, &u_polar);
    crossVector(w, u, &v);
    reduceToUnit(w);
}

// Essentially solves for the coefficients such that av + bu + cw = p and puts them in o
bool directionTo(Tuple3* o, Vector* v, Vector* u, Vector* w, Point* p) {
    double matrix[3][4];
    
    matrix[0][0] = v->x;
    matrix[1][0] = v->y;
    matrix[2][0] = v->z;

    matrix[0][1] = u->x;
    matrix[1][1] = u->y;
    matrix[2][1] = u->z;

    matrix[0][2] = w->x;
    matrix[1][2] = w->y;
    matrix[2][2] = w->z;

    matrix[0][3] = p->x;
    matrix[1][3] = p->y;
    matrix[2][3] = p->z;

    // What submatrix are we looking at
    for (int sub = 0; sub < 3; sub++) {
        // Get the largest digit in the top right
        int max_i;
        double max = 0;
        for (int i = sub; i < 3; i++) {
            if (fabs(matrix[i][sub]) > max) {
                max = fabs(matrix[i][sub]);
                max_i = i;
            }
        }
        if (max == 0) return false;
        for (int j = 0; j < 4; j++) {
            double row[4];
            row[j] = matrix[max_i][j];
            matrix[max_i][j] = matrix[sub][j];
            matrix[sub][j] = row[j];
        }
        // Make the left most non-zero digit 1
        for (int j = 3; j > -1; j--) matrix[sub][j] /= matrix[sub][sub];
        // Take away the top submatrix row from the others to make the rest of the column zero
        for (int i = 0; i < 3; i++) {
            if (i == sub) continue;
            if (matrix[i][sub] != 0) {
                for (int j = 3; j > sub - 1; j--) matrix[i][j] -= matrix[i][sub] * matrix[sub][j];
            }
        }
    }

    o->a = matrix[0][3];
    o->b = matrix[1][3];
    o->c = matrix[2][3];
    return true;
}
