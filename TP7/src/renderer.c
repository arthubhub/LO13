
#include <GL/glut.h>
#include <stdio.h>
#include "renderer.h"
#include "opengl_state.h"
#include "mesh.h"
#include "lighting.h"
#include "transform.h"
#include "math_utils.h"



/**
 * @brief   Affiche une matrice 4×4 dans la console.
 * @param   name  Nom à afficher avant la matrice (peut être NULL).
 * @param   M     Matrice 4×4 stockée en row-major (M[row*4 + col]).
 */
void PrintMatrix4x4(const char *name, const float M[16]) {
    if (name) {
        printf("%s =\n", name);
    }
    for (int i = 0; i < 4; ++i) {
        printf("[ ");
        for (int j = 0; j < 4; ++j) {
            printf("%8.4f", M[j*4 + i]);
            if (j < 3) printf(", ");
        }
        printf(" ]\n");
    }
}


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
 * @brief   Rend un triangle selon son index, avec ou sans « shrink »
 * @param   k   Index de début dans msh.triangles (3 sommets)
 *
 * Si ogl.shrink > 0.01f, on calcule d’abord le barycentre puis on
 * rétracte chaque sommet vers ce barycentre.
 * Sinon, on rend le triangle « normal ».
 *
 * @note    suppose que le contexte OpenGL est actif.
 */
void TracerTriangleUnique(int k) {
    int j, virt_base, real_base;
    int shrink_state = (ogl.shrink > 0.01f) ? 1 : 0;
    float Gx = 0.0f, Gy = 0.0f, Gz = 0.0f;

    if (shrink_state) {
        CalculerBarycentre(k, &Gx, &Gy, &Gz);
    }

    for (j = 0; j < 3; ++j) {
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;

        if (shrink_state) {
            float Sx = CalculerShrink(msh.vertices[real_base],     Gx);
            float Sy = CalculerShrink(msh.vertices[real_base + 1], Gy);
            float Sz = CalculerShrink(msh.vertices[real_base + 2], Gz);
            glVertex3f(Sx, Sy, Sz);
        }
        else {
            glVertex3f(
                msh.vertices[real_base],
                msh.vertices[real_base + 1],
                msh.vertices[real_base + 2]
            );
        }
    }
}
/**
 * @brief       Rend un triangle selon son index dans la table des triangles
 *              avec un ombrage de Phong et optionnellement un « shrink ».
 * 
 * Cette fonction ajoute le triangle donné au tracé en utilisant glNormal3f
 * pour les normales vertex et glVertex3f pour les sommets, en appliquant
 * un rétrécissement vers le barycentre si ogl.shrink > 0.01f.
 * 
 * @note        suppose que le contexte OpenGL est actif.
 */
