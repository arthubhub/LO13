#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>
#include <stdint.h>

/**
 * @brief Handle window resize events.
 */
void Reshape(int w, int h);

/**
 * @brief Handle keyboard events.
 */
void Keyboard(unsigned char key, int x, int y);

/**
 * @brief Handle mouse button events.
 */
void Mouse(int32_t button, int32_t state, int x, int y);

/**
 * @brief Handle mouse motion when a button is pressed.
 */
void Motion(int x, int y);

/**
 * @brief Register all GLUT callbacks for input and window events.
 */
void EvenementsGraphiques(void);

void Display(void);

#endif // INPUT_H
