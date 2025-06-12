#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "math_utils.h"
#include <math.h>
#include <time.h>




Mesh msh;

void InitialiserCouleursCarreaux(Mesh *msh)
{
    int i;
    // Allocation mémoire pour les couleurs RGB (3 floats par couleur)
    msh->couleurs_carreaux = (float*)malloc(msh->nb_couleurs_carreaux * 3 * sizeof(float));
    if (!msh->couleurs_carreaux) {
        printf("Erreur: impossible d'allouer la mémoire pour les couleurs des carreaux\n");
        return;
    }
    
    // Initialisation du générateur de nombres aléatoires
    srand((unsigned int)time(NULL));
    
    // Génération de couleurs RGB vives aléatoires
    for (i = 0; i < msh->nb_couleurs_carreaux; i++) {
        int base = i * 3;
        
        // Génération RGB directe dans la plage [0.15, 0.85] pour des couleurs vives
        msh->couleurs_carreaux[base + 0] = 0.15f + 0.7f * ((float)rand() / RAND_MAX); // R
        msh->couleurs_carreaux[base + 1] = 0.15f + 0.7f * ((float)rand() / RAND_MAX); // G
        msh->couleurs_carreaux[base + 2] = 0.15f + 0.7f * ((float)rand() / RAND_MAX); // B
    }
    
    printf("Couleurs des carreaux initialisées : %d couleurs RGB vives aléatoires\n", msh->nb_couleurs_carreaux);
}

void InitializeMesh(Mesh *msh)  /* Initialisation de la struture mesh */
{
    int		j;

    msh->number_of_vertices = 0;
    msh->number_of_triangles = 0;
    msh->vertices = NULL;
    msh->triangles = NULL;
    for (j=0; j<3; j++)
    {
        msh->ccenter[j] = 0.0;
        msh->cmin[j] = 0.0;
        msh->cmax[j] = 0.0;
    }
    msh->delta = 0.0;
    msh->memory = 0;
    msh->error = 0;
    msh->curvature_v = NULL;
    msh->carreaux = NULL;
    msh->curvature_min = 0;
    msh->curvature_max = 0;


    msh->couleurs_carreaux = NULL;
    msh->nb_couleurs_carreaux = 30;
    InitialiserCouleursCarreaux(msh);
}

