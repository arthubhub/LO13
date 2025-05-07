#include "math_utils.h"

// Compute squared norm of vect
double NormSquare(const float vect[3]) {
    return vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2];
}

// Compute Euclidean norm of vect
double Norm(const float vect[3]) {
    return sqrt(NormSquare(vect));
}

// Compute dot product of vect1 and vect2
double DotProduct(const float vect1[3], const float vect2[3]) {
    return vect1[0]*vect2[0] + vect1[1]*vect2[1] + vect1[2]*vect2[2];
}

// Compute normalized cross product
void CrossProduct(const float vect1[3], const float vect2[3], float result[3]) {
    result[0] = vect1[1]*vect2[2] - vect1[2]*vect2[1];
    result[1] = vect1[2]*vect2[0] - vect1[0]*vect2[2];
    result[2] = vect1[0]*vect2[1] - vect1[1]*vect2[0];
    double norm = Norm(result);
    if (norm != 0.0) {
        result[0] /= norm;
        result[1] /= norm;
        result[2] /= norm;
    }
}

// Compute angle between (point1-point2) and (point3-point2)
double Angle(const float point1[3], const float point2[3], const float point3[3]) {
    float v1[3], v2[3];
    for (int i = 0; i < 3; ++i) {
        v1[i] = point1[i] - point2[i];
        v2[i] = point3[i] - point2[i];
    }
    double dot = DotProduct(v1, v2);
    double denom = sqrt(NormSquare(v1) * NormSquare(v2));
    if (denom == 0.0) {
        return 0.0;
    }
    double cosA = dot / denom;
    if (cosA > 1.0) cosA = 1.0;
    if (cosA < -1.0) cosA = -1.0;
    return acos(cosA);
}
