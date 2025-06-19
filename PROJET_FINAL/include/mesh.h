#ifndef MESH_H
#define MESH_H

#include <stdint.h>

/* Constants for the built-in cube model */
#define NB_VERTICES_CUBE   8
#define NB_TRIANGLES_CUBE 12

/**
 * @brief Discrete mesh representation.
 */
typedef struct mesh {
    int      number_of_vertices;     
    float   *vertices;     
    float   *normal_v;     
    int      number_of_triangles;    
    int     *triangles;     
    float   *alpha_t;     
    float   *normal_t;     
    float   *curvature_v;
    int     *carreaux;
    float   curvature_min;
    float   curvature_max;
    float    ccenter[3];    
    float    cmin[3], cmax[3];    
    float    delta;     
    long     memory;     
    int      error;    

    float *couleurs_carreaux;
    int nb_couleurs_carreaux;
    int nb_carreaux;
} Mesh;

extern Mesh msh;

/* Mesh setup & teardown */
void InitializeMesh(Mesh *msh);
void ReadMesh     (Mesh *msh, char **argv);
void NormalizeMesh(Mesh *msh);
void SetNormals   (Mesh *msh);

/* Fonctions définissant le cube en 3d */
void defineCube(void);

/* Choisir le modèle discret */
void ModeleDiscret(Mesh *msh, char **argv);

void SetCurvature(Mesh *msh);


void InitialiserCouleursCarreaux(Mesh *msh);
void LibererCouleursCarreaux(void);
void SetColorFromCarreau(int numero_carreau);


#endif // MESH_H
