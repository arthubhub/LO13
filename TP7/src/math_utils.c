#include "math_utils.h"
#include "mesh.h"
#include "opengl_state.h"

double NormSquare(const float vect[3]) {
    return vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2];
}

double Norm(const float vect[3]) {
    return sqrt(NormSquare(vect));
}

double DotProduct(const float vect1[3], const float vect2[3]) {
    return vect1[0]*vect2[0] + vect1[1]*vect2[1] + vect1[2]*vect2[2];
}

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


void CalculerBarycentre(int k, float *Gx, float *Gy, float *Gz) {
    int j, virt_base, real_base;
    *Gx = *Gy = *Gz = 0.0f;
    for (j = 0; j < 3; ++j) {
        virt_base = msh.triangles[k + j];
        real_base  = 3 * virt_base;
        *Gx += msh.vertices[real_base];
        *Gy += msh.vertices[real_base + 1];
        *Gz += msh.vertices[real_base + 2];
    }
    *Gx /= 3.0f;
    *Gy /= 3.0f;
    *Gz /= 3.0f;
}

float CalculerShrink(float A, float G) {
    return (1.0f - ogl.shrink) * A + ogl.shrink * G;
}


void SubtractVectors(const float a[3], const float b[3], float out[3]) {
    out[0] = a[0] - b[0];
    out[1] = a[1] - b[1];
    out[2] = a[2] - b[2];
}


void AddVectors(const float a[3], const float b[3], float out[3]) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
}


void ScaleVector(const float v[3], float s, float out[3]) {
    out[0] = v[0] * s;
    out[1] = v[1] * s;
    out[2] = v[2] * s;
}


double NormalizeVector(float v[3]) {
    double n = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    if (n > 0.0) {
        v[0] = (float)(v[0] / n);
        v[1] = (float)(v[1] / n);
        v[2] = (float)(v[2] / n);
    }
    return n;
}

/* Projection de v sur axis */
void ProjectOnto(const float v[3], const float axis[3], float out[3]) {
    /* d = v ⋅ axis */
    double d = v[0]*axis[0] + v[1]*axis[1] + v[2]*axis[2];
    /* ||axis||^2 */
    double axis_sq = axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2];
    if (axis_sq <= 0.0) {
        /* Si axis est le vecteur nul, on renvoie (0,0,0) */
        out[0] = out[1] = out[2] = 0.0f;
    } else {
        double scale = d / axis_sq;
        out[0] = (float)(axis[0] * scale);
        out[1] = (float)(axis[1] * scale);
        out[2] = (float)(axis[2] * scale);
    }
}


void PerpComponent(const float v[3], const float axis[3], float out[3]) {
    float proj[3];
    ProjectOnto(v, axis, proj);
    out[0] = v[0] - proj[0];
    out[1] = v[1] - proj[1];
    out[2] = v[2] - proj[2];
}


void RawCrossProduct(const float a[3], const float b[3], float result[3]) {
    result[0] = a[1]*b[2] - a[2]*b[1];
    result[1] = a[2]*b[0] - a[0]*b[2];
    result[2] = a[0]*b[1] - a[1]*b[0];
}