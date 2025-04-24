#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define NB_VERTICES_CUBE 8
#define NB_TRIANGLES_CUBE 12

#define FZOOM 0.9

#define ROTATION_SENSIBILITY 0.2

#define TRANSLATION_STATE 1
#define ROTATION_STATE 2

/* Enums */

typedef enum {
    FILAIRE_STPC,
    FILAIRE_UNIE_ATPC,
    SOLIDE_DEG_ATPC,
    SOLIDE_FILAIRE_ATPC
} RenderMode;


/* Structures */
typedef struct mesh {
    int		number_of_vertices;		    /* nombre de sommets */
    float		*vertices;					/* liste des coordonnees des sommets*/
    float		*normal_v;					/* liste des normales aux sommets*/
    int		number_of_triangles;		/* nombre de triangles */
    int		*triangles;					/* liste des sommets des triangles */
    float		*alpha_t;					/* liste des angles aux sommets*/
    float		*normal_t;					/* liste des normales aux faces triangulaires*/
    float		ccenter[3];					/* coordonnees du centre de la boite englobante */
    float		cmin[3], cmax[3];			/* coordonnees min et max de la boite englobante */
    float		delta;						/* taille maximale de la boite englobante suivant les axes */
    long int		memory;						/* memoire allouee pour la structure mesh */
    int		error;						/* arret du programme si error est non nul */
} Mesh;

typedef struct opengl {

    /* window */
    int winSizeX, winSizeY;
    int winPosX, winPosY;


    /* repere de vue */
    float obsX, obsY, obsZ;
    float focalX, focalY, focalZ;
    float vertX, vertY, vertZ;

    /* couleurs */
    float bgColorR, bgColorG, bgColorB;
    float penColorR, penColorG, penColorB;
    float fillColorR, fillColorG, fillColorB;

    /* perspective de projection */
    float Umin, Umax;
    float Vmin, Vmax;
    float Dmin, Dmax;

    /* reshape */
    float fu, fv;
    float fzoom;

    /* rotations */
    int32_t sx0,sy0;       
    float rot_u, rot_v;

    /* Transformations */
    float * geometricTransformations; // Do not allocate memory directly in stack
    uint8_t flagTransformation;
    float focal_u, focal_v, focal_n;
    float trans_u, trans_v;


    /* mode de tracé actif */
    RenderMode renderMode;

} Opengl;


/* Variables globales */
Mesh msh;
Opengl ogl;






  /********** ------------------------------------------------------------------------------------ **********/
 /********** Procédures pour la lecture et la normalisation du maillage et le calcul des normales **********/
/********** ------------------------------------------------------------------------------------ **********/

double NormSquare(float vect[3])
{
    return((vect[0]*vect[0]) + (vect[1]*vect[1]) + (vect[2]*vect[2]));
}

double Norm(float vect[3])
{
    return(sqrt(NormSquare(vect)));
}


double DotProduct(float vect1[3], float vect2[3])
{
    return((vect1[0]*vect2[0]) + (vect1[1]*vect2[1]) + (vect1[2]*vect2[2]));
}


void CrossProduct(float vect1[3], float vect2[3], float *result)
{
    double        norme;

    result[0] = (vect1[1]*vect2[2]) - (vect1[2]*vect2[1]);
    result[1] = (vect1[2]*vect2[0]) - (vect1[0]*vect2[2]);
    result[2] = (vect1[0]*vect2[1]) - (vect1[1]*vect2[0]);
    norme = Norm(result); /*normalisation du produit vectoriel*/
    if (norme != 0.0)
    {
        result[0] = (float)(result[0]/norme);
        result[1] = (float)(result[1]/norme);
        result[2] = (float)(result[2]/norme);
    }
}


