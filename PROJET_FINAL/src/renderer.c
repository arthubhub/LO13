
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
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
 * @brief Définit la couleur OpenGL en fonction de la courbure `cur`.
 */

void SetColorFromCurvature(float cur)
{
    float ratio, r, g, b;

    if (cur > 0.0f) {
        ratio = cur / msh.curvature_max;
        // Clamp au cas où
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        r = 0.0f;
        g = ratio ;
        b = 1.0f - ratio;

        /*if (ratio >= 0.9f) {
            printf("[DEBUG] POS_HIGH: cur=%.6f, max=%.6f, ratio=%.6f -> color=(R:%.3f G:%.3f B:%.3f)\n",
                   cur, msh.curvature_max, ratio, r, g, b);
        }*/
    }
    else {
        ratio = (-cur) / fabsf(msh.curvature_min);
        // Clamp
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        r = ratio;
        g = 0.0f;
        b = 1.0f - ratio;

        /*if (ratio >= 0.9f) {
            printf("[DEBUG] NEG_HIGH: cur=%.6f, min=%.6f, ratio=%.6f -> color=(R:%.3f G:%.3f B:%.3f)\n",
                   cur, msh.curvature_min, ratio, r, g, b);
        }*/
    }

    /*printf("[DEBUG] cur=%.6f, ratio=%.6f -> color=(R:%.3f G:%.3f B:%.3f)\n",
           cur, ratio, r, g, b);*/

    glColor3f(r, g, b);
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
            msh.normal_v[real_base]    * ogl.normales_Current_Factor[0], // normale x
            msh.normal_v[real_base + 1]* ogl.normales_Current_Factor[1], // normale y
            msh.normal_v[real_base + 2]* ogl.normales_Current_Factor[2]  // normale z
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

void TracerTriangleSubdiviseUnique(int k) {
    int v0 = msh.triangles[k + 0];
    int v1 = msh.triangles[k + 1];
    int v2 = msh.triangles[k + 2];
    
    //  shrink -> comme dans tracer triangle unique
    int shrink_state = (ogl.shrink > 0.01f) ? 1 : 0;
    float Gx = 0.0f, Gy = 0.0f, Gz = 0.0f;
    if (shrink_state) {
        CalculerBarycentre(k, &Gx, &Gy, &Gz);
    }
    
    // récupération des points selon shrink/pas shrink
    float p0[3], p1[3], p2[3];
    if (shrink_state) {
        p0[0] = CalculerShrink(msh.vertices[3*v0], Gx);
        p0[1] = CalculerShrink(msh.vertices[3*v0 + 1], Gy);
        p0[2] = CalculerShrink(msh.vertices[3*v0 + 2], Gz);
        
        p1[0] = CalculerShrink(msh.vertices[3*v1], Gx);
        p1[1] = CalculerShrink(msh.vertices[3*v1 + 1], Gy);
        p1[2] = CalculerShrink(msh.vertices[3*v1 + 2], Gz);
        
        p2[0] = CalculerShrink(msh.vertices[3*v2], Gx);
        p2[1] = CalculerShrink(msh.vertices[3*v2 + 1], Gy);
        p2[2] = CalculerShrink(msh.vertices[3*v2 + 2], Gz);
    } else {
        p0[0] = msh.vertices[3*v0];
        p0[1] = msh.vertices[3*v0 + 1];
        p0[2] = msh.vertices[3*v0 + 2];
        
        p1[0] = msh.vertices[3*v1];
        p1[1] = msh.vertices[3*v1 + 1];
        p1[2] = msh.vertices[3*v1 + 2];
        
        p2[0] = msh.vertices[3*v2];
        p2[1] = msh.vertices[3*v2 + 1];
        p2[2] = msh.vertices[3*v2 + 2];
    }
    
    // récupération des curvatues aux sommets
    float cur0 = msh.curvature_v[v0];
    float cur1 = msh.curvature_v[v1];
    float cur2 = msh.curvature_v[v2];
    
    // milieux pour créer les 4 sous-triangles
    float m01[3] = {0.5f * (p0[0] + p1[0]), 0.5f * (p0[1] + p1[1]), 0.5f * (p0[2] + p1[2])};
    float m12[3] = {0.5f * (p1[0] + p2[0]), 0.5f * (p1[1] + p2[1]), 0.5f * (p1[2] + p2[2])};
    float m02[3] = {0.5f * (p0[0] + p2[0]), 0.5f * (p0[1] + p2[1]), 0.5f * (p0[2] + p2[2])};
    
    // Sous triangle sommet 0
    SetColorFromCurvature(cur0);
    glVertex3f(p0[0], p0[1], p0[2]);
    glVertex3f(m01[0], m01[1], m01[2]);
    glVertex3f(m02[0], m02[1], m02[2]);
    
    // Sous triangle sommet 1
    SetColorFromCurvature(cur1);
    glVertex3f(p1[0], p1[1], p1[2]);
    glVertex3f(m12[0], m12[1], m12[2]);
    glVertex3f(m01[0], m01[1], m01[2]);
    
    // Sous triangle sommet 2
    SetColorFromCurvature(cur2);
    glVertex3f(p2[0], p2[1], p2[2]);
    glVertex3f(m02[0], m02[1], m02[2]);
    glVertex3f(m12[0], m12[1], m12[2]);
    
    // Sous triangle milieu
    float curMoyenne = (cur0 + cur1 + cur2) / 3.0f;
    SetColorFromCurvature(curMoyenne);
    glVertex3f(m01[0], m01[1], m01[2]);
    glVertex3f(m12[0], m12[1], m12[2]);
    glVertex3f(m02[0], m02[1], m02[2]);
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
void TracerPointsDegGauss(void) {
    // 1) Dénominateurs pour la normalisation
    float posDenom = (msh.curvature_max > 0.0f)
                     ? msh.curvature_max : 1.0f;
    float negDenom = (msh.curvature_min < 0.0f)
                     ? -msh.curvature_min : 1.0f;

    // 2) Taille et style des points
    glPointSize(2.0f);
    glEnable(GL_POINT_SMOOTH);  // optionnel pour adoucir

    // 3) Parcours de tous les sommets
    glBegin(GL_POINTS);
    for (int i = 0; i < msh.number_of_vertices; ++i) {
        float cur   = msh.curvature_v[i];
        float ratio, r, g, b;

        if (cur > 0.0f) {
            // elliptique : rouge ← bleu  (r augmente, b diminue)
            ratio = cur / posDenom;
            ratio = fminf(fmaxf(ratio, 0.0f), 1.0f);
            r = ratio;
            g = 0.0f;
            b = 1.0f - ratio;
        }
        else {
            // hyperbolique : vert ← bleu (g diminue, b augmente)
            ratio = (-cur) / negDenom;
            ratio = fminf(fmaxf(ratio, 0.0f), 1.0f);
            r = 0.0f;
            g = 1.0f - ratio;
            b = ratio;
        }

        glColor3f(r, g, b);
        // 4) Position du point = coordonnées du sommet i
        glVertex3f(
            msh.vertices[3*i + 0],
            msh.vertices[3*i + 1],
            msh.vertices[3*i + 2]
        );
    }
    glEnd();
}

void TracerTrianglesDegGauss(void) {
    float posDenom = (msh.curvature_max > 0.0f)
                     ? msh.curvature_max : 1.0f;
    float negDenom = (msh.curvature_min < 0.0f)
                     ? -msh.curvature_min : 1.0f;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < msh.number_of_triangles; i++) {
        int k = 3*i;
        int v0 = msh.triangles[k],
            v1 = msh.triangles[k+1],
            v2 = msh.triangles[k+2];

        float cur = (msh.curvature_v[v0]
                   + msh.curvature_v[v1]
                   + msh.curvature_v[v2]) /3.0f;
        float ratio;
        float r, g, b;
        if (cur > 0.0f) {
            ratio = cur / posDenom;
            // clamp entre 0 et 1
            if (ratio < 0.0f) ratio = 0.0f;
            else if (ratio > 1.0f) ratio = 1.0f;

            r = 0.0f;
            g =  ratio;
            b = 1.0f - ratio;
            
        }
        else {
            ratio = (-cur) / negDenom;
            if (ratio < 0.0f) ratio = 0.0f;
            else if (ratio > 1.0f) ratio = 1.0f;
            r = ratio;
            g = 0.0f;
            b = 1.0f - ratio;   
            
        }
        //printf("ratio : %f\n",ratio);
        glColor3f(r, g, b);
        TracerTriangleUnique(k);
    }
    glEnd();
}




/**
 * @brief Subdivision en 4 de chaque triangle, couleur uniforme par sous-triangle.
 */
void TracerTrianglesSubdivises(void)
{
    int i, k;
    
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        k = 3 * i;
        TracerTriangleSubdiviseUnique(k);
    }
    glEnd();
}


