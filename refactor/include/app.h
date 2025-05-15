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

/**
 * @brief  Init OpenGL state (clear color, projection matrix, etc.).
 */
void InitialiserEnvironnementGraphique(void);

/**
 * @brief  Enter the GLUT main loop.
 */
void BoucleInfinie(void);

#endif // APP_H