double Angle(float point1[3], float point2[3], float point3[3]) /* calcul l'angle entre les vecteurs (point2,point1) et (point2,point3) */
{
    double        angle, d;
    float         v1[3], v2[3];
    int           i;

    for (i=0; i<3; i++)
    {
        v1[i] = point1[i] - point2[i];
        v2[i] = point3[i] - point2[i];
    }
    d = DotProduct(v1, v2);
    if (d > 1.0)
        d = 1.0;
    else if (d < -1.0)
        d = -1.0;
    angle = acos(d / sqrt( NormSquare(v1) * NormSquare(v2)));
    return(angle);
}

void InitializeMesh(Mesh *msh)  /* Initialisation de la struture mesh */
{
    int		j;

    msh->number_of_vertices = 0;
    msh->number_of_triangles = 0;
    msh->vertices = NULL;
    msh->triangles = NULL;
    for (j=0; j<3; j++)
    {
        msh->ccenter[j] = 0.0;
        msh->cmin[j] = 0.0;
        msh->cmax[j] = 0.0;
    }
    msh->delta = 0.0;
    msh->memory = 0;
    msh->error = 0;
}




void ReadMesh(Mesh *msh) /* lecture du fichier au format mesh INRIA Gamma3*/
{
    FILE		*file;
    static char file_name[] = "cube.mesh\0";
    char keyword[80];
    int i, j, ii, dim;
    if ((file = fopen(file_name, "r")) == NULL)
    {
        printf("error: file %s not found\n", file_name);
        msh->error = 100;
        return;
    }
    for (;;)
    {
        fscanf(file, "%s", keyword);
        if (strcmp(keyword, "EOF") == 0 || strcmp(keyword, "End") == 0 || strcmp(keyword, "end") == 0) /* fin du fichier */
            break;
        else if (strcmp(keyword, "Dimension") == 0 || strcmp(keyword, "dimension") == 0 ) /* mot cle dimension */
            fscanf(file, "%d", &dim);
        else if (strcmp(keyword, "Vertices") == 0 || strcmp(keyword, "vertices") == 0 )  /* mot cle Vertices */
        {
            fscanf(file, "%d", &(msh->number_of_vertices));
            msh->vertices = (float*)malloc((3*msh->number_of_vertices)*sizeof(float));
            if (!msh->vertices)
            {
                printf("error: not enough memory for %d vertices (%ld bytes)\n", msh->number_of_vertices, 3*msh->number_of_vertices*sizeof(float));
                msh->error = 200;
                return;
            }
            else
                msh->memory += (3*msh->number_of_vertices)*sizeof(float);
            if (dim == 2)
            {
                for (i=0; i<msh->number_of_vertices; i++)
                {
                    ii = 3 * i;
                    fscanf(file, "%f %f %d", &(msh->vertices[ii]), &(msh->vertices[ii+1]), &j);
                    msh->vertices[ii+2] = 0.0;
                }
            } else if (dim == 3)
            {
                for (i=0; i<msh->number_of_vertices; i++)
                {
                    ii = 3 * i;
                    fscanf(file, "%f %f %f %d", &(msh->vertices[ii]), &(msh->vertices[ii+1]), &(msh->vertices[ii+2]), &j);
                }
            }
        } else if (strcmp(keyword, "Triangles") == 0 || strcmp(keyword, "triangles") == 0) /* mot cle Triangles */
        {
            fscanf(file, "%d", &(msh->number_of_triangles));
            msh->triangles = (int*)malloc((3*msh->number_of_triangles)*sizeof(int));
            if (!msh->triangles)
            {
                printf("error: not enough memory for %d triangles (%ld bytes)\n", msh->number_of_triangles, 3*msh->number_of_triangles*sizeof(int));
                msh->error = 200;
                return;
            }
            else
                msh->memory += (3*msh->number_of_triangles)*sizeof(int);
            for (i=0; i<msh->number_of_triangles; i++)
            {
                ii = 3 * i;
                fscanf(file, "%d %d %d %d", &(msh->triangles[ii]), &(msh->triangles[ii+1]), &(msh->triangles[ii+2]), &j);
                for (j=0; j<3; j++)
                    msh->triangles[ii+j]--;
            }
        }
    }
    fclose(file);
    /* affichage a l'ecran des infos sur le maillage */
    printf("mesh : %d vertices -- %d triangles (%ld kbytes)\n", msh->number_of_vertices, msh->number_of_triangles, msh->memory/1024);
}

  /********** ----------------------------------------------------------------- **********/
 /********** Procédures de l'étage 4, transformations et traçage des triangles **********/
