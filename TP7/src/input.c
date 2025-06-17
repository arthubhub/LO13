#include <GL/glut.h>
#include <stdio.h>      // for printf
#include <math.h>       // for fabs
#include "input.h"
#include "opengl_state.h"
#include "transform.h"
#include "renderer.h"
#include "app.h"

void Reshape(int w, int h){
    if (w>h){

        ogl.fu = (float)w/h;
        ogl.fv = 1.0 ;

    }
    else {
        ogl.fv = (float)h/w;
        ogl.fu = 1.0 ; 
    }
    ogl.winSizeX=w;
    ogl.winSizeY=h;
    MatriceProjection();
    glViewport(0,0,w,h);
}

void Keyboard(unsigned char key, int x, int y){
    (void)x; (void)y; // to avoid warning

    switch (key) {
        case 'q':
        case 'Q':
        case 27:
            exit(0);
            break;
        case 'z':
            ogl.fzoom*=FZOOM;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'Z':
            ogl.fzoom*=1/FZOOM;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '0':
            ogl.renderMode = FILAIRE_STPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '1':
            ogl.renderMode = FILAIRE_UNIE_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '2':
            ogl.renderMode = SOLIDE_DEG_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '3':
            ogl.renderMode = SOLIDE_FILAIRE_ATPC;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '4':
            ogl.renderMode = OMBRAGE_CONST;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '5':
            ogl.renderMode = OMBRAGE_CONST_FILAIRE;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '6':
            ogl.renderMode = OMBRAGE_FILAIRE_CONST;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '7':
            ogl.renderMode = OMBRAGE_PHONG;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '8':
            ogl.renderMode = OMBRAGE_PHONG_FILAIRE;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case '9':
            ogl.renderMode = OMBRAGE_FILAIRE_PHONG;
            glutPostRedisplay();
            MatriceProjection();
            break;

        case 'g':
            ogl.renderMode = GAUSS_CURVATURE;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'C':
            ogl.renderMode = CARREAUX_CLASSIQUE;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'F':
            ogl.renderMode = CARREAUX_FUN;
            ogl.current_carreau = 0;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'D':
            ogl.renderMode = DIFFUSION;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'a':
            ogl.current_mat=&(ogl.steel);
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'c':
            ogl.current_mat=&(ogl.copper);
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'p':
            ogl.current_mat=&(ogl.plastic);
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 's':
            ogl.shrink+=0.02;
            if (ogl.shrink>0.9f)
                ogl.shrink=0.9f;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'S':
            ogl.shrink-=0.02;
            if (ogl.shrink<0.0f)
                ogl.shrink=0.0f;
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'i':
            ReinitialisationParamGraphiques();
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'P':
            if (ogl.proj_mode==PROJECTION){
                ogl.proj_mode=NONE;
            }
            else {
                ogl.proj_mode=PROJECTION;
            }
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'l':
            if (ogl.proj_mode==MIROIRS){
                ogl.proj_mode=NONE;
            }
            else {
                ogl.proj_mode=MIROIRS;
            }
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'k':
            if (ogl.proj_mode==OMBRE){
                ogl.proj_mode=NONE;
            }
            else {
                ogl.proj_mode=OMBRE;
            }
            glutPostRedisplay();
            MatriceProjection();
            break;
        case 'r':
            ogl.repere^=1;
            glutPostRedisplay();
            MatriceProjection();
            break;


        default:
            break;
    }
}


void Mouse(int32_t b, int32_t state, int32_t sx, int32_t sy) {
    if (b == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        printf("Clic gauche\n");
        ogl.flagTransformation |= ROTATION_STATE;
        ogl.sx0 = sx;
        ogl.sy0 = sy;
    } else {
        ogl.flagTransformation &= ~ROTATION_STATE; 
    }
    if (b == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        printf("Clic droit\n");
        ogl.flagTransformation |= TRANSLATION_STATE;
        ogl.sx0 = sx;
        ogl.sy0 = sy;
    } else {
        ogl.flagTransformation &= ~TRANSLATION_STATE; 
    }
}

void Motion(int32_t sx, int32_t sy){
    int32_t vx, vy;
    float q_u, q_v;
    float transl_u, transl_v;

    vx = sx - ogl.sx0;
    vy = ogl.sy0 - sy;

    if (ogl.flagTransformation & ROTATION_STATE) {
        ogl.rot_v = vx * ROTATION_SENSIBILITY;
        ogl.rot_u = vy * ROTATION_SENSIBILITY;
        glutPostRedisplay();
    }

    if (ogl.flagTransformation & TRANSLATION_STATE) {
        // 1. Règle de trois pour rapport déplacement souris
        q_u = (float)vx / ogl.winSizeX;
        q_v = (float)vy / ogl.winSizeY;

        // 2. Application au cadrage (plan de vue)
        transl_u = q_u * (ogl.Umax - ogl.Umin);
        transl_v = q_v * (ogl.Vmax - ogl.Vmin);

        // 3. Correction par Thalès
        transl_u *= fabs(ogl.focal_n) / ogl.Dmin;
        transl_v *= fabs(ogl.focal_n) / ogl.Dmin;

        // 4. Application du zoom et anisotropie
        ogl.trans_u = transl_u * ogl.fu * ogl.fzoom;
        ogl.trans_v = transl_v * ogl.fv * ogl.fzoom;

        glutPostRedisplay();
    }

    ogl.sx0 = sx;
    ogl.sy0 = sy;
}

void EvenementsGraphiques(void) {
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
}
