
#include <GL/glut.h>
#include <math.h>
#include "app.h"
#include "opengl_state.h"
#include "mesh.h"
#include "lighting.h"
#include "transform.h"
#include "renderer.h"
#include "input.h"

void InitialiserLibrairieGraphique(int *argc, char **argv) {
    glutInit(argc, argv);
}

void InitialiserParametresGraphiques(void) {
    /* fenetre */
    ogl.winSizeX=700;
    ogl.winSizeY=700;

    ogl.winPosX=100;
    ogl.winPosY=100;

    /* repere de vue */
    ogl.obsX = 8;
    ogl.obsY = 6;
    ogl.obsZ = 6;

    ogl.focalX=0.5;
    ogl.focalY=0.5;
    ogl.focalZ=0.5;

    ogl.vertX=0.0;
    ogl.vertY=0.0;
    ogl.vertZ=1.0;

    /*couleurs*/
    ogl.bgColorB=1.0;
    ogl.bgColorG=1.0;
    ogl.bgColorR=1.0;

    ogl.penColorR=0.0;
    ogl.penColorG=0.0;
    ogl.penColorB=0.0;

    ogl.fillColorR=0.5;
    ogl.fillColorG=0.5;
    ogl.fillColorB=0.0;

    /* perpective de projection*/
    ogl.Umin = -0.5;
    ogl.Umax = 0.5;
    ogl.Vmin = -0.5;
    ogl.Vmax = 0.5;
    ogl.Dmin = 6.5;
    ogl.Dmax = 15;

    /* zoom */

    ogl.fu = 1.0;
    ogl.fv = 1.0;
    ogl.fzoom = 1.0;



    /* Transformations */
    ogl.geometricTransformations = (float*)malloc(4*4*sizeof(float));
    for (int i = 0; i < 16; ++i) {
        ogl.geometricTransformations[i] = 0.0f;
    }
    ogl.geometricTransformations[0]=1.0f;
    ogl.geometricTransformations[5]=1.0f;
    ogl.geometricTransformations[10]=1.0f;
    ogl.geometricTransformations[15]=1.0f;

    ogl.flagTransformation=0x0;

    ogl.rot_u=0.0f;
    ogl.rot_v=0.0f;

    ogl.focal_u = 0.0f;
    ogl.focal_v = 0.0f;
    ogl.focal_n = -sqrtf( (ogl.obsX - ogl.focalX)*(ogl.obsX - ogl.focalX)
    + (ogl.obsY - ogl.focalY)*(ogl.obsY - ogl.focalY)
    + (ogl.obsZ - ogl.focalZ)*(ogl.obsZ - ogl.focalZ) );



    // Mode de tracé par défaut
    ogl.renderMode = FILAIRE_STPC;
    //ogl.renderMode = FILAIRE_UNIE_ATPC;
    //ogl.renderMode = SOLIDE_DEG_ATPC;
    //ogl.renderMode = SOLIDE_FILAIRE_ATPC;

    /* source lumineuse */
    InitialiserSourceLumineuse();

    /* materiaux */
    InitialiserMateriaux();




    ogl.shrink=0.0f;
}

void CreationFenetreGraphique(void) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(ogl.winSizeX, ogl.winSizeY);
    glutInitWindowPosition(ogl.winPosX, ogl.winPosY);
    glutCreateWindow("LO13 - Arthur");
}

void InitialiserEnvironnementGraphique(void) {
    glClearColor(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB, 0.0f);
    glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
    MatriceProjection();
}

void BoucleInfinie(void) {
    glutMainLoop();
}