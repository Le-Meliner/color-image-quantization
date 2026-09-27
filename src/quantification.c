#include <stdio.h>
#include <stdlib.h>  
#include <assert.h>  

#include "histogram.h"

void quantification(histo h, int* tab, int K)
{
  histo_iter iter;
  int rgb[3];
  int freq;

  int *top_rgb;   /* tableau temporaire pour stocker les couleurs retenues (taille K*3) */
  int *top_freq;  /* tableau temporaire pour stocker leurs fréquences (taille K) */

  int count;      /* nombre de couleurs actuellement stockées */
  int i;
  int pos;        /* position d'insertion dans la liste */

  /* cas d'erreur : on vérifie que l'histogramme, le tableau et que K soit défini correctement */
  assert(h != NULL);
  assert(tab != NULL);
  assert(K > 0);

  /* Allocation des tableaux temporaires (pour garder les "K meilleur'")*/
  top_rgb = malloc(3 * K * sizeof(int));
  top_freq = malloc(K * sizeof(int));
  assert(top_rgb != NULL);
  assert(top_freq != NULL);

  iter = create_histo_iter();
  assert(iter != NULL);
  start_histo_iter(iter);

  /* Initialisation : on prend les K premières couleurs rencontrées et on les insère 
   en gardant top_freq trié par ordre décroissant. */
  count = 0;
  
  /* On commence par la couleur courrante */
  give_color_histo_iter(iter, rgb);
  freq = give_freq_histo(h, rgb[0], rgb[1], rgb[2]);
 
  top_freq[0] = freq;
  top_rgb[0] = rgb[0];
  top_rgb[1] = rgb[1];
  top_rgb[2] = rgb[2];
  count = 1;

  /* puis on va jusqu'a K */
  while (count < K && next_histo_iter(iter)) {
    give_color_histo_iter(iter, rgb);
    freq = give_freq_histo(h, rgb[0], rgb[1], rgb[2]);

    /* On d'assure que l'ordre est décroissant */
    pos = count;
    while (pos > 0 && freq > top_freq[pos - 1]) {
      /* Décalage vers la droite pour faire de la place sur le tableau trié et faire l'insertion */
      top_freq[pos] = top_freq[pos - 1];

      top_rgb[3*pos + 0] = top_rgb[3*(pos - 1) + 0];
      top_rgb[3*pos + 1] = top_rgb[3*(pos - 1) + 1];
      top_rgb[3*pos + 2] = top_rgb[3*(pos - 1) + 2];

      pos--;
    }

    top_freq[pos] = freq;
    top_rgb[3*pos + 0] = rgb[0];
    top_rgb[3*pos + 1] = rgb[1];
    top_rgb[3*pos + 2] = rgb[2];

    count++;
  }

  /* Parcours des couleurs restantes : si on a déjà K couleurs stockées, on compare à la plus petite des K (top_freq[K-1]). 
   * Si freq est plus petite, on ignore. Sinon on insère et on retire la dernière. */
  while (next_histo_iter(iter)) {
    give_color_histo_iter(iter, rgb);
    freq = give_freq_histo(h, rgb[0], rgb[1], rgb[2]);

    /* Si on n'a pas encore K couleurs (image avec peu de couleurs) , on fait une insertion triée et on augmente count. 
       Permet d'eviter d'avoir des cases non initialisé */
    if (count < K) {
      pos = count;
      while (pos > 0 && freq > top_freq[pos - 1]) {
        top_freq[pos] = top_freq[pos - 1];

        top_rgb[3*pos + 0] = top_rgb[3*(pos - 1) + 0];
        top_rgb[3*pos + 1] = top_rgb[3*(pos - 1) + 1];
        top_rgb[3*pos + 2] = top_rgb[3*(pos - 1) + 2];

        pos--;
      }

      top_freq[pos] = freq;
      top_rgb[3*pos + 0] = rgb[0];
      top_rgb[3*pos + 1] = rgb[1];
      top_rgb[3*pos + 2] = rgb[2];

      count++;
    } else {
      /* count == K : top_freq[K-1] est la plus petite fréquence des K retenues */
      if (freq < top_freq[K - 1]) {
        /* Rien a faire ! */ 
      } else {
        /* On insère à la bonne place, et on décale */
        pos = K - 1;
        while (pos > 0 && freq > top_freq[pos - 1]) {
          top_freq[pos] = top_freq[pos - 1];

          top_rgb[3*pos + 0] = top_rgb[3*(pos - 1) + 0];
          top_rgb[3*pos + 1] = top_rgb[3*(pos - 1) + 1];
          top_rgb[3*pos + 2] = top_rgb[3*(pos - 1) + 2];

          pos--;
        }

        top_freq[pos] = freq;
        top_rgb[3*pos + 0] = rgb[0];
        top_rgb[3*pos + 1] = rgb[1];
        top_rgb[3*pos + 2] = rgb[2];
      }
    }
  }

  /* Transfert dans tab (taille K*3) On copie les couleurs retenues dans tab. */

  for (i = 0; i < count; i++) {
    tab[3*i + 0] = top_rgb[3*i + 0];
    tab[3*i + 1] = top_rgb[3*i + 1];
    tab[3*i + 2] = top_rgb[3*i + 2];
  }

  /* Free */ 
  delete_histo_iter(iter);
  free(top_rgb);
  free(top_freq);
}