void ReadMesh(Mesh *msh, char **argv) /* lecture du fichier au format mesh INRIA Gamma3*/
{
    FILE		*file;
    char		file_name[264], keyword[80];
    int           i, j, ii, dim;

    sprintf(file_name, "%s", argv[1]);
    if ((file = fopen(file_name, "r")) == NULL)
    {
        printf("error: file %s not found\n", file_name);
        msh->error = 100;
        exit(1);
    }
    for (;;)
    {
        fscanf(file, "%s", keyword);
        if (strcmp(keyword, "EOF") == 0 || strcmp(keyword, "End") == 0 || strcmp(keyword, "end") == 0) /* fin du fichier */
            break;
        else if (strcmp(keyword, "Dimension") == 0 || strcmp(keyword, "dimension") == 0 ) /* mot cle dimension */
            fscanf(file, "%d", &dim);
        else if (strcmp(keyword, "Vertices") == 0 || strcmp(keyword, "vertices") == 0 )  /* mot cle Vertices */
        {
            fscanf(file, "%d", &(msh->number_of_vertices));
            msh->vertices = (float*)malloc((3*msh->number_of_vertices)*sizeof(float));
            if (!msh->vertices)
            {
                printf("error: not enough memory for %d vertices (%ld bytes)\n", msh->number_of_vertices, 3*msh->number_of_vertices*sizeof(float));
                msh->error = 200;
                return;
            }
            else
                msh->memory += (3*msh->number_of_vertices)*sizeof(float);
            if (dim == 2)
            {
                for (i=0; i<msh->number_of_vertices; i++)
                {
                    ii = 3 * i;
                    fscanf(file, "%f %f %d", &(msh->vertices[ii]), &(msh->vertices[ii+1]), &j);
                    msh->vertices[ii+2] = 0.0;
                }
            } else if (dim == 3)
            {
                for (i=0; i<msh->number_of_vertices; i++)
                {
                    ii = 3 * i;
                    fscanf(file, "%f %f %f %d", &(msh->vertices[ii]), &(msh->vertices[ii+1]), &(msh->vertices[ii+2]), &j);
                }
            }
        } else if (strcmp(keyword, "Triangles") == 0 || strcmp(keyword, "triangles") == 0) /* mot cle Triangles */
        {
            fscanf(file, "%d", &(msh->number_of_triangles));
            msh->triangles = (int*)malloc((3*msh->number_of_triangles)*sizeof(int));
            msh->carreaux  = (int*)malloc((3*msh->number_of_triangles)*sizeof(int));
            if (!msh->triangles)
            {
                printf("error: not enough memory for %d triangles (%ld bytes)\n", msh->number_of_triangles, 3*msh->number_of_triangles*sizeof(int));
                msh->error = 200;
                return;
            }
            else
                msh->memory += (3*msh->number_of_triangles)*sizeof(int);
            for (i=0; i<msh->number_of_triangles; i++)
            {
                ii = 3 * i;
                fscanf(file, "%d %d %d %d", &(msh->triangles[ii]), &(msh->triangles[ii+1]), &(msh->triangles[ii+2]), &(msh->carreaux[i]));
                printf("Triangle %d, carreau = %d\n",i,msh->carreaux[i]);

                for (j=0; j<3; j++)
                    msh->triangles[ii+j]--;
            }
        }
    }
    fclose(file);
    /* affichage a l'ecran des infos sur le maillage */
    printf("mesh : %d vertices -- %d triangles (%ld kbytes)\n", msh->number_of_vertices, msh->number_of_triangles, msh->memory/1024);
}


void NormalizeMesh(Mesh *msh) /* normalisation des coordonnees des sommets entre 0 et 1 */
{
    int	        point_index, point_x_index, j;
    // cmin et cmax sont le premier point
    for (j=0; j<3; j++)
    {
        msh->cmin[j] = msh->vertices[j];
        msh->cmax[j] = msh->vertices[j];
    }
    // pour chaque point
    for (point_index=1; point_index<msh->number_of_vertices; point_index++)
    {
        // pour chaque coordonnée
        point_x_index = 3 * point_index;
        for (j=0; j<3; j++)
        {
            // si la coordonnée < cmin -> cmin = la coordonnée
            if (msh->cmin[j] > msh->vertices[point_x_index+j])
                msh->cmin[j] = msh->vertices[point_x_index+j];
            if (msh->cmax[j] < msh->vertices[point_x_index+j])
                msh->cmax[j] = msh->vertices[point_x_index+j];
        }
    }
    for (j=0; j<3; j++){ // le centre c'est le point du milieu de chaque coordonnée
        msh->ccenter[j] = (float)(0.5 * (msh->cmin[j] + msh->cmax[j]));
    }
    printf("min : x=%f  y=%f  z=%f\n",msh->cmin[0],msh->cmin[1],msh->cmin[2]);
    printf("max : x=%f  y=%f  z=%f\n",msh->cmax[0],msh->cmax[1],msh->cmax[2]);
    printf("centre : x=%f  y=%f  z=%f\n",msh->ccenter[0],msh->ccenter[1],msh->ccenter[2]);


    msh->delta = msh->cmax[0] - msh->cmin[0]; // delta prend la distance entre cmin(x) et cmax(x)
    for (j=1; j<3; j++) // on calcule le plus grand delta
        if (msh->delta < msh->cmax[j] - msh->cmin[j])
            msh->delta = msh->cmax[j] - msh->cmin[j];
    printf("delta=%f\n",msh->delta);
    // pour chaque point du mesh
    for (point_index=0; point_index<msh->number_of_vertices; point_index++)
    {
        point_x_index = 3 * point_index;
        for (j=0; j<3; j++)
        {
            msh->vertices[point_x_index+j] += 0.5*msh->delta - msh->ccenter[j]; // on met le point min en 0.5
            msh->vertices[point_x_index+j] /= msh->delta; // on met à l'échelle
        }
    }
    
    for (j=0; j<3; j++)
    {
        msh->cmin[j] +=  0.5*msh->delta - msh->ccenter[j];
        msh->cmax[j] +=  0.5*msh->delta - msh->ccenter[j];
        msh->ccenter[j] =0.5; 

        msh->cmax[j] /= msh->delta; 
        msh->cmin[j] /= msh->delta;
    }
    printf("min : x=%f  y=%f  z=%f\n",msh->cmin[0],msh->cmin[1],msh->cmin[2]);
    printf("max : x=%f  y=%f  z=%f\n",msh->cmax[0],msh->cmax[1],msh->cmax[2]);
    printf("centre : x=%f  y=%f  z=%f\n",msh->ccenter[0],msh->ccenter[1],msh->ccenter[2]);


}