void TracerTrianglesCarreauxClassique(void) {
    int i, k;
    float r,g,b;
    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        int carreau_id     = msh.carreaux[i];
        int couleur_index  = carreau_id % msh.nb_couleurs_carreaux;
        int base = couleur_index * 3;
        //printf("Triangle %d, carreau = %d\n",i,carreau_id);

        r = msh.couleurs_carreaux[base + 0];
        g = msh.couleurs_carreaux[base + 1];
        b = msh.couleurs_carreaux[base + 2];

        glColor3f(r, g, b); 
        k = 3 * i;
        TracerTriangleUnique(k);
    }
    glEnd();
}

void TracerTrianglesCarreauxFun(void) {
    int i, k;
    int tileId, current, nTiles;
    float r, g, b;
    
    current = ogl.current_carreau;
    nTiles  = msh.nb_carreaux;
    //printf("ntiles = %d\n",nTiles);
    //printf("Current : %d\n",current);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        tileId = msh.carreaux[i]; 
        int distMax = 20;
        int d1 = abs(tileId - current + nTiles) % nTiles;
        int d2 = abs(current - tileId + nTiles) % nTiles;
        int dist = (d1 < d2) ? d1 : d2; // on récupère la distance min
        if (dist <= distMax) {
            float t = 1.0f - (float)dist / (float)distMax;
            r = t;
            g = 0.0f;
            b = 1.0f - t;
        }
        else {
            r = 0.0f;
            g = 0.0f;
            b = 1.0f;
        }

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
        //printf("facteurs de normales chargés : \nx = %f\ny = %f\nz = %f\n",ogl.normales_Current_Factor[0],ogl.normales_Current_Factor[1],ogl.normales_Current_Factor[2]);
        normale_x=msh.normal_t[k]  * ogl.normales_Current_Factor[0];
        normale_y=msh.normal_t[k+1]* ogl.normales_Current_Factor[1];
        normale_z=msh.normal_t[k+2]* ogl.normales_Current_Factor[2];
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
    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.number_of_triangles; i++) {
        k = 3 * i;
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

void TracerUnie(float R, float G, float B){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(R, G, B);
            TracerTrianglesBasique();  // Tracé en mode remplissage
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
    
        DecalageArriereActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
            
            TracerTrianglesBasique();  // Tracé en mode remplissage
        DecalageArriereDesactivation();
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
        TracerTrianglesBasique();
        //TracerTrianglesBasique();  // Tracé en mode contour
    
}


void TracerSolideDegATPC(void){
    // Zbuffer -> oui
    // mode remplissage 
    // choisir couleur de début et de fin
    // draw triangles mode degradé
    // Zbuffer -> non
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    TracerTrianglesDegLineaire(); // Tracé en mode dégradé
    
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

    
    glColor3f(ogl.fillColorR, ogl.fillColorG, ogl.fillColorB);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        DecalageArriereActivation();
            TracerTrianglesBasique();  // Tracé en mode remplissage 
        DecalageArriereDesactivation();

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(ogl.penColorR, ogl.penColorG, ogl.penColorB);
        TracerTrianglesBasique();  // Tracé en mode contour
    
}

void TracerOmbrageConst(void){
    
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        /* ombrage */
        ActivationSource() ;
        AffectationMateriau() ;
        glShadeModel(GL_FLAT) ; /* mode ombrage constant */
            TracerTrianglesOmbrageConstant(); 
        DesactivationSource() ;

    
}

void TracerOmbrageConstFilaire(void){
    
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
    
}

void TracerOmbrageFilaireConst(void){
    
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
    
}
void TracerOmbragePhong(void){
    
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        /* ombrage */
        ActivationSource() ;
        AffectationMateriau() ;
        glShadeModel(GL_SMOOTH) ; /* mode ombrage de Phong */
            TracerTrianglesPhong(); 
        DesactivationSource() ;

    
}
void TracerOmbragePhongFilaire(void){
    
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
    
}
void TracerOmbrageFilairePhong(void){
    
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
    
}

void TracerGaussCurvature(void){

        DecalageArriereActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            TracerTrianglesSubdivises();  // Tracé en mode remplissage
            //TracerTrianglesDegGauss();
        DecalageArriereDesactivation();

    
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0.2f, 0.2f, 0.9f);
        TracerTrianglesBasique();


}


