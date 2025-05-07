#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

/**
 * @brief      Compute the squared norm of a 3D vector.
 */
double NormSquare(const float vect[3]);

/**
 * @brief      Compute the Euclidean norm of a 3D vector.
 */
double Norm(const float vect[3]);

/**
 * @brief      Compute the dot product of two 3D vectors.
 */
double DotProduct(const float vect1[3], const float vect2[3]);

/**
 * @brief      Compute the normalized cross product of two 3D vectors.
 *             The result is normalized in-place.
 */
void CrossProduct(const float vect1[3], const float vect2[3], float result[3]);

/**
 * @brief      Compute the angle (in radians) at point2 formed by vectors (point2->point1) and (point2->point3).
 */
double Angle(const float point1[3], const float point2[3], const float point3[3]);

#endif // MATH_UTILS_H