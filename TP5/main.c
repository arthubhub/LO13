#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

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
typedef struct mesh{
    uint8_t    nb_vertices;
    float       *vertices;
    
    uint8_t    nb_triangles;
    uint8_t    *triangles; 

} Mesh ;

typedef struct opengl {

    /* window */
    uint16_t winSizeX, winSizeY;
    uint16_t winPosX, winPosY;


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



/* niveau 4 */

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
void TracerTriangleUnique(uint16_t k){ 
    uint16_t j,k,virt_base,real_base;
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
    uint16_t i,k;
    
    glBegin(GL_TRIANGLES);
    for (i=0; i < msh.nb_triangles; i++){
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
    uint16_t i, k;
    float t = 1 / (float)(msh.nb_triangles - 1);  // t de 0 à 1

    // Boucle sur tous les triangles
    glBegin(GL_TRIANGLES);
    for (i = 0; i < msh.nb_triangles; i++) {
        float r = 1.0f - t*i;  // Diminue le rouge
        float b = t*i;          // Augmente le bleu
        float g = 0.0f;       
        glColor3f(r, g, b); 
        k = 3 * i;
        TracerTriangleUnique(k);
    }
    glEnd();
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
            // draw triangles basic
            // avancer l'objet
            // mode contours avec ogl.pencolorR,G,B
            // draw triangles basic
            // reculer l'objet
            // Zbuffer -> non
            ZbufferActivation();




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
            TracerTrianglesDegLineaire();
            ZbufferDesactivation();
            }
            break;
        case SOLIDE_FILAIRE_ATPC:{
            // Zbuffer -> oui
            // mode remplissage avec ogl.fillColorR,G,B
            // draw triangles basic
            // avancer l'objet
            // mode contours avec ogl.pencolorR,G,B
            // draw triangles basic
            // reculer l'objet
            // Zbuffer -> non
            }
            break;
    }
}





    /*
    uint16_t i,j,k,virt_base,real_base;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    for (i=0; i < msh.nb_triangles; i++){
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
    uint8_t i;
    
    
    msh.nb_vertices = NB_VERTICES_CUBE;
    msh.vertices    = (float*)malloc(msh.nb_vertices*3*sizeof(float));
/*    
          4-----------5
         /|          /|
        7-----------6 |
        | |         | |
        | 0---------|-1
        |/          |/
        3-----------2---- X
*/
    for (i=0; i<msh.nb_vertices*3;i++ ){
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

    msh.nb_triangles = NB_TRIANGLES_CUBE;
    msh.triangles=(uint8_t*)malloc(msh.nb_triangles*3*sizeof(uint8_t));

    
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
}
void ModeleDiscret(void){
    defineCube();
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