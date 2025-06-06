#ifndef LIGHTING_H
#define LIGHTING_H

/**
 * @brief Defines a light source (position + ambient/diffuse/specular components).
 */
typedef struct {
    float position[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
} Source;

/**
 * @brief Defines a material (ambient/diffuse/specular + shininess).
 */
typedef struct {
    float ambient[4];
    float diffuse[4];
    float specular[4];
    float shininess;
} Material;

/* Initialize the global light source (ogl.src) */
void InitialiserSourceLumineuse(void);

void InitialiserOption4_Ombres(void);

/* Initialize predefined materials (ogl.plastic, ogl.copper, ogl.steel) */
void InitialiserMateriaux(void);

/* Enable lighting using the current source (ogl.src) */
void ActivationSource(void);

/* Disable lighting */
void DesactivationSource(void);

/* Apply the current material (ogl.current_mat) */
void AffectationMateriau(void);

#endif // LIGHTING_H