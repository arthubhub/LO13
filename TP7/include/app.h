#ifndef APP_H
#define APP_H

/**
 * @brief  Init GLUT (must be called before anything else)
 */
void InitialiserLibrairieGraphique(int *argc, char **argv);

/**
 * @brief  Set up all ogl.* parameters (window size, view, materials, etc.)
 */
void InitialiserParametresGraphiques(void);

/**
 * @brief  Create the GLUT window and set display mode.
 */
void CreationFenetreGraphique(void);

void InitialiserWindow(void);
void InitialiserRepereVue(void);
void InitialiserCouleurs(void);
void InitialiserPerspectiveProj(void);
void InitialiserFacteursAnysotropie(void);
void InitilaiserZoom(void);
void InitialiserMatTransformGeometrique(void);
void InitialiserTransformations(void);
void InitialiserModeTrace(void);
void InitialiserOption1_Plan(void);
void InitialiserOption2_Projections(void);
void InitialiserOption3_Projections(void);
void InitialiserChangementRepere(void);


void ReinitialisationParamGraphiques(void);

/**
 * @brief  Init OpenGL state (clear color, projection matrix, etc.).
 */
void InitialiserEnvironnementGraphique(void);

/**
 * @brief  Enter the GLUT main loop.
 */
void BoucleInfinie(void);

#endif // APP_H
