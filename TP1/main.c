#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <stdint.h>
#include <math.h>

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

void displayCircle(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    uint8_t points=200;
    float rayon=1;
    float x,y;
    for (uint8_t i = 0 ; i<points ; i++){
        x = rayon * cos(i * (2* (float)M_PI) / points );
        y = rayon * sin(i * (2* (float)M_PI) / points );
        glVertex2f(x,y);
    }


    glEnd();
    glFlush();
}

void displaySemiCircle(float rayon,float a_begin, float a_end, uint16_t nb_pts){
    printf("Semi circle from a=%.3f to a=%.3f with r=%.3f and %d points...\n\n",a_begin,a_end,rayon,nb_pts);
    // on va aller de a_begin à a_end avec nb_pts points
    float x;
    float y;
    glVertex2f(0,0);
    for (uint16_t i = 0 ; i<nb_pts ; i++){
        x = rayon * cos( a_begin + ((float)i/nb_pts) * (a_end-a_begin) );
        y = rayon * sin( a_begin + ((float)i/nb_pts) * (a_end-a_begin) );
        glVertex2f(x,y);
        printf( "x = %.3f ; y = %.3f\n", x,y  );
    }
}


void displayCamembert(){
    float rayon=1;
    float a_begin ;
    float a_end ;
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);

    uint16_t divisions = 20;
    for (int i=0; i<divisions; i++){
        glColor3f(1-(float)i/divisions, 0, (float)i/divisions);
        a_begin = i * (2* (float)M_PI) / divisions;
        a_end = (i+1) * (2* (float)M_PI) / divisions;
        displaySemiCircle(rayon,a_begin,a_end,255);
    }
    glEnd();
    glFlush();
}


void displayTriangles(){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);


    uint8_t points=60;
    float rayon=1;
    float x,y;
    for (uint8_t i = 0 ; i<points ; i++){
        //glColor3f(1-fabs(1-(float)i*2/points),0,fabs(1-(float)i*2/points));
        glColor3f(cos(M_PI*(float)i/points),cos(2*M_PI/3+M_PI*(float)i/points),cos(4*M_PI/3+M_PI*(float)i/points));
        glBegin(GL_TRIANGLES);
        glVertex2f(0,0);
        x = rayon * cos(i * (2* (float)M_PI) / points );
        y = rayon * sin(i * (2* (float)M_PI) / points );
        glVertex2f(x,y);

        x = rayon * cos((i+1) * (2* (float)M_PI) / points );
        y = rayon * sin((i+1) * (2* (float)M_PI) / points );
        glVertex2f(x,y);
        glEnd();

    }




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
    glutDisplayFunc(displayTriangles);
    glutMainLoop();
    
    return EXIT_SUCCESS;
}