void TracerCarrauxClassique(void){

        DecalageArriereActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            TracerTrianglesCarreauxClassique();  // Tracé en mode carreaux
        DecalageArriereDesactivation();
        /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(ogl.penColorR,ogl.penColorG, ogl.penColorB);
        TracerTrianglesBasique();*/

}

void TracerCarreauxFun(void){

    DecalageArriereActivation();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        TracerTrianglesCarreauxFun();  // Tracé en mode carreaux fun
    DecalageArriereDesactivation();


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(ogl.penColorR,ogl.penColorG, ogl.penColorB);
    TracerTrianglesBasique();


}


float CalculerId(int vertex_index) {
    int real_base = 3 * vertex_index;
    float nx = msh.normal_v[real_base]   ;
    float ny = msh.normal_v[real_base+1] ;
    float nz = msh.normal_v[real_base+2] ;

    

    return nx*ogl.source_normalisee[0]
        + ny*ogl.source_normalisee[1]
        + nz*ogl.source_normalisee[2];
}

void TracerLigneCible(float Id_cible) {
    float Points[9];
    int nb_points;

    // Pour chaque triangle
    for (int i = 0; i < msh.number_of_triangles; i++) {
        int k = 3 * i;

        // indices virtuels des 3 sommets
        int triangle_a = msh.triangles[k + 0];
        int triangle_b = msh.triangles[k + 1];
        int triangle_c = msh.triangles[k + 2];
        /*printf("triangle_a : %d\n",triangle_a);
        printf("triangle_b : %d\n",triangle_b);
        printf("triangle_c : %d\n",triangle_c);*/

        // pointeurs vers les positions x,y,z de chaque sommet
        float *Pa = &msh.vertices[3 * triangle_a];
        float *Pb = &msh.vertices[3 * triangle_b];
        float *Pc = &msh.vertices[3 * triangle_c];

        /*printf("Pa : %p\n",Pa);
        printf("Pb : %p\n",Pb);
        printf("Pc : %p\n",Pc);*/

        // calcul des Id diffusés aux sommets
        float Id_a = CalculerId( msh.triangles[k+0] );
        float Id_b = CalculerId( msh.triangles[k+1] );
        float Id_c = CalculerId( msh.triangles[k+2] );

        /*printf("Id_a : %f\n",Id_a);
        printf("Id_b : %f\n",Id_b);
        printf("Id_c : %f\n",Id_c);*/

        // on convertit en V = Id/Id_cible - 1 pour checker le signe
        float Va = Id_a - Id_cible;
        float Vb = Id_b - Id_cible;
        float Vc = Id_c - Id_cible;
        /*printf("Va : %f\n",Va);
        printf("Vb : %f\n",Vb);
        printf("Vc : %f\n",Vc);*/


        nb_points = 0;

        // arête (a,b)
        if (Va * Vb < 0.0f) {
            if (Va < 0) {
                float t = - Va / (Vb - Va);
                Points[3*nb_points + 0] = Pa[0] + t * (Pb[0] - Pa[0]); // (1-t)*S0 + t*S1 = Pa + t*(Pb-Pa)
                Points[3*nb_points + 1] = Pa[1] + t * (Pb[1] - Pa[1]);
                Points[3*nb_points + 2] = Pa[2] + t * (Pb[2] - Pa[2]);
            }
            else {
                float t = Va / (Va - Vb);
                Points[3*nb_points + 0] = Pa[0] + t * (Pb[0] - Pa[0]); // (1-t)*S0 + t*S1 = Pa + t*(Pb-Pa)
                Points[3*nb_points + 1] = Pa[1] + t * (Pb[1] - Pa[1]);
                Points[3*nb_points + 2] = Pa[2] + t * (Pb[2] - Pa[2]);
            }
            nb_points++;
        }

        // arête (b,c)
        if (Vb * Vc < 0.0f) {
            if (Vb < 0) {
                float t = - Vb / (Vc - Vb);
                Points[3*nb_points + 0] = Pb[0] + t * (Pc[0] - Pb[0]);
                Points[3*nb_points + 1] = Pb[1] + t * (Pc[1] - Pb[1]);
                Points[3*nb_points + 2] = Pb[2] + t * (Pc[2] - Pb[2]);
            }
            else {
                float t = Vb / (Vb - Vc);
                Points[3*nb_points + 0] = Pb[0] + t * (Pc[0] - Pb[0]);
                Points[3*nb_points + 1] = Pb[1] + t * (Pc[1] - Pb[1]);
                Points[3*nb_points + 2] = Pb[2] + t * (Pc[2] - Pb[2]);
            }
            nb_points++;

        }

        // arête (c,a)
        if (Vc * Va < 0.0f) {
            if (Va < 0) {
                float t = - Va / (Vc - Va);
                Points[3*nb_points + 0] = Pa[0] + t * (Pc[0] - Pa[0]);
                Points[3*nb_points + 1] = Pa[1] + t * (Pc[1] - Pa[1]);
                Points[3*nb_points + 2] = Pa[2] + t * (Pc[2] - Pa[2]);
            }
            else {
                float t = Va / (Va - Vc);
                Points[3*nb_points + 0] = Pa[0] + t * (Pc[0] - Pa[0]);
                Points[3*nb_points + 1] = Pa[1] + t * (Pc[1] - Pa[1]);
                Points[3*nb_points + 2] = Pa[2] + t * (Pc[2] - Pa[2]);
            }
            nb_points++;
        }
        
        // dessin des segments
        if (nb_points == 2) {
            glVertex3f(Points[0], Points[1], Points[2]);
            glVertex3f(Points[3], Points[4], Points[5]);
        }
        else if (nb_points == 3) {
            // si les deux premiers points ne sont pas les memes, on les dessine
            if ((Points[0] != Points[3 + 0]) || (Points[1] != Points[3 + 1]) || (Points[2] != Points[3 + 2])){
                glVertex3f(Points[0], Points[1], Points[2]);
                glVertex3f(Points[3], Points[4], Points[5]);
            }
            //sinon on dessine le 1 et le 3
            else {                
                glVertex3f(Points[0], Points[1], Points[2]);
                glVertex3f(Points[6], Points[7], Points[8]);
            }

            printf(" NB de points = 3\n");
        }
        //printf("NB points : %d\n",nb_points);
        
        // sinon nb_points < 2 => pas de segment
    }
}