void TracerTriangleUniquePhong(int k) {
    int j, virt_base, real_base;
    int shrink_state = (ogl.shrink > 0.01f) ? 1 : 0;
    float Gx = 0.0f, Gy = 0.0f, Gz = 0.0f;

    /* 1) calcul du barycentre si shrink actif */
    if (shrink_state) {
        CalculerBarycentre(k, &Gx, &Gy, &Gz);
    }

    /* 2) pour chaque sommet du triangle */
    for (j = 0; j < 3; ++j) {
        virt_base = msh.triangles[k + j];
        real_base = 3 * virt_base;

        /* a) normales pour Phong */
        glNormal3f(
            msh.normal_v[real_base],     // normale x
            msh.normal_v[real_base + 1], // normale y
            msh.normal_v[real_base + 2]  // normale z
        );

        /* b) coordonnées des sommets, shrink ou non */
        if (shrink_state) {
            float Sx = CalculerShrink(msh.vertices[real_base],     Gx);
            float Sy = CalculerShrink(msh.vertices[real_base + 1], Gy);
            float Sz = CalculerShrink(msh.vertices[real_base + 2], Gz);
            glVertex3f(Sx, Sy, Sz);
        }
        else {
            glVertex3f(
                msh.vertices[real_base], 
                msh.vertices[real_base + 1], 
                msh.vertices[real_base + 2]
            );
        }
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
void TracerTrianglesBasique(void){
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


void TracerFilaireSTPC(){
    // Zbuffer -> non
    // Polygone mode 
    // couleur -> ogl.pencolorR,G,B
    // Draw Triangles basic
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
    TracerTrianglesBasique();
    
}

void TracerFilaireUnieATPC(void){
    // Zbuffer -> oui
    // mode remplissage avec ogl.bgColorR,G,B
    // reculer l'objet
    // draw triangles basic
    // avancer l'objet
    // mode contours avec ogl.pencolorR,G,B
    // draw triangles basic
    // Zbuffer -> non
    //ZbufferActivation();
        DecalageArriereActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
            
            TracerTrianglesBasique();  // Tracé en mode remplissage
        DecalageArriereDesactivation();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
        TracerTrianglesBasique();
        //TracerTrianglesBasique();  // Tracé en mode contour
    //ZbufferDesactivation();
}


void TracerSolideDegATPC(void){
    // Zbuffer -> oui
    // mode remplissage 
    // choisir couleur de début et de fin
    // draw triangles mode degradé
    // Zbuffer -> non
    //ZbufferActivation();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TracerTrianglesDegLineaire(); // Tracé en mode dégradé
    //ZbufferDesactivation();
}

void TracerSolideFilaireAPTC(void){
    // Zbuffer -> oui
    // mode remplissage avec ogl.fillColorR,G,B
    // reculer l'objet
    // draw triangles basic
    // avancer l'objet
    // mode contours avec ogl.pencolorR,G,B
    // draw triangles basic
    // Zbuffer -> non

    //ZbufferActivation();
    glColor3f(ogl.fillColorR, ogl.fillColorG, ogl.fillColorB);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DecalageArriereActivation();
            TracerTrianglesBasique();  // Tracé en mode remplissage 
        DecalageArriereDesactivation();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
        TracerTrianglesBasique();  // Tracé en mode contour
    //ZbufferDesactivation();
}

void TracerOmbrageConst(void){
    //ZbufferActivation();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        /* ombrage */
        ActivationSource() ;
        AffectationMateriau() ;
        glShadeModel(GL_FLAT) ; /* mode ombrage constant */
            TracerTrianglesOmbrageConstant(); 
        DesactivationSource() ;

    //ZbufferDesactivation();
}

void TracerOmbrageConstFilaire(void){
    //ZbufferActivation();
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
    //ZbufferDesactivation();
}

void TracerOmbrageFilaireConst(void){
    //ZbufferActivation();
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
    //ZbufferDesactivation();
}
void TracerOmbragePhong(void){
    //ZbufferActivation();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        /* ombrage */
        ActivationSource() ;
        AffectationMateriau() ;
        glShadeModel(GL_SMOOTH) ; /* mode ombrage de Phong */
            TracerTrianglesPhong(); 
        DesactivationSource() ;

    //ZbufferDesactivation();
}
void TracerOmbragePhongFilaire(void){
    //ZbufferActivation();
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
    //ZbufferDesactivation();
}
void TracerOmbrageFilairePhong(void){
    //ZbufferActivation();
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
    //ZbufferDesactivation();
}


void DessinerPlans(void){
    int i;
    int pas = 10; // >0
    float decallage=-0.03f;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
    glBegin(GL_LINES);
    {
        for (i = 0 ; i<=pas ; i++){
            float t = (float)i / pas;

            // Plan z = 0 
            glVertex3f(t, 0.0f, decallage);
            glVertex3f(t, 1.0f, decallage);
            glVertex3f(0.0f, t, decallage);
            glVertex3f(1.0f, t, decallage);

            // Plan y = 0
            glVertex3f(t, decallage, 0.0f);
            glVertex3f(t, decallage, 1.0f);
            glVertex3f(0.0f, decallage, t);
            glVertex3f(1.0f, decallage, t);

            // Plan x = 0
            glVertex3f(decallage, t, 0.0f);
            glVertex3f(decallage, t, 1.0f);
            glVertex3f(decallage, 0.0f, t);
            glVertex3f(decallage, 1.0f, t);
            printf("Plans : t = %f\n",t);
            
        }
    }
    glEnd();


}

void TracerPlans(void){
    if (ogl.mode_plan){
        glPushMatrix();
        {
        MatriceVuePlan();
        DessinerPlans();
        }
        glPopMatrix();
    }

}

void TracerProjectionX(void){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    MatriceVueProjectionX();
    glColor3f(1.0, 0.6, 0.6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TracerTrianglesBasique();
    glPopMatrix();
}
void TracerProjectionY(void){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    MatriceVueProjectionY();
    glColor3f(0.6, 1.0, 0.6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TracerTrianglesBasique();
    glPopMatrix();
}
void TracerProjectionZ(void){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    MatriceVueProjectionZ();
    glColor3f(0.6, 0.6, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TracerTrianglesBasique();
    glPopMatrix();
}

void TracerProjections(void){
    if (ogl.mode_projection){
        //ZbufferActivation();
        TracerProjectionX(); // on trace la projections sur X
        TracerProjectionY(); // on trace la projections sur Y
        TracerProjectionZ(); // on trace la projections sur Z
        //ZbufferDesactivation();
        
    }
}


void TracerObjet(void){
    

    switch (ogl.renderMode) {
        case FILAIRE_STPC: {
            TracerFilaireSTPC();
            }
            break;
        case FILAIRE_UNIE_ATPC:{
            TracerFilaireUnieATPC();
            }
            break;
        case SOLIDE_DEG_ATPC:{
            TracerSolideDegATPC();
            }
            break;
        case SOLIDE_FILAIRE_ATPC:{
            TracerSolideFilaireAPTC();
            }
            break;
        case OMBRAGE_CONST:{
            TracerOmbrageConst();
            }
            break;
        case OMBRAGE_CONST_FILAIRE:{
            TracerOmbrageConstFilaire();
            }
            break;
        case OMBRAGE_FILAIRE_CONST:{
            TracerOmbrageFilaireConst();
            }
            break;
        case OMBRAGE_PHONG:{
            TracerOmbragePhong();
            }
            break;
        case OMBRAGE_PHONG_FILAIRE:{
            TracerOmbragePhongFilaire();
            }
            break;
        case OMBRAGE_FILAIRE_PHONG:{
            TracerOmbrageFilairePhong();
            }
            break;
    }
}
