#include <stdio.h>
#include <assert.h>  

#include "histogram.h"
#include "image.h"


static int dist2(int* pix, int* col)
{
  int dR;
  int dG;
  int dB;

  dR = pix[0] - col[0];
  dG = pix[1] - col[1];
  dB = pix[2] - col[2];

  return (dR*dR + dG*dG + dB*dB);
}

void mapping(image input, image output, int* tab, int K) { 
  
  int dim;
  int w;
  int h;

  int* pix;          /* pixel courant lu dans l'image */
  int best_i;        /* indice de la meilleure couleur dans tab */
  int best_dist;     /* meilleure distance (min) */
  int i;            
  int out_color[3];  /* couleur à écrire dans output */

  bouleen cont_in;   /* bool renvoyé par image_pixel_suivant(input) */
  bouleen cont_out;  /* bool renvoyé par image_pixel_suivant(output) */

  int* candidate;    /* pointeur sur la couleur candidate dans tab */
  int d;             /* distance calculée */

  /* cas d'erreurs */
  assert(input != NULL_IMAGE);
  assert(output != NULL_IMAGE);
  assert(tab != NULL);
  assert(K > 0);

  /* dimention de l'image */
  dim = image_give_dim(input);
  w   = image_give_largeur(input);
  h   = image_give_hauteur(input);
  assert(dim == 3);

  /* initialisation de l'image de sortie */
  image_initialize(output, dim, w, h);

  /*  parcours sdes images input et output (comme dans l'exemple 2)  */
  image_debut(input);
  image_debut(output);

  do
  {
    /* lire le pixel courant */
    pix = image_lire_pixel(input);

    /* recherche de la couleur la plus proche dans tab */
    best_i = 0;                          /* initialisé à 0 pour prendre la première valeur */
    candidate = &tab[0];                 /* couleur 0 = tab[0],tab[1],tab[2] */
    best_dist = dist2(pix, candidate);

    for (i = 1; i < K; i++) {
      candidate = &tab[3*i];           /* couleur i : tab[3*i .. 3*i+2] */
      d = dist2(pix, candidate);

      if (d < best_dist) {
        best_dist = d;
        best_i = i;
      }
    }

    /* On réccupère dans l'image output de la meilleur couleur */
    out_color[0] = tab[3*best_i + 0];
    out_color[1] = tab[3*best_i + 1];
    out_color[2] = tab[3*best_i + 2];

    image_ecrire_pixel(output, out_color);

    /* Avancer dans input et output (même taille => même avancement) */
    cont_in  = image_pixel_suivant(input);
    cont_out = image_pixel_suivant(output);

    /* on vérifie qu'on reste synchronisé */
    assert(cont_in == cont_out);
  }
  while (cont_in);
}
