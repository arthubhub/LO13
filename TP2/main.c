#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

#define NB_VERTICES_CUBE 8
#define NB_TRIANGLES_CUBE 12

/* Structures */
typedef struct mesh{
    uint8_t    nb_vertices;
    float       *vertices;
    
    uint8_t    nb_triangles;
    uint8_t    *triangles; 

} Mesh ;


/* Variables globales */
Mesh msh;



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





int main(int argc, uint8_t **argv){
    InitialiserLibrairieGraphique(&argc,argv); // -> glutInit
    InitialiserParametresGraphiques();
    ModeleDiscret();
    CreationFenetreGraphique();
    EvenementsGraphiques();
    BoucleInfinie();
    

    return 1;
}