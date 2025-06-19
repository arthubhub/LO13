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
