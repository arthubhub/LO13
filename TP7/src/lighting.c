#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include "lighting.h"
#include "math_utils.h"
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

    for (int i=0;i<3;i++){
        ogl.src.ambient[i]=1;
        ogl.src.diffuse[i]=1;
        ogl.src.position[i]=5;
        ogl.src.specular[i]=1;
    }

    
    ogl.src.ambient[3]=1;
    ogl.src.diffuse[3]=1;
    ogl.src.specular[3]=1;
    ogl.src.position[3]=0.0f; // position à l'infini



}

void InitialiserOption4_Ombres(void){
    ogl.PlanOmbre_Z=-0.01f;

    memset(ogl.Po, 0, 16 * sizeof(float));
/*
0  4  8  12
1  5  9  13
2  6  10 14
3  7  11 15
*/
    float d =  ogl.PlanOmbre_Z  - ogl.src.position[2];

    ogl.Po[0]  = 1.0f; 
    ogl.Po[5]  =  1.0f;
    ogl.Po[10] =  1.0f;
    ogl.Po[11] =  1/d;  

}

void InitialiserOption8_Diffusion(void){
    ogl.nb_lignes_diffusion=20;

    float norm_src = Norm(ogl.src.position);
    ogl.source_normalisee[0] = ogl.src.position[0]/norm_src;
    ogl.source_normalisee[1] = ogl.src.position[1]/norm_src;
    ogl.source_normalisee[2] = ogl.src.position[2]/norm_src;

    printf("----------\nSource normalisée :\nx = %f\ny = %f\nz = %f\n----------\n",ogl.source_normalisee[0],ogl.source_normalisee[1],ogl.source_normalisee[2]);
    printf("Norme de la source normalisée : %f\n",Norm(ogl.source_normalisee));
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



    /*steel */
    // this one is more a grey plastic than realsteel
    // Name 	        Ambient 	                Diffuse 	                Specular 	                        Shininess
    //steel 	        0.23125 	0.23125 	0.23125 	0.2775 	0.2775 	0.2775 	0.773911 	0.773911 	0.773911 	0.6
    /*ogl.steel.ambient[0]=0.23125;
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
    ogl.steel.specular[3]=1;*/


    //Polished  steel
    ogl.steel.ambient[0]  = 0.23125f;
    ogl.steel.ambient[1]  = 0.23125f;
    ogl.steel.ambient[2]  = 0.23125f;
    ogl.steel.diffuse[0]  = 0.2775f;
    ogl.steel.diffuse[1]  = 0.2775f;
    ogl.steel.diffuse[2]  = 0.2775f;
    ogl.steel.specular[0] = 0.773911f;
    ogl.steel.specular[1] = 0.773911f;
    ogl.steel.specular[2] = 0.773911f;
    ogl.steel.shininess   = 89.6f; // valeur typique pour surface très brillante

    ogl.steel.ambient[3]  = 1.0f;
    ogl.steel.diffuse[3]  = 1.0f;
    ogl.steel.specular[3] = 1.0f;

    //  steel (argent mat)
    /*ogl.steel.ambient[1]  = 0.19225f;
    ogl.steel.ambient[0]  = 0.19225f;
    ogl.steel.ambient[2]  = 0.19225f;
    ogl.steel.diffuse[0]  = 0.50754f;
    ogl.steel.diffuse[1]  = 0.50754f;
    ogl.steel.diffuse[2]  = 0.50754f;
    ogl.steel.specular[0] = 0.508273f;
    ogl.steel.specular[1] = 0.508273f;
    ogl.steel.specular[2] = 0.508273f;
    ogl.steel.shininess   = 51.2f;
    ogl.steel.ambient[3]  = 1.0f;
    ogl.steel.diffuse[3]  = 1.0f;
    ogl.steel.specular[3] = 1.0f;*/

    // steel (acier miroir)
    /*ogl.steel.ambient[0]  = 0.25f;
    ogl.steel.ambient[1]  = 0.25f;
    ogl.steel.ambient[2]  = 0.25f;
    ogl.steel.diffuse[0]  = 0.4f;
    ogl.steel.diffuse[1]  = 0.4f;
    ogl.steel.diffuse[2]  = 0.4f;
    ogl.steel.specular[0] = 0.774597f;
    ogl.steel.specular[1] = 0.774597f;
    ogl.steel.specular[2] = 0.774597f;
    ogl.steel.shininess   = 76.8f;
    ogl.steel.ambient[3]  = 1.0f;
    ogl.steel.diffuse[3]  = 1.0f;
    ogl.steel.specular[3] = 1.0f;*/

    // steel (métal sombre)
    /*ogl.steel.ambient[0]  = 0.105882f;
    ogl.steel.ambient[1]  = 0.058824f;
    ogl.steel.ambient[2]  = 0.113725f;
    ogl.steel.diffuse[0]  = 0.427451f;
    ogl.steel.diffuse[1]  = 0.470588f;
    ogl.steel.diffuse[2]  = 0.541176f;
    ogl.steel.specular[0] = 0.333333f;
    ogl.steel.specular[1] = 0.333333f;
    ogl.steel.specular[2] = 0.521569f;
    ogl.steel.shininess   = 9.84615f;
    ogl.steel.ambient[3]  = 1.0f;
    ogl.steel.diffuse[3]  = 1.0f;
    ogl.steel.specular[3] = 1.0f;*/






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


