# Visualiseur 3D - Projet LO13

**Auteur :** Arthur BIDET  
**Filière :** Réseaux & Télécommunications  
**Matière :** LO13 — Infographie 3D : théorie et application  
**Établissement :** Université de Technologie de Troyes (UTT)

---

## 🧠 Objectif du projet

Ce projet a été développé dans le cadre de l’UE LO13 et vise à concevoir un visualiseur 3D interactif en C avec OpenGL et GLUT. Il permet de visualiser, manipuler et transformer des maillages 3D selon différents modes de rendu, d’éclairage et de projection.

---

## ✨ Fonctionnalités principales

- **Initialisation complète de la scène graphique**
- **Définition du repère de vue, focalisation, et transformation géométrique**
- **Gestion des couleurs d'arrière-plan, de trait et de remplissage**
- **Choix entre projection orthogonale et perspective**
- **Zoom, translation et rotation dynamiques**
- **Ajout et configuration de la source lumineuse**
- **Support des ombres et des matériaux**
- **Projections, miroirs, symétries selon X, Y, Z**
- **Matrices de transformation homogènes (4x4)**
- **Mode "carreaux" et repères optionnels**
- **Mode d'affichage des lignes de diffusion**
- **Affichage d’aide interactif**

---

## 🏗️ Architecture du projet

```
.
├── src/ # Fichiers sources (.c)
├── include/ # Fichiers d'en-tête (.h)
├── obj/ # Objets compilés
├── main.c # Point d’entrée du programme
├── Makefile # Script de compilation
└── README.md # Documentation du projet
```


Modules principaux :
- `main.c`: configuration de la fenêtre
- `app.c` : initialisation globale
- `input.c` : gestion du clavier et de la souris
- `lighting.c` : configuration de la source lumineuse/matériaux
- `opengl_state.c` : gestion de l’état global OpenGL
- `math_utils` : procédures d'opérations sur les vecteurs
- `mash.c` : chargement du modèle et des paramètres
- `opengl_state.c` : structure opengl
- `renderer.c` : rendu visuel des objets et des options
- `transform.c` : transformations géométriques, logique d'affichage


---

## 🖥️ Lancer le projet

### 🔧 Prérequis

- GCC (compilateur C)
- OpenGL
- GLUT (FreeGLUT ou autre)

### ▶️ Compilation

```bash
make
./renderer <fichier.mesh>
```


### + Exemples 

  1) Affichage filaire
     <img width="648" alt="image" src="https://github.com/user-attachments/assets/dcc199d4-197a-4ee7-9dc7-00b5c8c645a5" />
  3) Affichage filaire avec traitement des parties cachées
     <img width="646" alt="image" src="https://github.com/user-attachments/assets/cc2441dc-72bf-4d99-972f-479a1e42d231" />
  5) Affichage filaire + uni
  6) Ombrage uni
  7) Ombrage uni + filaire
  8) Ombrage uni sur les arrêtes
  9) Ombrage de Phong
  10) Ombrage de Phong + filaire
  11) Ombrage de Phong sur les arrêtes
  12) Repère
  13) Plans et projections
  14) Miroirs
  15) Ombrage au sol
  16) Affichage des carreaux du fichier mesh
  17) Shrink
  18) Lignes de diffusion
  19) Courbure Gaussienne
