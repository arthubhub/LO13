#ifndef OPENGL_STATE_H
#define OPENGL_STATE_H

#include <stdint.h>
#include "lighting.h"

/**
 * @brief Available rendering modes.
 */
typedef enum {
    FILAIRE_STPC,
    FILAIRE_UNIE_ATPC,
    SOLIDE_DEG_ATPC,
    SOLIDE_FILAIRE_ATPC,
    OMBRAGE_CONST,
    OMBRAGE_CONST_FILAIRE,
    OMBRAGE_FILAIRE_CONST,
    OMBRAGE_PHONG,
    OMBRAGE_PHONG_FILAIRE,
    OMBRAGE_FILAIRE_PHONG
} RenderMode;

/**
 * @brief Available option .
 */
typedef enum {
    NONE,
    PROJECTION,
    MIROIRS,
    OMBRE
} ProjectionMode;

/**
 * @brief Global state for all OpenGL settings and transforms.
 */
typedef struct opengl {
    /* Window geometry */
    int      winSizeX, winSizeY;
    int      winPosX,  winPosY;

    /* View reference frame */
    float    obsX, obsY, obsZ;
    float    focalX, focalY, focalZ;
    float    vertX, vertY, vertZ;

    /* Colors */
    float    bgColorR, bgColorG, bgColorB;
    float    penColorR, penColorG, penColorB;
    float    fillColorR, fillColorG, fillColorB;

    /* Projection parameters */
    float    Umin, Umax;
    float    Vmin, Vmax;
    float    Dmin, Dmax;

    /* Reshape factors and zoom */
    float    fu, fv;
    float    fzoom;

    /* Mouse-based rotation state */
    int32_t  sx0, sy0;
    float    rot_u, rot_v;

    /* Accumulated geometric transform matrix */
    float   *geometricTransformations;
    uint8_t  flagTransformation;
    float    focal_u, focal_v, focal_n;
    float    trans_u,   trans_v;

    /* Active render mode */
    RenderMode renderMode;

    /* Lighting source */
    Source   src;

    /* Materials and current material */
    Material plastic, copper, steel;
    Material *current_mat;

    /* Shrink factor for face shrink effect */
    float    shrink;

    /* option 1 : Plan*/
    int mode_plan;

    /* options : menu*/
    ProjectionMode proj_mode;

    /* option 2 : Projection*/
    float Tchr[16];
    float Tchr_1[16];
    float Px[16], Py[16], Pz[16];


    /* option 3 : Miroirs*/
    float Sx[16], Sy[16], Sz[16];

    /*facteurs de normales*/
    float normales_basiques[3];
    float normales_inv[3];

    float * normales_Current_Factor;

    /* option 4 : Ombre*/
    float Po[16];
    float PlanOmbre_Z;
    


} Opengl;

/* Global instance */
extern Opengl ogl;

#endif // OPENGL_STATE_H