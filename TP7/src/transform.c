
#include <GL/glut.h>
#include <stdio.h>
#include "transform.h"
#include "opengl_state.h"
#include "renderer.h" 




void computeLastTransformation(){
    // On évalue la rotation
    switch ((ogl.flagTransformation & ROTATION_STATE) >> 0x1)
    {
    case 0x1: // On fait une rotation
        glTranslatef(ogl.focal_u,ogl.focal_v,ogl.focal_n);
        glRotatef(ogl.rot_u, -1.0, 0.0, 0.0); // U
        glRotatef(ogl.rot_v, 0.0, 1.0, 0.0); // V
        glTranslatef(-ogl.focal_u,-ogl.focal_v,-ogl.focal_n);
        ogl.rot_u=0.0f;
        ogl.rot_v=0.0f;
        break;
    case 0x0: // On ne fait rien
        break;
    default:
        printf("You should not go there...\n");
        break;
    }
    switch (ogl.flagTransformation & TRANSLATION_STATE)
    {
    case 0x1: // On fait une translation
        glTranslatef(ogl.trans_u, ogl.trans_v, 0.0); /* translation dans le plan de vue */
        //printf("Translation with u = %02.2f and v = %02.2f \n",ogl.trans_u,ogl.trans_v);
        /* mise à jour des coordonnées du point focal dans le repère de vue */
        ogl.focal_u += ogl.trans_u ;
        ogl.focal_v += ogl.trans_v ;
        ogl.trans_u=0;
        ogl.trans_v=0;
        break;
    case 0x0: // On ne fait rien
        break;
    default:
        printf("You should not go there...\n");
        break;
    }
    
}




void MatriceVueObjet(void)
{
    /* pointer la matrice courante sur l’instance GL_MODELVIEW */
    glMatrixMode(GL_MODELVIEW);
    /* Initialiser la matrice courante avec l’identité */
    glLoadIdentity();
    /* Appliquer la dernière transformation géométrique */
    computeLastTransformation();
    /* Appliquer toutes les transformations géométriques antécédentes */
    glMultMatrixf(ogl.geometricTransformations);
    /* Mettre à jour l’historique des transformations géométriques */
    glGetFloatv(GL_MODELVIEW_MATRIX, ogl.geometricTransformations);
    /* Appliquer le changement de repère */
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
}

void MatriceVuePlan(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
}

void MatriceVueProjectionX(void){
    glLoadIdentity();
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
    // Multiplier par Py initialisée au début
    glMultMatrixf(ogl.Px);
    PrintMatrix4x4("ogl.Px", ogl.Px);
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr_1", ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr", ogl.Tchr);
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    // Remultiplier par Tchr
    glMultMatrixf(ogl.Tchr);
}
void MatriceVueProjectionY(void){
    glLoadIdentity();
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
    // Multiplier par Py initialisée au début
    glMultMatrixf(ogl.Py);
    PrintMatrix4x4("ogl.Py", ogl.Py);
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr_1", ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr", ogl.Tchr);
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    // Remultiplier par Tchr
    glMultMatrixf(ogl.Tchr);
}
void MatriceVueProjectionZ(void){
    glLoadIdentity();
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
    // Multiplier par Pz initialisée au début
    glMultMatrixf(ogl.Pz);
    PrintMatrix4x4("ogl.Pz", ogl.Pz);
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr_1", ogl.Tchr_1);
    PrintMatrix4x4("ogl.Tchr", ogl.Tchr);
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    // Remultiplier par Tchr
    glMultMatrixf(ogl.Tchr);
}


void MatriceProjection(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(ogl.Umin * ogl.fu * ogl.fzoom ,ogl.Umax * ogl.fu * ogl.fzoom,ogl.Vmin * ogl.fv * ogl.fzoom, ogl.Vmax * ogl.fv * ogl.fzoom,ogl.Dmin,ogl.Dmax);
    glMatrixMode(GL_MODELVIEW);
}



void EffacerEcran(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}

void ViderMemoireEcran(void){
    glutSwapBuffers(); // -> for double buffer
    //glFlush(); // -> for 1 buffer
}


/**
 * @brief   GLUT display callback: clear, set view, draw, swap.
 */
void Display(void) {
    EffacerEcran();
    // Zbuffer On
    ZbufferActivation();
    // Plans
    TracerPlans();
    // Objet
    MatriceVueObjet();
    TracerObjet();
    // Projections
    TracerProjections();
    // Zbuffer Off
    ZbufferDesactivation();
    ViderMemoireEcran();
}