void TracerDiffusion(void){
    float Id_cible, r, g, b, t;

    DecalageArriereActivation();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(ogl.bgColorR, ogl.bgColorG, ogl.bgColorB);
            
            TracerTrianglesBasique();  // Tracé en mode remplissage
    DecalageArriereDesactivation();


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_LINES);

    for (int i = - ogl.nb_lignes_diffusion ; i <= ogl.nb_lignes_diffusion; ++i) {
        t = (float)i / (float)ogl.nb_lignes_diffusion; // normalisé entre 0 et 1
        Id_cible = t;

        r = 0.5f + 0.5f * sinf(M_PI * t + 0.0f);
        g = 0.5f + 0.5f * sinf(M_PI * t + 2.0f * M_PI / 3.0f);
        b = 0.5f + 0.5f * sinf(M_PI * t + 4.0f * M_PI / 3.0f);

        glColor3f(r, g, b);
        TracerLigneCible(Id_cible);
    }

    glEnd();
    

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
            //printf("Plans : t = %f\n",t);
            
        }
    }
    glEnd();


}

void TracerPlans(void){
    glPushMatrix();
    {
        MatriceVuePlan();
        DessinerPlans();
    }
    glPopMatrix();
}

void TracerProjection(const float* projectionMatrix, const char* matrixName, 
                      float red, float green, float blue) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        MatriceVueProjection(projectionMatrix, matrixName);
        TracerUnie(red, green, blue);
    glPopMatrix();
}

