
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

void CalculateTchr_1(void){
    float m[16];

    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    
    // transposée pour la mat 3*3 en haut à gauche
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ogl.Tchr_1[i*4 + j] = m[j*4 + i];
        }
    }

    // les 3 0 en bas
    for (int col = 0; col < 3; ++col) {
        ogl.Tchr_1[3*4 + col] = 0.0f;
    }
    //en bas à droite
    ogl.Tchr_1[15] = 0.0f;

    ogl.Tchr_1[3] = ogl.obsX-ogl.focalX;
    ogl.Tchr_1[7] = ogl.obsY-ogl.focalY;
    ogl.Tchr_1[11] = ogl.obsZ-ogl.focalZ;
    glPopMatrix();

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

void MatriceVueProjectionY(void){
    // Calculer Tchr-1
    CalculateTchr_1();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
    // Multiplier par Py initialisée au début
    glMultMatrixf(ogl.Py);
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);

}


void MatriceProjection(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(ogl.Umin * ogl.fu * ogl.fzoom ,ogl.Umax * ogl.fu * ogl.fzoom,ogl.Vmin * ogl.fv * ogl.fzoom, ogl.Vmax * ogl.fv * ogl.fzoom,ogl.Dmin,ogl.Dmax);
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
    TracerObjet();
    // Projections
    TracerProjections();
    // Zbuffer Off
    ZbufferDesactivation();
    ViderMemoireEcran();
}