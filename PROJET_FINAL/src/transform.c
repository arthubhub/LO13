
#include <GL/glut.h>
#include <stdio.h>
#include "transform.h"
#include "opengl_state.h"
#include "renderer.h" 

const char* helpText[] = {
    "Commandes clavier :",
    "",
    " q, Q ou Échap   : Quitter",
    " z              : Zoom avant",
    " Z              : Zoom arrière",
    "",
    " 0              : Filaire (STPC)",
    " 1              : Filaire (ATPC)",
    " 2              : Solide dégradé (ATPC)",
    " 3              : Solide +  Filaire (ATPC)",
    " 4              : Ombrage constant",
    " 5              : Ombrage constant +  Filaire",
    " 6              : Filaire <- ombrage constant",
    " 7              : Ombrage Phong",
    " 8              : Phong +  Filaire",
    " 9              : Filaire <- Phong",
    " g              : Courbure de Gauss",
    " C              : Carreaux classiques",
    " F              : Carreaux fun",
    " D              : Lignes de diffusion",
    "",
    " a, c, p        : Changer matériau (acier, cuivre, plastique)",
    "",
    " s, S           : Shrink (réduction géométrique)",
    " i              : Réinitialiser paramètres graphiques",
    "",
    " P              : Activer/désactiver projection",
    " l              : Activer/désactiver miroirs",
    " k              : Activer/désactiver ombres",
    "",
    " r              : Afficher/masquer repère",
    "",
    "Utilise ces touches pour explorer les différents modes de rendu.",
    NULL
};

void AfficherAide(void) {
    // Texte en noir
    glColor3f(0, 0, 0);
    
    // Position de départ (en bas à gauche de l’écran 2D)
    glRasterPos2i(10, ogl.winSizeY - 20);
    
    for (int i = 0; helpText[i] != NULL; ++i) {
        for (const char* p = helpText[i]; *p; ++p) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
        }
        // Descendre d’une ligne (15 pixels environ)
        glRasterPos2i(10, ogl.winSizeY - 20 - (i+1)*15);
    }
}



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

void MatriceVueOmbre(const float* projectionMatrix, const char* matrixName){

    // O' = Tchr Tos Po Tso Tchr-1 Tg Tchr 0

    glLoadIdentity();
    
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
              ogl.focalX, ogl.focalY, ogl.focalZ,
              ogl.vertX, ogl.vertY, ogl.vertZ);

    // Appliquer Tos

    glTranslatef(ogl.src.position[0],ogl.src.position[1],ogl.src.position[2]);
    
    // Multiplier par la matrice de projection
    glMultMatrixf(projectionMatrix);
    //PrintMatrix4x4(matrixName, projectionMatrix);

    // Appliquer Tso
    glTranslatef(-ogl.src.position[0],-ogl.src.position[1],-ogl.src.position[2]);

    
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    //PrintMatrix4x4("ogl.Tchr_1", ogl.Tchr_1);
    //PrintMatrix4x4("ogl.Tchr", ogl.Tchr);
    
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    
    // Remultiplier par Tchr
    glMultMatrixf(ogl.Tchr);


}

void MatriceVueProjection(const float* projectionMatrix, const char* matrixName) {
    glLoadIdentity();

    //O' = Tchr (Px/Py/Pz/Sx/Sy/Sz) Tchr-1 Tg Tchr 0  
    
    // Appliquer Tchr
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
              ogl.focalX, ogl.focalY, ogl.focalZ,
              ogl.vertX, ogl.vertY, ogl.vertZ);
    
    // Multiplier par la matrice de projection (Px, Py, ou Pz, Sx, Sy ou Sz)
    glMultMatrixf(projectionMatrix);
    //PrintMatrix4x4(matrixName, projectionMatrix);
    
    // Multiplier par Tchr-1
    glMultMatrixf(ogl.Tchr_1);
    //PrintMatrix4x4("ogl.Tchr_1", ogl.Tchr_1);
    //PrintMatrix4x4("ogl.Tchr", ogl.Tchr);
    
    // Multiplier par Tg
    glMultMatrixf(ogl.geometricTransformations);
    
    // Remultiplier par Tchr
    glMultMatrixf(ogl.Tchr);
}

void MatriceVueProjectionX(void) {
    MatriceVueProjection(ogl.Px, "ogl.Px");
}

void MatriceVueProjectionY(void) {
    MatriceVueProjection(ogl.Py, "ogl.Py");
}

void MatriceVueProjectionZ(void) {
    MatriceVueProjection(ogl.Pz, "ogl.Pz");
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
        // repere
        TracerRepere();
        // Objet
        TracerObjetBasique();
        // Modes de projections, miroirs, plans, ombre ...
        TracerProjOptions();
    // Zbuffer Off
    ZbufferDesactivation();


    if (ogl.aide){
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, ogl.winSizeX, 0, ogl.winSizeY);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            AfficherAide();
            glPopAttrib();
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

    }

    ViderMemoireEcran();
}