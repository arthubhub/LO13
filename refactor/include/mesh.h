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
    float    ccenter[3];    
    float    cmin[3], cmax[3];    
    float    delta;     
    long     memory;     
    int      error;     
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

#endif // MESH_H
