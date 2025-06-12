#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "opengl_state.h"

void PrintMatrix4x4(const char *name, const float M[16]);

/* Z-buffer control */
void ZbufferActivation(void);
void ZbufferDesactivation(void);

/* Polygon offset controls */
void DecalageAvantActivation(void);
void DecalageAvantDesactivation(void);
void DecalageArriereActivation(void);
void DecalageArriereDesactivation(void);

/* Low-level triangle renderers */
void TracerTriangleUnique(int k);
void TracerTriangleUniqueShrink(int k);
void TracerTriangleUniquePhong(int k);

/* High-level triangle loops */
void TracerTrianglesBasique(void);
//void TracerTrianglesShrink(void); //<- ancienne fonction pour le shrink
void TracerTrianglesDegLineaire(void);
void TracerTrianglesOmbrageConstant(void);
void TracerTrianglesPhong(void);
void TracerTrianglesCarreauxClassique(void);

void TracerTrianglesDegGauss(void);
void TracerPointsDegGauss(void);
void TracerGaussCurvature(void);
void TracerCarrauxClassique(void);
void TracerTrianglesSubdivises(void);
void SetColorFromCurvature(float cur, float posDenom, float negDenom);

//modes de tracé
void TracerUnie(float R, float G, float B);
void TracerFilaireSTPC(void);
void TracerFilaireUnieATPC(void);
void TracerSolideDegATPC(void);
void TracerSolideFilaireAPTC(void);

void TracerOmbrageConst(void);
void TracerOmbrageConstFilaire(void);
void TracerOmbrageFilaireConst(void);

void TracerOmbragePhong(void);
void TracerOmbragePhongFilaire(void);
void TracerOmbrageFilairePhong(void);

void DessinerPlans(void);
void DessinerSol(void);

void DessinerFlecheVoyante(const float axis[3], float longueur, float tailleTete);

void DessinerRepere(void);

/* Main drawing dispatch based on ogl.renderMode */
void TracerObjet(void);
void TracerPlans(void);

void TracerProjection(const float* projectionMatrix, const char* matrixName, 
                      float red, float green, float blue);
void TracerProjections(void);
void TracerMiroir(const float* matriceMiroir, const char* matrixName);
void TracerMiroirs(void);

void TracerSol(void);
void TracerOmbre(void);

void TracerProjOptions(void);

void DessinerCadres(void);
void TracerCadres(void);
void TracerObjetBasique(void);

void TracerRepere(void);

#endif // RENDERER_H