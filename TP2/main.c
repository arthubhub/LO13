#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

typedef struct s_mesh { // 5.2 Projection and view matrices
    uint32_t nVertices, nTriangles;
    float vertices[8][3]; // 8 sommets définis sur 3 coordonnées
    uint32_t triangles[12][3]; // 12 triangles de 3 sommets
} Cube;

Cube MY_CUBE;


typedef struct { // 5.2 Projection and view matrices
    double ox, oy, oz; // Position de la caméra
    double fx, fy, fz; // Focus
    double vx, vy, vz; // Up vector
} ViewMatrix;
typedef struct { // 5.2 Projection and view matrices
    double left, right;
    double bottom, top;
    double nearVal, farVal;
} ProjectionMatrix;


ViewMatrix getDefaultCamera() { // 5.2 Projection and view matrices
    ViewMatrix cam = { // utilisation de la syntaxe c99
        .ox = 2, .oy = 2, .oz = 2,    // Position de la caméra
        .fx = 0, .fy = 0, .fz = 0,    // Cible 
        .vx = 0, .vy = 1, .vz = 0     // "Up" vector
    };
    return cam;
}
ProjectionMatrix getDefaultProjection() { // 5.2 Projection and view matrices
    ProjectionMatrix proj = {
        .left = -1.0, .right = 1.0,
        .bottom = -1.0, .top = 1.0,
        .nearVal = 1.0, .farVal = 10.0
    };
    return proj;
}



void viewMatrix(){ // 5.2 Projection and view matrices
    ViewMatrix camera = getDefaultCamera();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        camera.ox, camera.oy, camera.oz,
        camera.fx, camera.fy, camera.fz,
        camera.vx, camera.vy, camera.vz
    );
}
void projectionMatrix(){ // 5.2 Projection and view matrices
    ProjectionMatrix projection = getDefaultProjection();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        projection.left, projection.right,
        projection.bottom, projection.top,
        projection.nearVal, projection.farVal
    );
}



void initCube(){

    MY_CUBE.nVertices = 8;
    MY_CUBE.nTriangles = 12;
/* Merci ChatGPT
 A = {0,0,0}
    E--------F
    /|       /|
   H--------G |
   | |      | |
   | A------|-B
   |/       |/
   D--------C
*/

    // On peut faire ca comme ca pour cette fois car le cube n'a que 8 coins
    // En général, on évitera de stocker des grands tableaux dans la stack
    // 8 points * 3 dimensions
    float points[] = {
        0, 0, 0, // A (0)
        0, 1, 0, // B (1)
        1, 1, 0, // C (2)
        1, 0, 0, // D (3)
        0, 0, 1, // E (4)
        0, 1, 1, // F (5)
        1, 1, 1, // G (6)
        1, 0, 1  // H (7)
    };
    for (uint8_t i = 0; i < 8; i++) {
        MY_CUBE.vertices[i][0] = points[i * 3 + 0];
        MY_CUBE.vertices[i][1] = points[i * 3 + 1];
        MY_CUBE.vertices[i][2] = points[i * 3 + 2];
    }

    // 12 triangles * 3 indices
    uint32_t triangles[] = {
        0, 1, 2,  // face avant
        2, 3, 0,
        3, 7, 2,  // face droite
        7, 6, 2,
        1, 2, 6,  // face haut
        6, 5, 1,
        4, 5, 6,  // face arrière
        6, 7, 4,
        0, 3, 7,  // face bas
        7, 4, 0,
        0, 1, 5,  // face gauche
        5, 4, 0
    };
    for (int i = 0; i < 12; i++) {
        MY_CUBE.triangles[i][0] = triangles[i * 3 + 0];
        MY_CUBE.triangles[i][1] = triangles[i * 3 + 1];
        MY_CUBE.triangles[i][2] = triangles[i * 3 + 2];
    }
}


void displayCube(){
    glClear(GL_COLOR_BUFFER_BIT);  // Clear the screen

    glColor3f(1, 0.5, 0.5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw only lines (wireframe)
    // For each Triangle
    for (uint8_t triangle = 0; triangle < MY_CUBE.nTriangles; triangle++) {
        glBegin(GL_TRIANGLES);
        // For each point of the Triangle
        for (uint8_t point_index = 0; point_index < 3; point_index++) {
            uint32_t vertex_index = MY_CUBE.triangles[triangle][point_index];
            float x = MY_CUBE.vertices[vertex_index][0];
            float y = MY_CUBE.vertices[vertex_index][1];
            float z = MY_CUBE.vertices[vertex_index][2];
            printf("Plotting point %d of triangle %d at (%f, %f, %f)...\n",
                   vertex_index, triangle, x, y, z);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glFlush();  
}


void initColors() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
}

void initOpengGL(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("TPGL");
    glutDisplayFunc(displayCube);
}

int main(int argc, char **argv) {
    initOpengGL(argc, argv);
    initCube();
    initColors();
    viewMatrix();
    projectionMatrix();
    glutMainLoop();
    return EXIT_SUCCESS;
}