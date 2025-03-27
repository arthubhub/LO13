#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

#define NB_VERTICES_CUBE 8
#define NB_TRIANGLES_CUBE 12

#define FZOOM 0.99

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

    /* perspective de projection */
    float Umin, Umax;
    float Vmin, Vmax;
    float Dmin, Dmax;

    /* reshape */
    float fu, fv;
    float fzoom;


} Opengl;


/* Variables globales */
Mesh msh;
Opengl ogl;

/* niveau 3 */

void TracerObjet(void){
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
}
void EffacerEcran(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void MatriceVue(void){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ogl.obsX,ogl.obsY,ogl.obsZ,
        ogl.focalX,ogl.focalY,ogl.focalZ,  
        ogl.vertX,ogl.vertY,ogl.vertZ
    ); 
}
void ViderMemoireEcran(void){
    glFlush();
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
        default:
            break;
    }


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
    ogl.obsX = 2.0;
    ogl.obsY = 1.6;
    ogl.obsZ = 1.4;

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

    /* perpective de projection*/
    ogl.Umin = -0.5;
    ogl.Umax = 0.5;
    ogl.Vmin = -0.5;
    ogl.Vmax = 0.5;
    ogl.Dmin = 1;
    ogl.Dmax = 10;

    /* zoom */

    ogl.fu = 1.0;
    ogl.fv = 1.0;
    ogl.fzoom = 1.0;

}
void ModeleDiscret(void){
    defineCube();
}
void CreationFenetreGraphique(void){
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
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
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
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