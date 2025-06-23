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
     
  2) Affichage filaire avec traitement des parties cachées
     
     <img width="646" alt="image" src="https://github.com/user-attachments/assets/cc2441dc-72bf-4d99-972f-479a1e42d231" />
     
  3) Affichage filaire + uni

      <img width="638" alt="image" src="https://github.com/user-attachments/assets/ac5ae66f-90e7-47c1-8b74-2d1e4df4940b" />

  4) Ombrage uni

      <img width="624" alt="image" src="https://github.com/user-attachments/assets/63e6d44b-058d-40c0-a914-03f94a16bc6c" />


  8) Ombrage uni + filaire

      <img width="629" alt="image" src="https://github.com/user-attachments/assets/ee93a035-c97c-401a-a975-4720b166a498" />


  9) Ombrage uni sur les arrêtes

      <img width="633" alt="image" src="https://github.com/user-attachments/assets/04d58311-da3c-4737-9f18-e0ba85920b8d" />


  10) Ombrage de Phong

      <img width="615" alt="image" src="https://github.com/user-attachments/assets/859d9eed-0b26-4635-999c-c5dab5f27cfc" />


  11) Ombrage de Phong + filaire

      <img width="616" alt="image" src="https://github.com/user-attachments/assets/b5ec5235-2dbf-4deb-bfb1-3467a692d4d0" />


  12) Ombrage de Phong sur les arrêtes

      <img width="608" alt="image" src="https://github.com/user-attachments/assets/29d5c7ab-1afb-488a-8a94-ab78c63c53d2" />


  13) Repère

      <img width="679" alt="image" src="https://github.com/user-attachments/assets/e219e999-fba2-4ca6-827a-b3648b9f6ab8" />


  14) Plans et projections

      <img width="664" alt="image" src="https://github.com/user-attachments/assets/29200bef-4543-4cc8-9fe7-c8849749ec67" />


  15) Miroirs

      <img width="580" alt="image" src="https://github.com/user-attachments/assets/045d7c50-a0a0-4da1-814c-58a6bb590774" />


  16) Ombrage au sol

      <img width="526" alt="image" src="https://github.com/user-attachments/assets/d37e004a-f43b-4c15-a70f-1e7132d0a8f9" />


  17) Affichage des carreaux du fichier mesh
  
      <img width="809" alt="image" src="https://github.com/user-attachments/assets/6dace2ec-6f8b-4259-9ed6-b0fe2469e456" />


  19) Shrink

      <img width="1136" alt="image" src="https://github.com/user-attachments/assets/4a12063d-93ca-478f-a9c8-0318c491cd56" />

  
  20) Lignes de diffusion

      <img width="645" alt="image" src="https://github.com/user-attachments/assets/774e786a-dc2e-413e-9561-7eb6fa4c206f" />


  21) Courbure Gaussienne

      <img width="1248" alt="image" src="https://github.com/user-attachments/assets/2d6a703f-4da2-4c38-8f6a-f705e5551bb8" />


