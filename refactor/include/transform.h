#ifndef TRANSFORM_H
#define TRANSFORM_H

#define FZOOM 0.9
#define ROTATION_SENSIBILITY 0.2
#define TRANSLATION_STATE 1
#define ROTATION_STATE 2


/**
 * @brief Applies pending translation/rotation transforms to the GL_MODELVIEW matrix.
 */
void computeLastTransformation(void);

/**
 * @brief Sets up the model-view matrix (identity, transforms, lookAt).
 */
void MatriceVue(void);

/**
 * @brief Sets up the projection matrix using frustum and current zoom.
 */
void MatriceProjection(void);

/**
 * @brief Clears the color and depth buffers.
 */
void EffacerEcran(void);

/**
 * @brief Swaps buffers (or flushes) to present the rendered frame.
 */
void ViderMemoireEcran(void);

#endif // TRANSFORM_H