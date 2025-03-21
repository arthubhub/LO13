#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

typedef struct s_mesh {
    uint32_t nVertices, nTriangles;
    float vertices[8][3]; // 8 sommets définis sur 3 coordonnées
    uint32_t triangles[12][3]; // 12 triangles de 3 sommets
} Square;


void definePoint(float *points_location, float coordinates[3] ){
    points_location[0]=coordinates[0];
    points_location[1]=coordinates[1];
    points_location[2]=coordinates[2];
}
void defineTriangle(float *triangle_location,uint8_t index0,uint8_t index1,uint8_t index2){
    triangle_location[0]=(uint32_t)index0;
    triangle_location[1]=(uint32_t)index1;
    triangle_location[2]=(uint32_t)index2;

}

void initCube(Square *s_struct){

    s_struct -> nVertices = 8;
    s_struct -> nTriangles = 12;
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
//  type  array = {x,y,z}; -> Attention, ce n'est pas une bonne pratique de définir les variables comme ca ^^
    float A[]   = {0,0,0}; // -> offset = 0
    uint8_t A_id=0;
    float B[]   = {0,1,0}; // -> offset = 3
    uint8_t B_id=1;
    float C[]   = {1,1,0}; // -> offset = 6
    uint8_t C_id=2;
    float D[]   = {1,0,0}; // -> offset = 9
    uint8_t D_id=3;
    float E[]   = {0,0,1}; // -> offset = 12
    uint8_t E_id=4;
    float F[]   = {0,1,1}; // -> offset = 15
    uint8_t F_id=5;
    float G[]   = {1,1,1}; // -> offset = 18
    uint8_t G_id=6;
    float H[]   = {1,0,1}; // -> offset = 21
    uint8_t H_id=7;

    // sommets :

    uint8_t offset = 0;
    definePoint(&s_struct->vertices[offset * 3],A);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],B);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],C);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],D);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],E);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],F);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],G);
    offset++;
    definePoint(&s_struct->vertices[offset * 3],H);

    // Triangles :
    offset=0;
    defineTriangle(&s_struct->triangles[offset*3],A_id,B_id,C_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],C_id,D_id,A_id);
    offset++;

    defineTriangle(&s_struct->triangles[offset*3],D_id,H_id,C_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],H_id,G_id,C_id);

    offset++;
    defineTriangle(&s_struct->triangles[offset*3],B_id,C_id,G_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],G_id,F_id,B_id);

    offset++;
    defineTriangle(&s_struct->triangles[offset*3],E_id,F_id,G_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],G_id,H_id,E_id);

    offset++;
    defineTriangle(&s_struct->triangles[offset*3],A_id,D_id,H_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],H_id,E_id,A_id);

    offset++;
    defineTriangle(&s_struct->triangles[offset*3],A_id,B_id,F_id);
    offset++;
    defineTriangle(&s_struct->triangles[offset*3],F_id,E_id,A_id);




}


void displaySquare(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
        glVertex2f(-0.5,-0.5);
        glVertex2f(0.5,-0.5);
        glVertex2f(0.5,0.5);
        glVertex2f(-0.5,0.5);
    glEnd();
    glFlush();
}

void init(){
    glClearColor(0.0,0.0,0.0,0.0);
    glColor3f(1.0,1.0,1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0,1.0,-1.0,1.0);
}


int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("LO13");
    init();
    glutDisplayFunc(displaySquare);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}