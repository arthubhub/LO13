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


/**
 * @brief   Calcule le barycentre du triangle d’indice k
 * @param   k   Index de début dans msh.triangles (3 sommets)
 * @param   Gx  Pointeur où stocker la coordonnée x du barycentre
 * @param   Gy  Pointeur où stocker la coordonnée y du barycentre
 * @param   Gz  Pointeur où stocker la coordonnée z du barycentre
 */
void CalculerBarycentre(int k, float *Gx, float *Gy, float *Gz);

/**
 * @brief   Calcule la coordonnée rétractée A' = (1–s)*A + s*G
 * @param   A   Coordonnée d’origine (x, y ou z)
 * @param   G   Coordonnée du barycentre (x, y ou z)
 * @return      La coordonnée rétractée selon ogl.shrink
 */
float CalculerShrink(float A, float G);


#endif // MATH_UTILS_H