void TracerMiroir(const float* matriceMiroir, const char* matrixName) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        MatriceVueProjection(matriceMiroir, matrixName);
        //TracerUnie(0.5,0.5,0.5);
        TracerObjet();
    glPopMatrix();
}


void DessinerCadres(void) {
    DecalageArriereActivation();
        // On reste en mode "plein", et on choisit la couleur du grand carré (anneau).
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(ogl.bgColorR,ogl.bgColorG,ogl.bgColorB);

        // 1) Plan YZ (X = 0)
        glBegin(GL_TRIANGLES);
            // Bande du BAS
            glVertex3f(0.0f, -5.0f, -5.0f);
            glVertex3f(0.0f, -5.0f,  5.0f);
            glVertex3f(0.0f, 0.0f,  5.0f);

            glVertex3f(0.0f, -5.0f, -5.0f);
            glVertex3f(0.0f, 0.0f,  5.0f);
            glVertex3f(0.0f, 0.0f, -5.0f);

            // Bande du HAUT
            glVertex3f(0.0f,  1.0f, -5.0f);
            glVertex3f(0.0f,  1.0f,  5.0f);
            glVertex3f(0.0f,  5.0f,  5.0f);

            glVertex3f(0.0f,  1.0f, -5.0f);
            glVertex3f(0.0f,  5.0f,  5.0f);
            glVertex3f(0.0f,  5.0f, -5.0f);

            // Bande GAUCHE
            glVertex3f(0.0f, 0.0f, -5.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f,  1.0f, 0.0f);

            glVertex3f(0.0f, 0.0f, -5.0f);
            glVertex3f(0.0f,  1.0f, 0.0f);
            glVertex3f(0.0f,  1.0f, -5.0f);

            // Bande DROITE
            glVertex3f(0.0f, 0.0f,  1.0f);
            glVertex3f(0.0f, 0.0f,  5.0f);
            glVertex3f(0.0f,  1.0f,  5.0f);

            glVertex3f(0.0f, 0.0f,  1.0f);
            glVertex3f(0.0f,  1.0f,  5.0f);
            glVertex3f(0.0f,  1.0f,  1.0f);
        glEnd();
        // 2) Plan XZ (Y = 0)
        glBegin(GL_TRIANGLES);
            // Bande du « bas »
            glVertex3f(-5.0f, 0.0f, -5.0f);
            glVertex3f( 5.0f, 0.0f, -5.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);

            glVertex3f(-5.0f, 0.0f, -5.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);
            glVertex3f(-5.0f, 0.0f, 0.0f);

            // Bande du « haut »
            glVertex3f(-5.0f, 0.0f,  1.0f);
            glVertex3f( 5.0f, 0.0f,  1.0f);
            glVertex3f( 5.0f, 0.0f,  5.0f);

            glVertex3f(-5.0f, 0.0f,  1.0f);
            glVertex3f( 5.0f, 0.0f,  5.0f);
            glVertex3f(-5.0f, 0.0f,  5.0f);

            // Bande GAUCHE
            glVertex3f(-5.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f,  1.0f);

            glVertex3f(-5.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f,  1.0f);
            glVertex3f(-5.0f, 0.0f,  1.0f);

            // Bande DROITE
            glVertex3f( 1.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f,  1.0f);

            glVertex3f( 1.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f,  1.0f);
            glVertex3f( 1.0f, 0.0f,  1.0f);
        glEnd();
        // 3) Plan XY (Z = 0)
        glBegin(GL_TRIANGLES);
            // Bande du BAS
            glVertex3f(-5.0f, -5.0f, 0.0f);
            glVertex3f( 5.0f, -5.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);

            glVertex3f(-5.0f, -5.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);
            glVertex3f(-5.0f, 0.0f, 0.0f);

            // Bande du HAUT 
            glVertex3f(-5.0f,  1.0f, 0.0f);
            glVertex3f( 5.0f,  1.0f, 0.0f);
            glVertex3f( 5.0f,  5.0f, 0.0f);

            glVertex3f(-5.0f,  1.0f, 0.0f);
            glVertex3f( 5.0f,  5.0f, 0.0f);
            glVertex3f(-5.0f,  5.0f, 0.0f);

            // Bande GAUCHE 
            glVertex3f(-5.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f,  1.0f, 0.0f);

            glVertex3f(-5.0f, 0.0f, 0.0f);
            glVertex3f(0.0f,  1.0f, 0.0f);
            glVertex3f(-5.0f,  1.0f, 0.0f);

            // Bande DROITE 
            glVertex3f( 1.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f,  1.0f, 0.0f);

            glVertex3f( 1.0f, 0.0f, 0.0f);
            glVertex3f( 5.0f,  1.0f, 0.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
    glEnd();
    DecalageArriereDesactivation();
    // Cadre autour du miroir
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(2.0f); 
    

    // Arêtes de connexion entre les faces
    glBegin(GL_LINES);
        // Depuis le coin (0,0,0) vers les 3 directions
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        
        // Depuis le coin (1,0,0) vers les 2 directions restantes
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        
        // Depuis le coin (0,1,0) vers les 2 directions restantes
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        
        // Depuis le coin (0,0,1) vers les 2 directions restantes
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 1.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glVertex3f(1.0f, 0.0f, 1.0f);
    glEnd();
    glLineWidth(1.0f);

}

void DessinerSol(void){
    float side, t;
    int i;
    int pas = 15; // >0, nb de carreaux de coté
    side = 5; // pour side = n, on va de -n+0.5 à n+0.5
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_LINES);
    {
        for (i = 0 ; i<=pas ; i++){
            t = -((float)side/2) + + (float)i/pas * side + 0.5f;
            //printf("%f -> %f (%f)\n",-(side/2)+0.5f,(side/2)+0.5f,t);

            glVertex3f(t, -(side/2)+0.5f, ogl.PlanOmbre_Z);
            glVertex3f(t, (side/2)+0.5f, ogl.PlanOmbre_Z);

            glVertex3f(-(side/2)+0.5f, t, ogl.PlanOmbre_Z);
            glVertex3f((side/2)+0.5f, t, ogl.PlanOmbre_Z);
            
        }
    }
    glEnd();



}