/********** ----------------------------------------------------------------- **********/


void  ZbufferActivation (void) { 
    glEnable (GL_DEPTH_TEST) ; 
}

void  ZbufferDesactivation (void) { 
    glDisable (GL_DEPTH_TEST) ; 
}



void computeLastTransformation(){
    // On évalue la rotation
    switch ((ogl.flagTransformation & ROTATION_STATE) >> 0x1)
    {
    case 0x1: // On fait une rotation
        glTranslatef(ogl.focal_u,ogl.focal_v,ogl.focal_n);
        glRotatef(ogl.rot_u, -1.0, 0.0, 0.0) ; // U
        glRotatef(ogl.rot_v, 0.0, 1.0, 0.0) ; // V
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
        glTranslatef(ogl.trans_u, ogl.trans_v, 0.0) ; /* translation dans le plan de vue */
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

void DecalageAvantActivation(void)
{
    glPolygonOffset (1.0, 1.0) ;
    glEnable (GL_POLYGON_OFFSET_LINE) ; 
}
void DecalageAvantDesactivation(void)
{
    glDisable (GL_POLYGON_OFFSET_LINE) ;
}
void DecalageArriereActivation(void)
{
glPolygonOffset (1.0, 1.0) ;
glEnable (GL_POLYGON_OFFSET_FILL) ; 
}
void DecalageArriereDesactivation(void)
{
glDisable (GL_POLYGON_OFFSET_FILL) ;
}

/* niveau 3 */

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
                TracerTrianglesBasique();  // Tracé en mode contour
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
    }
}





    /*
    int i,j,k,virt_base,real_base;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (i=0; i < msh.number_of_triangles; i++){
        k = 3*i;
        for (j=0; j < 3; j++){
            virt_base = msh.triangles[k + j];
            real_base = 3 * virt_base;  // Pas de "-1", indexation à partir de 0
            glVertex3f(msh.vertices[real_base],     // x
                       msh.vertices[real_base + 1], // y
                       msh.vertices[real_base + 2]  // z
            );
        }
    }
    glEnd();

    */







void EffacerEcran(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
}
void MatriceVue(void)
{
    /* pointer la matrice courante sur l’instance GL_MODELVIEW */
    glMatrixMode(GL_MODELVIEW) ;
    /* Initialiser la matrice courante avec l’identité */
    glLoadIdentity() ;
    /* Appliquer la dernière transformation géométrique */
    computeLastTransformation();
    /* Appliquer toutes les transformations géométriques antécédentes */
    glMultMatrixf(ogl.geometricTransformations) ;
    /* Mettre à jour l’historique des transformations géométriques */
    glGetFloatv(GL_MODELVIEW_MATRIX, ogl.geometricTransformations) ;
    /* Appliquer le changement de repère */
    gluLookAt(ogl.obsX, ogl.obsY, ogl.obsZ,
        ogl.focalX, ogl.focalY, ogl.focalZ,
        ogl.vertX, ogl.vertY, ogl.vertZ);
}
void ViderMemoireEcran(void){
    glutSwapBuffers() ; // -> for double buffer
    //glFlush(); // -> for 1 buffer
}




/* niveau 2 */