void SetTriangleNormals(Mesh *msh)
{
    int           i, ii, j; /*declaration des variables*/
    float         *v0, *v1, *v2, u[3], v[3], n[3];
    double        norme;

    msh->normal_t = (float*)malloc((3*msh->number_of_triangles)*sizeof(float)); /*allocation de la memoire pour les normales aux triangles*/
    if (!msh->normal_t) /*gestion de l'erreur d'allocation m�moire*/
    {
        printf("error: not enough memory for %d normales aux triangles (%ld bytes)\n", msh->number_of_triangles, 3*msh->number_of_triangles*sizeof(float));
        msh->error = 200;
        return;
    }
    else
        msh->memory += (3*msh->number_of_triangles)*sizeof(float);
    /*fin de l'allocation de la memoire pour les normales aux triangles*/

    msh->alpha_t = (float*)malloc((3*msh->number_of_triangles)*sizeof(float)); /*allocation de la memoire pour les angles des triangles*/
    if (!msh->alpha_t) /*gestion de l'erreur d'allocation m�moire*/
    {
        printf("error: not enough memory for %d angles aux triangles (%ld bytes)\n", msh->number_of_triangles, 3*msh->number_of_triangles*sizeof(float));
        msh->error = 200;
        return;
    }
    else
        msh->memory += (3*msh->number_of_triangles)*sizeof(float);
    /*fin de l'allocation de la memoire pour les angles des triangles*/

    for (i = 0; i < (msh->number_of_triangles); i++) /*boucle sur les triangles*/
    {
        ii = 3 * i;
        v0 = &(msh->vertices[3 * msh->triangles[ii]]); /*premiere coordonnee du premier point du triangle ii*/
        v1 = &(msh->vertices[3 * msh->triangles[ii+1]]); /*premiere coordonnee du deuxieme point du triangle ii*/
        v2 = &(msh->vertices[3 * msh->triangles[ii+2]]); /*premiere coordonnee du troisieme point du triangle ii*/
        for (j = 0; j < 3; j++) /*calcul des vecteurs du triangles*/
        {
            u[j] = v1[j] - v0[j];
            v[j] = v2[j] - v0[j];
        }
        CrossProduct(u, v, n); /*effectue le produit vectoriel de u et v, le r�sultats est le tableau n*/
        norme = NormSquare(n); /*effectue la norme au carre de n (n est un tableau de 3 float)*/
        if (norme != 0)
        {
            norme = 1.0 / sqrt(norme);
            msh->normal_t[ii] = (float)(norme * n[0]);
            msh->normal_t[ii+1] = (float)(norme * n[1]);
            msh->normal_t[ii+2] = (float)(norme * n[2]);
        }
        else
        {
            msh->normal_t[ii] = n[0];
            msh->normal_t[ii+1] = n[1];
            msh->normal_t[ii+2] = n[2];
        }
        /* calcul des angles associ�s �chaque sommet de chaque triangle*/
        msh->alpha_t[ii] = (float)Angle(v2, v0, v1); /*calcul du premier angle du triangle*/
        msh->alpha_t[ii+1] = (float)Angle(v0, v1, v2); /*calcul du deuxieme angle du triangle*/
        msh->alpha_t[ii+2] =  (float)Angle(v1, v2, v0); /*calcul du troisieme angle du triangle*/
    }
}


