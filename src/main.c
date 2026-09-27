#include <stdio.h>   
#include <stdlib.h>  

#include "histogram.h"
#include "image.h"

int main(int argc, char **argv) {   
  int K;
  int err;
  int *tab;

  image input;
  image output;

  histo h;

  /* Vérification des arguments */ 
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <K> <input.ppm> > output.ppm\n", argv[0]);
    return 1;
  }

  /* Lecture de K */
  K = atoi(argv[1]);
  if (K <= 0) {
    fprintf(stderr, "Erreur: K doit être un entier strictement positif.\n");
    return 1;
  }

  /*  Chargement de l'image d'entrée (module image fourni) */
  input = FAIRE_image();
  err = image_charger(input, argv[2]);
  if (err < 0) {
    fprintf(stderr, "Erreur: impossible de charger '%s' (code %d)\n", argv[2], err);
    DEFAIRE_image(input);
    free(input);
    return 1;
  }

  /* Construction de l'histogramme */
  h = create_histo();
  init_histo(h, input);

  /* Quantification */
  tab = malloc(3 * K * sizeof(int));
  if (tab == NULL) {
    fprintf(stderr, "Erreur: malloc(tab) a échoué.\n");
    delete_histo(h);
    DEFAIRE_image(input);
    free(input);
    return 1;
  }

  quantification(h, tab, K);

  /* Mapping (créer l'image output) */
  output = FAIRE_image();
  mapping(input, output, tab, K);

  /* Écriture de l'image résultat sur stdout -> on redirige dans un fichier : ./quantif K input.ppm > output.ppm */
  image_to_stream(output, stdout);

  /* Free */
  free(tab);
  delete_histo(h);
  DEFAIRE_image(output);
  free(output);
  DEFAIRE_image(input);
  free(input);

  return 0;
}