void defineCube(void){
    int i;
    
    
    msh.number_of_vertices = NB_VERTICES_CUBE;
    msh.vertices    = (float*)malloc(msh.number_of_vertices*3*sizeof(float));
/*    
          4-----------5
         /|          /|
        7-----------6 |
        | |         | |
        | 0---------|-1
        |/          |/
        3-----------2---- X
*/
    for (i=0; i<msh.number_of_vertices*3;i++ ){
        msh.vertices[i]=0.0;
    }
    
    // Bottom
    // 0
    i=0;
    // 1
    i++;
    msh.vertices[3*i +1 ]=1.0;
    // 2
    i++;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i ]=1.0;
    // 3
    i++;
    msh.vertices[3*i  ]=1.0;
    // Top
    // 4
    i++;
    msh.vertices[3*i +2 ]=1.0;
    // 5
    i++;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i +2 ]=1.0;
    // 6
    i++;
    msh.vertices[3*i  ]=1.0;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i +2 ]=1.0;
    // 7
    i++;
    msh.vertices[3*i ]=1.0;
    msh.vertices[3*i +2 ]=1.0;

    msh.number_of_triangles = NB_TRIANGLES_CUBE;
    msh.triangles=(int*)malloc(msh.number_of_triangles*3*sizeof(int));

    
    // Face du bas
    i=0;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=1;
    msh.triangles[i*3 + 2]=2;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=3;
    // Face de gauche
    i++;
    msh.triangles[i*3 + 0]=7;
    msh.triangles[i*3 + 1]=4;
    msh.triangles[i*3 + 2]=0;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=3;
    msh.triangles[i*3 + 2]=7;
    // Face de droite
    i++;
    msh.triangles[i*3 + 0]=1;
    msh.triangles[i*3 + 1]=5;
    msh.triangles[i*3 + 2]=6;
    i++;
    msh.triangles[i*3 + 0]=6;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=1;
    // Face du haut
    i++;
    msh.triangles[i*3 + 0]=6;
    msh.triangles[i*3 + 1]=5;
    msh.triangles[i*3 + 2]=4;
    i++;
    msh.triangles[i*3 + 0]=4;
    msh.triangles[i*3 + 1]=7;
    msh.triangles[i*3 + 2]=6;
    // Face de devant
    i++;
    msh.triangles[i*3 + 0]=3;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=7;
    i++;
    msh.triangles[i*3 + 0]=7;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=6;
    // Face de derriere
    i++;
    msh.triangles[i*3 + 0]=1;
    msh.triangles[i*3 + 1]=0;
    msh.triangles[i*3 + 2]=5;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=4;
    msh.triangles[i*3 + 2]=5;
}
void MatriceProjection(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(ogl.Umin * ogl.fu * ogl.fzoom ,ogl.Umax * ogl.fu * ogl.fzoom,ogl.Vmin * ogl.fv * ogl.fzoom, ogl.Vmax * ogl.fv * ogl.fzoom,ogl.Dmin,ogl.Dmax);
}
void Display(void){
    EffacerEcran();
    MatriceVue(); // -> matricemode modelview , identité, lookat()
    TracerObjet(); // -> tracer les triangles
    ViderMemoireEcran(); // -> glflush 

}
void Reshape(int w, int h){
    if (w>h){

        ogl.fu = (float)w/h;
        ogl.fv = 1.0 ;

    }
    else {
        ogl.fv = (float)h/w;
        ogl.fu = 1.0 ; 
    }
    ogl.winSizeX=w;
    ogl.winSizeY=h;
    MatriceProjection();
    glViewport(0,0,w,h);


}
void Keyboard(unsigned char key, int x, int y){
    switch (key) {
        case 'q':
        case 'Q':
        case 27:
            exit(0);
            break;
        case 'z':
            ogl.fzoom*=FZOOM;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'Z':
            ogl.fzoom*=1/FZOOM;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '1':
            ogl.renderMode = FILAIRE_STPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '2':
            ogl.renderMode = FILAIRE_UNIE_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '3':
            ogl.renderMode = SOLIDE_DEG_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '4':
            ogl.renderMode = SOLIDE_FILAIRE_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        default:
            break;
    }
}