void SetVertexNormals(Mesh *msh)
{
    int           i, ii, j, k;
    double        norme;

    msh->normal_v = (float*)malloc((3*msh->number_of_vertices)*sizeof(float)); /*allocation de la memoire des normales aux sommets*/
    if (!msh->normal_v)/*gestion de l'erreur d'allocation m�moire*/
    {
        printf("error: not enough memory for %d normales aux sommets (%ld bytes)\n",
                msh->number_of_vertices, 3*msh->number_of_vertices*sizeof(float));
        msh->error = 200;
        return;
    }
    else
        msh->memory += (3*msh->number_of_vertices)*sizeof(float);
    /*fin de l'allocation de la memoire des normales aux sommets*/

    for (i = 0; i < (msh->number_of_vertices); i++) /* d�but du calcul des normales aux sommets (initialistation a 0) */
    {
        ii = 3 * i;
        msh->normal_v[ii] = 0.0;
        msh->normal_v[ii+1] = 0.0;
        msh->normal_v[ii+2] = 0.0;
    }
    if (msh->number_of_triangles != 0) /*test si il y a des triangles*/
    {
        for (i = 0; i < (msh->number_of_triangles); i++) /*calcul des normales aux sommets des triangles*/
        {
            ii = 3 * i;
            for (j = 0; j < 3; j++) /*boucle sur les sommets*/
                for (k = 0; k < 3; k++) /*boucle sur les coordonnes*/
                    msh->normal_v[3*msh->triangles[ii+j]+k] += msh->alpha_t[ii+j] * msh->normal_t[ii+k]; /*incr�mentation pour chaque sommet*/
        }
    }
    for (i = 0; i < (msh->number_of_vertices); i++) /*normalisations des normales*/
    {
        ii = 3 * i;
        norme = NormSquare(&(msh->normal_v[ii]));
        if (norme != 0)
        {
            norme = 1.0 / sqrt(norme);
            for (j = 0; j < 3; j++)
                msh->normal_v[ii+j] = (float)(norme * msh->normal_v[ii+j]);
        }
        else
            for (j = 0; j < 3; j++)
                msh->normal_v[ii+j] = 0.0;
    }
}


void SetNormals(Mesh *msh)
{
    if (msh->number_of_triangles != 0) /*test si il y a des triangles*/
    {
        SetTriangleNormals(msh);
        SetVertexNormals(msh);
    }
}




void SetCurvature(Mesh *msh) {
    int i, j, vi;
    int nV = msh->number_of_vertices;
    int nT = msh->number_of_triangles;
    msh->curvature_v = (float*)malloc(nV * sizeof(float));
    if (!msh->curvature_v) {
        printf("error: not enough memory for %d curvature values\n", nV);
        msh->error = 200;
        return;
    }
    msh->memory += nV * sizeof(float);

    for (i = 0; i < nV; ++i)
        msh->curvature_v[i] = 0.0f;

    for (i = 0; i < nT; ++i) {
        int base = 3*i;
        for (j = 0; j < 3; ++j) {
            vi = msh->triangles[base + j];
            msh->curvature_v[vi] += msh->alpha_t[base + j]; // -> ii+j dans le code du prof
            
        }
    }
    for (i = 0; i < nV; ++i) {
        msh->curvature_v[i] = (float)(2.0 * M_PI - msh->curvature_v[i]);
        if (msh->curvature_v[i]<-0.5f) msh->curvature_v[i]=-0.5f;
        if (msh->curvature_v[i]>0.5f) msh->curvature_v[i]=0.5f;
        if (msh->curvature_v[i] < msh->curvature_min){ 
            msh->curvature_min = msh->curvature_v[i];
        }
        else if (msh->curvature_v[i] > msh->curvature_max) msh->curvature_max = msh->curvature_v[i];

        //printf("Curvature[%d] = %f\n",i,msh->curvature_v[i]);
    }
    printf("Min curvature : %f\nMax curvature : %f\n",msh->curvature_min,msh->curvature_max);
    //msh->curvature_min /= 10;
    //msh->curvature_max /= 10;

}



