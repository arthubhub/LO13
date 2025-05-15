
#include <GL/glut.h>
#include <math.h>
#include <string.h>
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

void InitialiserWindow(void){
    ogl.winSizeX=700;
    ogl.winSizeY=700;

    ogl.winPosX=100;
    ogl.winPosY=100;
}

void InitialiserRepereVue(void){
    ogl.obsX = 8;
    ogl.obsY = 6;
    ogl.obsZ = 6;

    ogl.focalX=0.5;
    ogl.focalY=0.5;
    ogl.focalZ=0.5;

    ogl.vertX=0.0;
    ogl.vertY=0.0;
    ogl.vertZ=1.0;
}

void InitialiserCouleurs(void){
    ogl.bgColorB=1.0;
    ogl.bgColorG=1.0;
    ogl.bgColorR=1.0;

    ogl.penColorR=0.0;
    ogl.penColorG=0.0;
    ogl.penColorB=0.0;

    ogl.fillColorR=0.5;
    ogl.fillColorG=0.5;
    ogl.fillColorB=0.0;
}

void InitialiserPerspectiveProj(void){
    ogl.Umin = -0.5;
    ogl.Umax = 0.5;
    ogl.Vmin = -0.5;
    ogl.Vmax = 0.5;
    ogl.Dmin = 6.5;
    ogl.Dmax = 15;
}

void InitialiserFacteursAnysotropie(void){
    ogl.fu = 1.0;
    ogl.fv = 1.0;
}

void InitilaiserZoom(void){
    ogl.fzoom = 1.0;
    ogl.shrink=0.0f;
}

void InitialiserMatTransformGeometrique(void){
    ogl.geometricTransformations = (float*)malloc(4*4*sizeof(float));
    for (int i = 0; i < 16; ++i) {
        ogl.geometricTransformations[i] = 0.0f;
    }
    ogl.geometricTransformations[0]=1.0f;
    ogl.geometricTransformations[5]=1.0f;
    ogl.geometricTransformations[10]=1.0f;
    ogl.geometricTransformations[15]=1.0f;

}
void InitialiserTransformations(void){
    ogl.flagTransformation=0x0;
    ogl.rot_u=0.0f;
    ogl.rot_v=0.0f;
    ogl.focal_u = 0.0f;
    ogl.focal_v = 0.0f;
    ogl.focal_n = -sqrtf( (ogl.obsX - ogl.focalX)*(ogl.obsX - ogl.focalX)
    + (ogl.obsY - ogl.focalY)*(ogl.obsY - ogl.focalY)
    + (ogl.obsZ - ogl.focalZ)*(ogl.obsZ - ogl.focalZ) );
}
void InitialiserModeTrace(void){
    ogl.renderMode = FILAIRE_STPC;
}

void InitialiserOption1_Plan(void){
    ogl.mode_plan = 0;
}
void InitialiserOption2_Projections(void){
    float seuil = -0.1;
    ogl.mode_projection = 0;
    float Tchr_1[16];
    float Px[16], Py[16], Pz[16];
    memset(Px, 0, 16 * sizeof(float));
    memset(Py, 0, 16 * sizeof(float));
    memset(Pz, 0, 16 * sizeof(float));
    memset(Tchr_1, 0, 16 * sizeof(float));

    Px[3]  = seuil; 
    Px[5]  = 1.0f;
    Px[10] = 1.0f;
    Px[15] = 1.0f;  

    Py[0]  = 1.0f; 
    Py[7]  = seuil;
    Py[10] = 1.0f;
    Py[15] = 1.0f;  

    Pz[0]  = 1.0f; 
    Pz[5]  = 1.0f;
    Pz[11] = seuil;
    Pz[15] = 1.0f;  

}


void ReinitialisationParamGraphiques(void) {
   //InitialiserWindow();
    InitialiserRepereVue();
    InitialiserCouleurs();
    InitialiserPerspectiveProj();
    //InitialiserFacteursAnysotropie();
    InitilaiserZoom();
    InitialiserMatTransformGeometrique();
    InitialiserTransformations();
    InitialiserModeTrace();
    InitialiserOption1_Plan();
    InitialiserOption2_Projections();
    InitialiserSourceLumineuse();
    InitialiserMateriaux();
}


void InitialiserParametresGraphiques(void) {
    /* fenetre */
    InitialiserWindow();

    /* repere de vue */
    InitialiserRepereVue();

    /*couleurs*/
    InitialiserCouleurs();

    /* perpective de projection*/
    InitialiserPerspectiveProj();

    /* zoom */
    InitialiserFacteursAnysotropie();

    InitilaiserZoom();


    /* Transformations */

    InitialiserMatTransformGeometrique();
    InitialiserTransformations();



    // Mode de tracé par défaut
    InitialiserModeTrace();
    //ogl.renderMode = FILAIRE_UNIE_ATPC;
    //ogl.renderMode = SOLIDE_DEG_ATPC;
    //ogl.renderMode = SOLIDE_FILAIRE_ATPC;

    /* source lumineuse */
    InitialiserSourceLumineuse();

    /* materiaux */
    InitialiserMateriaux();

   
    /* option 1 : Plan*/
    
    InitialiserOption1_Plan();

    /* option 2 : Projection*/
    InitialiserOption2_Projections();
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