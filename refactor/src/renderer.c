
#include <GL/glut.h>
#include "renderer.h"
#include "opengl_state.h"
#include "mesh.h"
#include "lighting.h"
#include "transform.h"


void  ZbufferActivation (void) { 
    glEnable (GL_DEPTH_TEST); 
}

void  ZbufferDesactivation (void) { 
    glDisable (GL_DEPTH_TEST); 
}

void DecalageAvantActivation(void)
{
    glPolygonOffset (1.0, 1.0);
    glEnable (GL_POLYGON_OFFSET_LINE); 
}
void DecalageAvantDesactivation(void)
{
    glDisable (GL_POLYGON_OFFSET_LINE);
}
void DecalageArriereActivation(void)
{
glPolygonOffset (1.0, 1.0);
glEnable (GL_POLYGON_OFFSET_FILL); 
}
void DecalageArriereDesactivation(void)
{
glDisable (GL_POLYGON_OFFSET_FILL);
}

/**
 * @brief       Rend un triangle selon son index dans la table des triangles
 * 
 * Cette fonction ajoute le triangle donné au tracé en utilisant glVertex3f.
 * Elle peut être utilisée en mode uni ou en mode dégradé
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTriangleUnique(int k){ 
    int j,virt_base,real_base;
    for (j=0; j < 3; j++){
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;  // Pas de "-1", indexation à partir de 0
        glVertex3f(msh.vertices[real_base],     // x
                   msh.vertices[real_base + 1], // y
                   msh.vertices[real_base + 2]  // z
        );
    }
}
/**
 * @brief       Rend un triangle avec un shrink
 * 
 * Cette fonction ajoute le triangle donné au tracé en utilisant glVertex3f.
 * Elle peut être utilisée en mode uni ou en mode dégradé
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTriangleUniqueShrink(int k){ 
    int j, virt_base, real_base;
    float Gx = 0.0f, Gy = 0.0f, Gz = 0.0f;
    float Sx, Sy, Sz;

    // 1. Calcul du barycentre G
    for (j = 0; j < 3; j++){
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;
        Gx += msh.vertices[real_base];
        Gy += msh.vertices[real_base + 1];
        Gz += msh.vertices[real_base + 2];
    }
    Gx /= 3.0f;  Gy /= 3.0f;  Gz /= 3.0f;

    // 2. Pour chaque sommet : d'abord récupérer A, puis calculer A'
    for (j = 0; j < 3; j++){
        // a) retrouver le sommet et son offset
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;

        // b) calcul du point réduit : A' = (1–s)*A + s*G
        Sx = (1.0f - ogl.shrink) * msh.vertices[real_base]     + ogl.shrink * Gx;
        Sy = (1.0f - ogl.shrink) * msh.vertices[real_base + 1] + ogl.shrink * Gy;
        Sz = (1.0f - ogl.shrink) * msh.vertices[real_base + 2] + ogl.shrink * Gz;

        // c) envoi à OpenGL
        glVertex3f(Sx, Sy, Sz);
    }
}
/**
 * @brief       Rend un triangle selon son index dans la table des triangles avec un ombrage de Phong
 * 
 * Cette fonction ajoute le triangle donné au tracé en utilisant glVertex3f.
 * Elle est utilisée pour le tracé avec un ombrage de Phong.
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTriangleUniquePhong(int k){ 
    int j,virt_base,real_base;
    for (j=0; j < 3; j++){ // pour chaque sommet du triangle
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;  // Pas de "-1", indexation à partir de 0

        /* les coordonnées de la normale au sommet du triangle i */
        glNormal3f(
            msh.normal_v[real_base],     //normale_x
            msh.normal_v[real_base + 1], //normale_y
            msh.normal_v[real_base + 2]  //normale_z
        );

        /* les coordonnées du  sommet du triangle i */
        glVertex3f(msh.vertices[real_base],     // x
                   msh.vertices[real_base + 1], // y
                   msh.vertices[real_base + 2]  // z
        );
    }
}

