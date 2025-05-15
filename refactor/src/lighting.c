#include <GL/glut.h>
#include "lighting.h"
#include "opengl_state.h"  // Defines 'extern Opengl ogl;'

/*
 * Stub implementations: paste the corresponding code from your original file
 * into these functions in mesh.c to preserve your logic.
 */

 void InitialiserSourceLumineuse(void){
    /*Le premier champ indique les coordonnées homogènes de la source. Par exemple une source
    dans la direction (1, 1, 1) à l’infini aura pour coordonnées (1.0, 1.0, 1.0, 0.0). Les autres champs
    sont conformes à la théorie et sont respectivement 𝑆௔, 𝑆ௗ, 𝑆௦ avec la quatrième composante
    pour chaque champ à fixer à 1.0 (pour l’opacité).*/

    for (int i=0;i<=3;i++){
        ogl.src.ambient[i]=1;
        ogl.src.diffuse[i]=1;
        ogl.src.position[i]=1;
        ogl.src.specular[i]=1;
    }
    ogl.src.position[3]=0.0f; // position à l'infini

}

void InitialiserMateriaux(void){
    // http://devernay.free.fr/cours/opengl/materials.html
    /*En considérant des matériaux opaques, la dernière composante de ces vecteurs
est toujours fixée à 1.0.*/


    /* Plastic */
    // Name 	        Ambient 	            Diffuse 	                    Specular 	                        Shininess
    // cyan plastic 	0.0 	0.1 	0.06 	0.0 	0.50980392 	0.50980392 	0.50196078 	0.50196078 	0.50196078 	0.25
    ogl.plastic.ambient[0]=0.0;
    ogl.plastic.ambient[1]=0.15;
    ogl.plastic.ambient[2]=0.15;
    ogl.plastic.diffuse[0]=0.0;
    ogl.plastic.diffuse[1]=0.50980392;
    ogl.plastic.diffuse[2]=0.50980392;
    ogl.plastic.specular[0]=0.50196078;
    ogl.plastic.specular[1]=0.50196078;
    ogl.plastic.specular[2]=0.50196078;
    ogl.plastic.shininess=0.25;

    ogl.plastic.ambient[3]=1;
    ogl.plastic.diffuse[3]=1;
    ogl.plastic.specular[3]=1;

    /* copper */
    // Name 	        Ambient 	                Diffuse 	                Specular 	                        Shininess
    // copper 	        0.19125 	0.0735 	0.0225 	0.7038 	0.27048 	0.0828 	0.256777 	0.137622 	0.086014 	0.1
    ogl.copper.ambient[0]=0.19125;
    ogl.copper.ambient[1]=0.0735;
    ogl.copper.ambient[2]=0.0225;
    ogl.copper.diffuse[0]=0.7038;
    ogl.copper.diffuse[1]=0.27048;
    ogl.copper.diffuse[2]=0.0828;
    ogl.copper.specular[0]=0.256777;
    ogl.copper.specular[1]=0.137622;
    ogl.copper.specular[2]=0.086014;
    ogl.copper.shininess=0.1;

    ogl.copper.ambient[3]=1;
    ogl.copper.diffuse[3]=1;
    ogl.copper.specular[3]=1;



    /* steel */
    // this one is more a grey plastic than real steel
    // Name 	        Ambient 	                Diffuse 	                Specular 	                        Shininess
    // steel 	        0.23125 	0.23125 	0.23125 	0.2775 	0.2775 	0.2775 	0.773911 	0.773911 	0.773911 	0.6
    ogl.steel.ambient[0]=0.23125;
    ogl.steel.ambient[1]=0.23125;
    ogl.steel.ambient[2]=0.27;
    ogl.steel.diffuse[0]=0.2775;
    ogl.steel.diffuse[1]=0.2775;
    ogl.steel.diffuse[2]=0.3075;
    ogl.steel.specular[0]=0.773911;
    ogl.steel.specular[1]=0.773911;
    ogl.steel.specular[2]=0.803911;
    ogl.steel.shininess=0.6;

    ogl.steel.ambient[3]=1;
    ogl.steel.diffuse[3]=1;
    ogl.steel.specular[3]=1;

    ogl.current_mat = &(ogl.plastic); // Materiau par défaut
}

void ActivationSource(void){
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(GL_LIGHT0, GL_POSITION, ogl.src.position);
    glPopMatrix();
    glLightfv(GL_LIGHT0, GL_AMBIENT, ogl.src.ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ogl.src.diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, ogl.src.specular);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}
void DesactivationSource(void){
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}
void AffectationMateriau(void)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (ogl.current_mat)->ambient) ; // or *(ogl.current_mat).ambient
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (ogl.current_mat)->diffuse) ;
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (ogl.current_mat)->specular) ;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, (ogl.current_mat)->shininess) ;
}