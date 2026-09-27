#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "histogram.h"
#include "image.h"


/* variable global, pointeur sur l'histogramme courant */
static histo g_histo = NULL;

struct cell {
  unsigned char B;
  int freq;
  cell next;
};

struct histo_iter {
  int R, G;
  cell current;
};

cell create_cell(int B, cell next) {
  /* allocation mémoire */
  cell c = malloc(sizeof(struct cell)); 
  if (c == NULL) {
    return NULL;
  }
  /* initialisation de la nouvelle cellule */
  c->B = B;
  c->freq = 1;
  c->next = next;
  return c;
}




cell insert_cell(cell head, int B) {
  /* si la liste est vide, la première cellule est le head */
  if (head == NULL) {
    return create_cell(B, NULL);
  }
  /* Si le nouveau B est plus petit que le B du head, cette cellule prend la place du head */
  if (B < head->B) {
    cell newc = create_cell(B, head);
    if (newc == NULL) { 
      return head;
    }
    return newc; 
  }

  /* Si B à la même valeur que le head, on auguemente juste la fréquence dans le head */
  if (B == head->B) {
    head->freq++;
    return head;
  }

  /* Cas général : B est quelque part après le head */
  cell prev = head;                                   /* prev pointe sur une cellule avec prev->B < B */
  cell curr = head->next;                             /* curr (current) pointe sur la cellule suivante */


  /* Tant que curr existe, et que notre B current est iférieur au nouveau B, on avance */
  while (curr != NULL && curr->B < B) {
    prev = curr;
    curr  = curr->next;
  }

  /* ensuite, si on trouve B, on auguemente juste la fréquence */
  if (curr != NULL && curr->B == B) {
    curr->freq++;
    return head;
  }

  /* si par contre la nouvelle valeur de B n'est pas présente, on crée une nouvelle cellule */
  cell newc = create_cell(B, curr);
  if (newc == NULL) {
    return head;
  }

  prev->next = newc;
  return head;
}




cell delete_list(cell list) {
  cell curr = list;

  /* tant qu'il reste des cellules on libère la mémoire */
  while (curr != NULL) {
    cell nxt = curr->next;
    free(curr);
    curr = nxt;
  }

  return NULL; /* On a plus de liste ni de cellules */
}



histo create_histo(void) {

  histo h = calloc(256 * 256, sizeof(cell));
  /* variable globale qu'on à défini au départ, sert à récuperer l'histogramme 
   * (utile pour une prochaine fonction)*/
  g_histo = h;   
  return h;
}


void init_histo(histo h, image img) {
  /* existence */
  if (h == NULL || img == NULL_IMAGE) {
    return;
  }

  int width  = image_give_largeur(img);
  int height = image_give_hauteur(img);

  /* parcourt sur les pixels de l'image*/
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {


      /* On réccupere les valeur du pixel (y,x) */
      int *pix = image_get_pixel(img, y, x);
      int R = pix[0];
      int G = pix[1];
      int B = pix[2];
      /* On accède à la case (R,G) de l'histogramme. index = R*256 + G correspond à histo[R][G] = h[index], 
       * et qu'on met a jour  */
      int index = R * 256 + G;
      h[index] = insert_cell(h[index], B);
    }
  }
}



void delete_histo(histo h) {
  /* si l'histogramme est null */
  if (h == NULL) {
    return;
  }

  /* On parcourt toutes les h[R][G] possibles. */
  for (int index = 0; index < 256 * 256; index++) {
    h[index] = delete_list(h[index]);
  }
  /* On libère le tableau */
  free(h);
}


int give_freq_histo(histo h, int R, int G, int B) {
  /* existence */
  if (h == NULL) {
    return 0;
  }
  /* On récupère la liste h[R][G]. */
  int index = R * 256 + G;
  cell curr = h[index];   /* head du couple (R,G) */

  while (curr != NULL && curr->B < B) { 
    curr = curr->next; 
    /* La liste est croissante donc le premier "curr->B" est le plus petit,
     * donc on monte dans les valeurs de B */
  }

  if (curr != NULL && curr->B == B) {
    return curr->freq;
  }

  return 0; /* B n'est pas présent */
}


histo_iter create_histo_iter(void) {

  int is_found = 0; /* pour le assert */

  /* existence */
  assert(g_histo != NULL); /* rappel : g_histo est une variable globale */

  /* definition + allacoation mémoire de l'itérateur */
  histo_iter iter = malloc(sizeof(struct histo_iter));
  if (iter == NULL) {
    return NULL;
  }

  /* On recherche la première cellule non nulle */
  for (int R = 0; R < 256 && !is_found; R++) {
    for (int G = 0; G < 256; G++) {
      int index = R * 256 + G;

      if (g_histo[index] != NULL) {
        iter->R = R;
        iter->G = G;
        iter->current = g_histo[index]; /* première cellule de la liste */
        is_found = 1;
        break;
      }
    }
  }

  /*assert si toutes les cellules sont nulles */
  assert(is_found);
  return iter;
}

/* même principe que la fonction précédente */
void start_histo_iter(histo_iter iter) {
  /* existence */
  assert(iter != NULL);
  assert(g_histo != NULL);

  for (int R = 0; R < 256; R++) {
    for (int G = 0; G < 256; G++) {
      int index = R * 256 + G;

      if (g_histo[index] != NULL) {
        iter->R = R;
        iter->G = G;
        iter->current = g_histo[index];
        return;
      }
    }
  }

  /* assert si les entrée sont nulles */
  assert(0);
}

boolean next_histo_iter(histo_iter iter) {
  /* existence */
  assert(iter != NULL);
  assert(g_histo != NULL);

  /* Si on peut avancer au prochain B dans la liste (R,G). */
  if (iter->current != NULL && iter->current->next != NULL) {
    iter->current = iter->current->next;
    return true;
  }

  /* Sinon, on passe à la prochaine case (R,G) non vide */
  int index = iter->R * 256 + iter->G + 1;
  while (index < 256 * 256) {
    if (g_histo[index] != NULL) {
      /* On reconvertit index -> (R,G) (index = R*256 + G, donc R = index/256 et G = index%256 ) */
      iter->R = index / 256;
      iter->G = index % 256;

      /* On se place au début de la nouvelle liste */
      iter->current = g_histo[index];
      return true;
    }
    index++;
  }  

  /* si plus d'iteration possible */
  iter->current = NULL;
  return false;
}


void give_color_histo_iter(histo_iter iter, int *tab) {
  /* existence */
  assert(iter != NULL);
  assert(tab != NULL);
  assert(iter->current != NULL); /* On vérifie qu'une est bien sur un cellule */ 

  /* On remplie le tableau RGB */
  tab[0] = iter->R;
  tab[1] = iter->G;
  tab[2] = iter->current->B;
}

void delete_histo_iter(histo_iter iter)
{
  free(iter);
}

