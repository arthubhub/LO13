#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "opengl_state.h"

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

//modes de tracé
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

/* Main drawing dispatch based on ogl.renderMode */
void TracerObjet(void);
void TracerPlans(void);

void TracerProjections(void);
void TracerProjectionY(void);

#endif // RENDERER_H