/**
 * @brief       Rend l'objet courant selon le mode de tracé défini.
 * 
 * Cette fonction utilise le mode de tracé courant (champ ogl.renderMode)
 * pour configurer OpenGL et dessiner l'objet 3D représenté par le mesh msh.
 * Les différents modes qui l'utilisent incluent :
 *  - FILAIRE_STPC
 *  - FILAIRE_UNIE_ATPC
 *  - SOLIDE_FILAIRE_ATPC
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTrianglesShrink(void){
    int i,k;
    
    glBegin(GL_TRIANGLES);
    for (i=0; i < msh.number_of_triangles; i++){
        k = 3*i;
        TracerTriangleUniqueShrink(k);
    }
    glEnd();
}

/**
 * @brief       Rend l'objet courant selon le mode de tracé défini.
 * 
 * Cette fonction utilise le mode de tracé courant (champ ogl.renderMode)
 * pour configurer OpenGL et dessiner l'objet 3D représenté par le mesh msh.
 * Les différents modes qui l'utilisent incluent :
 *  - FILAIRE_STPC
 *  - FILAIRE_UNIE_ATPC
 *  - SOLIDE_FILAIRE_ATPC
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTrianglesBasique(void){
    
    if (ogl.shrink>0.01f){
        TracerTrianglesShrink();
        return;
    }

    int i,k;
    
    glBegin(GL_TRIANGLES);
    for (i=0; i < msh.number_of_triangles; i++){
        k = 3*i;
        TracerTriangleUnique(k);
    }
    glEnd();
}



/**
 * @brief       Trace les triangles avec un dégradé linéaire de rouge à bleu.
 * 
 * Cette fonction applique un dégradé linéaire pour chaque triangle
 * du mesh en fonction de son indice dans la liste des triangles.
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTrianglesDegLineaire(void) {
    int i, k;
    float r,g,b;
    float t = 1 / (float)(msh.number_of_triangles - 1);  // t de 0 à 1

    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        r = 1.0f - t*i;  // Diminue le rouge
        b = t*i;          // Augmente le bleu
        g = 0.0f;       
        glColor3f(r, g, b); 
        k = 3 * i;
        TracerTriangleUnique(k);
    }
    glEnd();
}

/**
 * @brief       Trace les triangles avec la normale pour un obrage linéaire
 * 
 * Cette fonction applique un ombrage linéaire pour chaque triangle
 * du mesh en fonction de son indice dans la liste des triangles.
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTrianglesOmbrageConstant(void) {
    int i, k;
    float normale_x, normale_y, normale_z;
    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        k = 3 * i;
        normale_x=msh.normal_t[k];
        normale_y=msh.normal_t[k+1];
        normale_z=msh.normal_t[k+2];
        glNormal3f(normale_x, normale_y, normale_z);
        TracerTriangleUnique(k);
    }
    glEnd();
}

/**
 * @brief       Trace les triangles avec un ombrage de Phong
 * 
 * Cette fonction applique un ombrage de Phong pour chaque triangle
 * du mesh en fonction de son indice dans la liste des triangles.
 * 
 * @note        Cette fonction suppose que le contexte OpenGL est actif.
 */
void TracerTrianglesPhong(void) {
    int i, k;
    float normale_x, normale_y, normale_z;
    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        k = 3 * i;
        normale_x=msh.normal_t[k];
        normale_y=msh.normal_t[k+1];
        normale_z=msh.normal_t[k+2];
        glNormal3f(normale_x, normale_y, normale_z);
        TracerTriangleUniquePhong(k);
    }
    glEnd();
}