void DessinerFlecheVoyante(const float axis[3], float longueur, float tailleTete) {
    /*- 1) Point de la pointe P = longueur * axis- */
    float P[3];
    ScaleVector(axis, longueur, P);

    /*- 2) V = P - O (O = position de la caméra dans ogl)- */
    float O[3] = { ogl.obsX, ogl.obsY, ogl.obsZ };
    float V[3];
    SubtractVectors(P, O, V);

    /*- 3) V_perp = V - ( (V⋅axis) / ||axis||^2 ) * axis- */
    float Vperp[3];
    PerpComponent(V, axis, Vperp);

    /* Si Vperp est quasi-nul, on choisit un vecteur quelconque perpendiculaire à axis */
    const double EPS = 1e-6;
    if (fabsf(Vperp[0]) < EPS && fabsf(Vperp[1]) < EPS && fabsf(Vperp[2]) < EPS) {
        /* Par exemple, si axis ∥ X, on prend Y ; si axis ∥ Y, on prend X ; sinon X. */
        if (axis[0] == 1.0f && axis[1] == 0.0f && axis[2] == 0.0f) {
            Vperp[0] = 0.0f; Vperp[1] = 1.0f; Vperp[2] = 0.0f;
        } else if (axis[0] == 0.0f && axis[1] == 1.0f && axis[2] == 0.0f) {
            Vperp[0] = 1.0f; Vperp[1] = 0.0f; Vperp[2] = 0.0f;
        } else {
            Vperp[0] = 1.0f; Vperp[1] = 0.0f; Vperp[2] = 0.0f;
        }
    }

    /*- 4) U = (axis × Vperp) / ||axis × Vperp||- */
    float tmpCross[3];
    RawCrossProduct(axis, Vperp, tmpCross);
    double normCross = NormalizeVector(tmpCross);
    float U[3];
    if (normCross < EPS) {
        /* Cas dégénéré : Vperp ∥ axis (rare si on gère bien au-dessus) */
        /* On force U perpendiculairement à axis */
        if (axis[0] == 1.0f && axis[1] == 0.0f && axis[2] == 0.0f) {
            U[0] = 0.0f; U[1] = 1.0f; U[2] = 0.0f;
        } else if (axis[0] == 0.0f && axis[1] == 1.0f && axis[2] == 0.0f) {
            U[0] = 1.0f; U[1] = 0.0f; U[2] = 0.0f;
        } else {
            U[0] = 1.0f; U[1] = 0.0f; U[2] = 0.0f;
        }
    } else {
        /* tmpCross est déjà normalisé par NormalizeVector, on le recopie dans U */
        U[0] = tmpCross[0];
        U[1] = tmpCross[1];
        U[2] = tmpCross[2];
    }

    /*- 5) Centre de la base : B = P - s * axis- */
    float B[3];
    ScaleVector(axis, tailleTete, B);      /* B = axis * tailleTete */
    SubtractVectors(P, B, B);              /* B = P - (axis * tailleTete) */

    /*- 6) Sommets de la base : B1 = B + r*U,  B2 = B - r*U- */
    float r = tailleTete * 0.5f;
    float B1[3], B2[3], temp[3];
    ScaleVector(U,  r, temp);   /* temp = r * U */
    AddVectors(B, temp, B1);    /* B1 = B + r*U */
    ScaleVector(U, -r, temp);   /* temp = -r * U */
    AddVectors(B, temp, B2);    /* B2 = B - r*U */

    /* ========== Tracé OpenGL =========== */
    /* 1) Tige (shaft) */
    glBegin(GL_LINES);
      glVertex3f(0.0f, 0.0f, 0.0f);
      glVertex3f(P[0],  P[1],  P[2]);
    glEnd();

    /* 2) Pointe (triangle) */
    glBegin(GL_TRIANGLES);
      glVertex3f(P[0],   P[1],   P[2]);
      glVertex3f(B1[0],  B1[1],  B1[2]);
      glVertex3f(B2[0],  B2[1],  B2[2]);
    glEnd();
}

