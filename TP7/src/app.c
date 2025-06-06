
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
    ogl.proj_mode = NONE;
}

void InitialiserOption1_Plan(void){
    ogl.mode_plan = 0;
}
void InitialiserOption2_Projections(void){
    float seuil = -0.03f;
    memset(ogl.Px, 0, 16 * sizeof(float));
    memset(ogl.Py, 0, 16 * sizeof(float));
    memset(ogl.Pz, 0, 16 * sizeof(float));
/*
0  4  8  12
1  5  9  13
2  6  10 14
3  7  11 15
*/
    ogl.Px[12]  = seuil; 
    ogl.Px[5]  = 1.0f;
    ogl.Px[10] = 1.0f;
    ogl.Px[15] = 1.0f;  

    ogl.Py[0]  = 1.0f; 
    ogl.Py[13]  = seuil;
    ogl.Py[10] = 1.0f;
    ogl.Py[15] = 1.0f;  

    ogl.Pz[0]  = 1.0f; 
    ogl.Pz[5]  = 1.0f;
    ogl.Pz[14] = seuil;
    ogl.Pz[15] = 1.0f;  
}

void InitialiserOption3_Projections(void){
    memset(ogl.Sx, 0, 16 * sizeof(float));
    memset(ogl.Sx, 0, 16 * sizeof(float));
    memset(ogl.Sx, 0, 16 * sizeof(float));
/*
0  4  8  12
1  5  9  13
2  6  10 14
3  7  11 15
*/
    ogl.Sx[0]  = -1.0f; 
    ogl.Sx[5]  =  1.0f;
    ogl.Sx[10] =  1.0f;
    ogl.Sx[15] =  1.0f;  

    ogl.Sy[0]  =  1.0f; 
    ogl.Sy[5]  = -1.0f;
    ogl.Sy[10] =  1.0f;
    ogl.Sy[15] =  1.0f;  

    ogl.Sz[0]  =  1.0f; 
    ogl.Sz[5]  =  1.0f;
    ogl.Sz[10] = -1.0f;
    ogl.Sz[15] =  1.0f;  
}

void InitialiserOption3_Normales(void){
    ogl.normales_basiques[0] = 1.0f ;
    ogl.normales_basiques[1] = 1.0f ;
    ogl.normales_basiques[2] = 1.0f ;

    ogl.normales_inv[0] = -1.0f ;
    ogl.normales_inv[1] = -1.0f ;
    ogl.normales_inv[2] = -1.0f ;


    ogl.normales_Current_Factor=ogl.normales_basiques;

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
    InitialiserOption3_Projections();
    InitialiserOption3_Normales();
    InitialiserSourceLumineuse();
    InitialiserMateriaux();
}


void InitialiserChangementRepere(void){

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ, ogl.focalX, ogl.focalY, ogl.focalZ, ogl.vertX, ogl.vertY, ogl.vertZ);
    glGetFloatv(GL_MODELVIEW_MATRIX,ogl.Tchr);
    float M[16];
    
    // tout = 0.0f
    memset(ogl.Tchr_1, 0.0f, 16 * sizeof(float));

    //transposer la mat 3*3 en haut à gauche
    for (int i=0; i<3; i++){
        for (int j=0; j<3;j++){
            ogl.Tchr_1[i+4*j] = ogl.Tchr[4*i+j];
        }
    }
    // Tout en bas à droite
    ogl.Tchr_1[15]=1.0;

    // ligne de droite
    ogl.Tchr_1[12]=ogl.obsX;
    ogl.Tchr_1[13]=ogl.obsY;
    ogl.Tchr_1[14]=ogl.obsZ;
    glMultMatrixf(ogl.Tchr_1);

    glGetFloatv(GL_MODELVIEW_MATRIX,M);

    PrintMatrix4x4("Tchr_1*Tchr",M);
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
    InitialiserOption3_Projections();

    InitialiserOption3_Normales();

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
    InitialiserChangementRepere(); // on le met ici car sinon la fenetre n'est pas encore initialisée
}

void BoucleInfinie(void) {
    glutMainLoop();
}