void TracerObjet(void){
    switch (ogl.renderMode) {
        case FILAIRE_STPC: {
            // Zbuffer -> non
            // Polygone mode 
            // couleur -> ogl.pencolorR,G,B
            // Draw Triangles basic
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            TracerTrianglesBasique();
            }
            break;
        case FILAIRE_UNIE_ATPC:{
            // Zbuffer -> oui
            // mode remplissage avec ogl.bgColorR,G,B
            // reculer l'objet
            // draw triangles basic
            // avancer l'objet
            // mode contours avec ogl.pencolorR,G,B
            // draw triangles basic
            // Zbuffer -> non
            ZbufferActivation();
                DecalageArriereActivation();
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
                    
                    TracerTrianglesBasique();  // Tracé en mode remplissage
                DecalageArriereDesactivation();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
                TracerTrianglesBasique();
                //TracerTrianglesBasique();  // Tracé en mode contour
            ZbufferDesactivation();

            }
            break;
        case SOLIDE_DEG_ATPC:{
            // Zbuffer -> oui
            // mode remplissage 
            // choisir couleur de début et de fin
            // draw triangles mode degradé
            // Zbuffer -> non
            ZbufferActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            TracerTrianglesDegLineaire(); // Tracé en mode dégradé
            ZbufferDesactivation();
            }
            break;
        case SOLIDE_FILAIRE_ATPC:{
            // Zbuffer -> oui
            // mode remplissage avec ogl.fillColorR,G,B
            // reculer l'objet
            // draw triangles basic
            // avancer l'objet
            // mode contours avec ogl.pencolorR,G,B
            // draw triangles basic
            // Zbuffer -> non

            ZbufferActivation();
            glColor3f(ogl.fillColorR, ogl.fillColorG, ogl.fillColorB);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                DecalageArriereActivation();
                    TracerTrianglesBasique();  // Tracé en mode remplissage 
                DecalageArriereDesactivation();

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
                TracerTrianglesBasique();  // Tracé en mode contour
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_CONST:{
            ZbufferActivation();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                /* ombrage */
                ActivationSource() ;
                AffectationMateriau() ;
                glShadeModel(GL_FLAT) ; /* mode ombrage constant */
                    TracerTrianglesOmbrageConstant(); 
                DesactivationSource() ;
 
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_CONST_FILAIRE:{
            ZbufferActivation();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                DecalageArriereActivation();
                    ActivationSource() ;
                    AffectationMateriau() ;
                    glShadeModel(GL_FLAT) ; /* mode ombrage constant */
                        TracerTrianglesOmbrageConstant();  // Tracé en mode ombrage constant 
                    DesactivationSource() ;
                DecalageArriereDesactivation();

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
                TracerTrianglesBasique();  // Tracé en mode contour
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_FILAIRE_CONST:{
            ZbufferActivation();
                DecalageArriereActivation();
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
                    TracerTrianglesBasique();  // Tracé en mode remplissage
                DecalageArriereDesactivation();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                ActivationSource() ;
                AffectationMateriau() ;
                glShadeModel(GL_FLAT) ; /* mode ombrage constant */
                    TracerTrianglesOmbrageConstant();  // Tracé en mode contour
                DesactivationSource() ;
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_PHONG:{
            ZbufferActivation();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                /* ombrage */
                ActivationSource() ;
                AffectationMateriau() ;
                glShadeModel(GL_SMOOTH) ; /* mode ombrage de Phong */
                    TracerTrianglesPhong(); 
                DesactivationSource() ;
 
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_PHONG_FILAIRE:{
            ZbufferActivation();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                DecalageArriereActivation();
                    ActivationSource() ;
                    AffectationMateriau() ;
                    glShadeModel(GL_SMOOTH) ; /* mode ombrage de Phong */
                        TracerTrianglesPhong(); 
                    DesactivationSource() ;
                DecalageArriereDesactivation();
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
                TracerTrianglesBasique();  // Tracé en mode contour
            ZbufferDesactivation();
            }
            break;
        case OMBRAGE_FILAIRE_PHONG:{
            ZbufferActivation();
                DecalageArriereActivation();
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
                    TracerTrianglesBasique();  // Tracé en mode remplissage
                DecalageArriereDesactivation();
                
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                ActivationSource() ;
                AffectationMateriau() ;
                glShadeModel(GL_SMOOTH) ; /* mode ombrage de Phong */
                    TracerTrianglesPhong(); 
                DesactivationSource() ;
            ZbufferDesactivation();
            }
            break;

        
    }
}