void DessinerRepere(void) {
    float longueurAxe = 1.5f;
    float tailleFleche = 0.05f;

    //glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    /* Axe X */
    glColor3f(1.0f, 0.0f, 0.0f);
    {
        float axisX[3] = {1.0f, 0.0f, 0.0f};
        DessinerFlecheVoyante(axisX, longueurAxe, tailleFleche);
    }
    /* Axe Y */
    glColor3f(0.0f, 1.0f, 0.0f);
    {
        float axisY[3] = {0.0f, 1.0f, 0.0f};
        DessinerFlecheVoyante(axisY, longueurAxe, tailleFleche);
    }
    /* Axe Z */
    glColor3f(0.0f, 0.0f, 1.0f);
    {
        float axisZ[3] = {0.0f, 0.0f, 1.0f};
        DessinerFlecheVoyante(axisZ, longueurAxe, tailleFleche);
    }

    glLineWidth(1.0f);
    //glEnable(GL_LIGHTING);
}

void TracerCadres(void){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        MatriceVuePlan();
        DessinerCadres();
    glPopMatrix();
}


void TracerProjections(void) {
    TracerProjection(ogl.Px, "ogl.Px", 1.0f, 0.6f, 0.6f);  // X projection - red 
    TracerProjection(ogl.Py, "ogl.Py", 0.6f, 1.0f, 0.6f);  // Y projection - green 
    TracerProjection(ogl.Pz, "ogl.Pz", 0.6f, 0.6f, 1.0f);  // Z projection - blue 
}