void defineCube(void){
    int i;
    
    
    msh.number_of_vertices = NB_VERTICES_CUBE;
    msh.vertices    = (float*)malloc(msh.number_of_vertices*3*sizeof(float));
/*    
          4-----------5
         /|          /|
        7-----------6 |
        | |         | |
        | 0---------|-1
        |/          |/
        3-----------2---- X
*/
    for (i=0; i<msh.number_of_vertices*3;i++ ){
        msh.vertices[i]=0.0;
    }
    
    // Bottom
    // 0
    i=0;
    // 1
    i++;
    msh.vertices[3*i +1 ]=1.0;
    // 2
    i++;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i ]=1.0;
    // 3
    i++;
    msh.vertices[3*i  ]=1.0;
    // Top
    // 4
    i++;
    msh.vertices[3*i +2 ]=1.0;
    // 5
    i++;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i +2 ]=1.0;
    // 6
    i++;
    msh.vertices[3*i  ]=1.0;
    msh.vertices[3*i +1 ]=1.0;
    msh.vertices[3*i +2 ]=1.0;
    // 7
    i++;
    msh.vertices[3*i ]=1.0;
    msh.vertices[3*i +2 ]=1.0;

    msh.number_of_triangles = NB_TRIANGLES_CUBE;
    msh.triangles=(int*)malloc(msh.number_of_triangles*3*sizeof(int));

    
    // Face du bas
    i=0;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=1;
    msh.triangles[i*3 + 2]=2;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=3;
    // Face de gauche
    i++;
    msh.triangles[i*3 + 0]=7;
    msh.triangles[i*3 + 1]=4;
    msh.triangles[i*3 + 2]=0;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=3;
    msh.triangles[i*3 + 2]=7;
    // Face de droite
    i++;
    msh.triangles[i*3 + 0]=1;
    msh.triangles[i*3 + 1]=5;
    msh.triangles[i*3 + 2]=6;
    i++;
    msh.triangles[i*3 + 0]=6;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=1;
    // Face du haut
    i++;
    msh.triangles[i*3 + 0]=6;
    msh.triangles[i*3 + 1]=5;
    msh.triangles[i*3 + 2]=4;
    i++;
    msh.triangles[i*3 + 0]=4;
    msh.triangles[i*3 + 1]=7;
    msh.triangles[i*3 + 2]=6;
    // Face de devant
    i++;
    msh.triangles[i*3 + 0]=3;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=7;
    i++;
    msh.triangles[i*3 + 0]=7;
    msh.triangles[i*3 + 1]=2;
    msh.triangles[i*3 + 2]=6;
    // Face de derriere
    i++;
    msh.triangles[i*3 + 0]=1;
    msh.triangles[i*3 + 1]=0;
    msh.triangles[i*3 + 2]=5;
    i++;
    msh.triangles[i*3 + 0]=0;
    msh.triangles[i*3 + 1]=4;
    msh.triangles[i*3 + 2]=5;
}

void ModeleDiscret(Mesh *msh, char **argv) {
    // Pour un fichier mesh, faire :
    InitializeMesh(msh);
    ReadMesh(msh,argv);
    NormalizeMesh(msh);
    SetNormals(msh);
    SetCurvature(msh);
    // Pour utiliser le Cube, faire : 
    // defineCube();
}