void Mouse(int32_t b, int32_t state, int32_t sx, int32_t sy) {
    if (b == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        printf("Clic gauche\n");
        ogl.flagTransformation |= ROTATION_STATE;
        ogl.sx0 = sx;
        ogl.sy0 = sy;
    } else {
        ogl.flagTransformation &= ~ROTATION_STATE; 
    }
    if (b == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        printf("Clic droit\n");
        ogl.flagTransformation |= TRANSLATION_STATE;
        ogl.sx0 = sx;
        ogl.sy0 = sy;
    } else {
        ogl.flagTransformation &= ~TRANSLATION_STATE; 
    }
}

void Motion(int32_t sx, int32_t sy){
    int32_t vx, vy;
    float q_u, q_v;
    float transl_u, transl_v;

    vx = sx - ogl.sx0;
    vy = ogl.sy0 - sy;

    if (ogl.flagTransformation & ROTATION_STATE) {
        ogl.rot_v = vx * ROTATION_SENSIBILITY;
        ogl.rot_u = vy * ROTATION_SENSIBILITY;
        glutPostRedisplay();
    }

    if (ogl.flagTransformation & TRANSLATION_STATE) {
        // 1. Règle de trois pour rapport déplacement souris
        q_u = (float)vx / ogl.winSizeX;
        q_v = (float)vy / ogl.winSizeY;

        // 2. Application au cadrage (plan de vue)
        transl_u = q_u * (ogl.Umax - ogl.Umin);
        transl_v = q_v * (ogl.Vmax - ogl.Vmin);

        // 3. Correction par Thalès
        transl_u *= fabs(ogl.focal_n) / ogl.Dmin;
        transl_v *= fabs(ogl.focal_n) / ogl.Dmin;

        // 4. Application du zoom et anisotropie
        ogl.trans_u = transl_u * ogl.fu * ogl.fzoom;
        ogl.trans_v = transl_v * ogl.fv * ogl.fzoom;

        glutPostRedisplay();
    }

    ogl.sx0 = sx;
    ogl.sy0 = sy;
}




/* niveau 1 */
void InitialiserLibrairieGraphique(int *argc, char **argv){
    glutInit(argc,argv);
}
void InitialiserParametresGraphiques(void){
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
    ogl.vertY=1.0;
    ogl.vertZ=0.0;

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
    ogl.Dmin = 1;
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

}
void ModeleDiscret(void){
    // Pour le cube :
    // defineCube();

    // Pour un modèle mesh :
    InitializeMesh(&msh);
    ReadMesh(&msh);
}
void CreationFenetreGraphique(void){
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH) ; // double pour aller plus vite et depth pour le z-buffer

    glutInitWindowSize(ogl.winSizeX,ogl.winSizeY);
    glutInitWindowPosition(ogl.winPosX, ogl.winPosY);
    glutCreateWindow("LO13 - Arthur");
}
void InitialiserEnvironnementGraphique(void){


    /* Couleur de fond */
    glClearColor(ogl.bgColorR,ogl.bgColorG,ogl.bgColorB,0.0);
    /* couleur courante*/
    glColor3f(ogl.penColorR,ogl.penColorG,ogl.penColorB);
    MatriceProjection();
}
void EvenementsGraphiques(void){
    glutDisplayFunc(Display);   // -> What to do for displaying
    glutReshapeFunc(Reshape);   // -> What to do for reshaping
    glutKeyboardFunc(Keyboard); // -> What to do on keyboard event
    glutMouseFunc(Mouse);       // What to do on mouse click events
    glutMotionFunc(Motion);     // What to do when moving the mouse
}
void BoucleInfinie(){
    glutMainLoop();
}

int main(int argc, char **argv){
    InitialiserLibrairieGraphique(&argc,argv); // -> glutInit
    InitialiserParametresGraphiques(); // -> variables de opengl
    ModeleDiscret(); // -> remplir le cube  
    CreationFenetreGraphique(); // créer la fenetre avec les parametres configurés précédemment
    InitialiserEnvironnementGraphique();
    EvenementsGraphiques();
    BoucleInfinie();
    
    return 1;
}