void TracerMiroirs(){
    ogl.normales_Current_Factor=ogl.normales_inv;
    TracerMiroir(ogl.Sx, "ogl.Sx");
    TracerMiroir(ogl.Sy, "ogl.Sy");
    TracerMiroir(ogl.Sz, "ogl.Sz");
}

void TracerSol(){
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        MatriceVuePlan();
        DessinerSol();
    glPopMatrix();

}

void TracerOmbre(){
    glPushMatrix();
        MatriceVueOmbre(ogl.Po, "ogl.Po");
        TracerUnie(0.2f, 0.2f, 0.2f);
    glPopMatrix();

}

void TracerProjOptions(){
    switch (ogl.proj_mode){
        case NONE:
            break;
        case PROJECTION:{
            TracerPlans();
            TracerProjections();
            break;
        }
        case MIROIRS:{
            TracerCadres();
            TracerMiroirs();
            break;
        }
        case OMBRE:{
            TracerSol();
            TracerOmbre();
            break;
        }
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
        case GAUSS_CURVATURE:{
            TracerGaussCurvature();
            }
            break;
        case CARREAUX_CLASSIQUE:{
            TracerCarrauxClassique();
            }
            break;

        case CARREAUX_FUN:{
            if (msh.nb_carreaux>5){
                TracerCarreauxFun();
                ogl.current_carreau = (ogl.current_carreau+1)+msh.nb_carreaux;
                glutPostRedisplay();
            }
            else {
                printf("### ERROR : Your mesh file doesnt contains information about tiles\n");

            }
        }
        break;
        case DIFFUSION:{
            TracerDiffusion();
        }
        break;
    }
}

void TracerObjetBasique(void){
    MatriceVueObjet();
    ogl.normales_Current_Factor=ogl.normales_basiques;
    TracerObjet();

}


void TracerRepere(void){
    if (ogl.repere){
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            MatriceVuePlan();
            DessinerRepere();
        glPopMatrix();
}
}
