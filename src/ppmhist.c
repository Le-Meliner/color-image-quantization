#include <stdio.h>   
#include <stdlib.h>  
#include <assert.h>  

#include "image.h"
#include "histogram.h"

int main(int argc, char **argv) {

  image img;
  histo h;
  histo_iter iter;

  int rgb[3];  /* tableau où on récupère R,G et B */
  int lum;     /* luminance = (R+G+B)/3 */
  int occ;     /* nombre d'occurrences de la couleur */
  int err;     /* vérifie si il y a une erreur */
  int dim;     /* dimension de l'image */

  /* message d'erreur si la fonction est mal utilisé */ 
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input.ppm>\n", argv[0]);
    return 1;
  }

  /* Chargement de l'image */
  img = FAIRE_image();              /* alloue une structure image */
  err = image_charger(img, argv[1]); /* charge le fichier PPM dans img */

  /* dans image.h -> "Return a negative value in case of error" */
  if (err < 0) {
    fprintf(stderr, "Erreur: impossible de charger '%s' (code %d)\n", argv[1], err);

    /* on libère la mémoire alloué pour l'image*/
    DEFAIRE_image(img);
    free(img);
    return 1;
  }

  /* on vérifie la dimention */
  dim = image_give_dim(img);
  assert(dim == 3);

  /* construction de l'histogramme */
  h = create_histo();
  assert(h != NULL);
  init_histo(h, img);

  /* Parcours et affichage avec l'itérateur */
  iter = create_histo_iter();
  assert(iter != NULL);
  start_histo_iter(iter);
  give_color_histo_iter(iter, rgb);

  /* luminance */
  lum = (rgb[0] + rgb[1] + rgb[2]) / 3;

  /* fréquence */
  occ = give_freq_histo(h, rgb[0], rgb[1], rgb[2]);

  /* affichage du n-uplet */
  printf("(%d,%d,%d,%d,%d)\n", rgb[0], rgb[1], rgb[2], lum, occ);

  /* une fois que la première couleur est faire, on peut passer au prochaines */ 
  while (next_histo_iter(iter)) {
    give_color_histo_iter(iter, rgb);
    lum = (rgb[0] + rgb[1] + rgb[2]) / 3;
    occ = give_freq_histo(h, rgb[0], rgb[1], rgb[2]);
    printf("(%d,%d,%d,%d,%d)\n", rgb[0], rgb[1], rgb[2], lum, occ);
  }

  /* libération de la mémoire */
  delete_histo_iter(iter);
  delete_histo(h);
  DEFAIRE_image(img);
  free(img);

  return 0;
}


