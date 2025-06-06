#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>
#include <stdbool.h>


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


/**
 * Soustrait deux vecteurs a - b, résultat dans out.
 *   out[i] = a[i] - b[i]  pour i=0,1,2
 */
void SubtractVectors(const float a[3], const float b[3], float out[3]);

/**
 * Additionne deux vecteurs a + b, résultat dans out.
 */
void AddVectors(const float a[3], const float b[3], float out[3]);

/**
 * Met le vecteur v à l'échelle s, résultat dans out.
 *   out[i] = v[i] * s  pour i=0,1,2
 */
void ScaleVector(const float v[3], float s, float out[3]);

/**
 * Normalise (en place) le vecteur v. 
 *   Si ||v|| > 0, alors v[i] := v[i] / ||v||  pour i=0,1,2
 *   Renvoie la norme initiale de v (avant division).
 */
double NormalizeVector(float v[3]);

/**
 * Calcule la projection de v sur axis (axis doit être un vecteur unitaire
 * ou non-unitaire, on projette quand même). 
 *   proj = ( (v⋅axis) / ||axis||^2 ) * axis 
 *   Résultat dans out.
 */
void ProjectOnto(const float v[3], const float axis[3], float out[3]);

/**
 * Calcule la composante de v orthogonale à axis, c’est-à-dire
 *   out = v - ( (v⋅axis) / ||axis||^2 ) * axis.
 *   Résultat dans out.
 */
void PerpComponent(const float v[3], const float axis[3], float out[3]);

/**
 * Produit vectoriel sans normalisation. 
 *   result = a × b (brut). 
 *   IMPORTANT : ne normalise pas le résultat !
 */
void RawCrossProduct(const float a[3], const float b[3], float result[3]);

#endif // MATH_UTILS_H