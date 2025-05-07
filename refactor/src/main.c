// src/main.c

#include <stdlib.h>
#include "app.h"
#include "mesh.h"
#include "lighting.h"
#include "transform.h"
#include "renderer.h"
#include "input.h"

int main(int argc, char **argv) {
    // Initialize GLUT
    InitialiserLibrairieGraphique(&argc, argv);

    // Set up all OpenGL parameters (window size, view, materials, etc.)
    InitialiserParametresGraphiques();

    // Load and prepare the model (mesh or cube)
    ModeleDiscret(&msh,argv);

    // Create the window and set up rendering context
    CreationFenetreGraphique();
    InitialiserEnvironnementGraphique();

    // Register all input and display callbacks
    EvenementsGraphiques();

    // Enter the GLUT main loop
    BoucleInfinie();

    return 0;
}
