#ifndef HISTOGRAM_H
#define HISTOGRAM_H

/**
 * @file histogram.h
 * @brief Public interface for the RGB color histogram module (PPM).
 *
 * This module provides:
 * - an optimized color histogram using a 256x256 table of linked lists
 * - frequency query for a given color (R,G,B)
 * - an iterator to scan all colors present in the histogram
 * - popularity-based quantification (top-K colors) and pixel mapping (nearest color)
 *
 * Note: some structures (cell, histo_iter) are opaque here.
 * Their real definitions are private and located in histogram.c.
 */

/**
 * @brief Opaque pointer type for a list cell.
 *
 * The real struct `struct cell` is private and defined in histogram.c.
 */

#include <stdio.h>
#include "image.h"

typedef struct cell * cell; 

/**
 * @brief Histogram type: linearized 256x256 array of pointers to 'cell' lists.
 *
 * Each entry corresponds to a (R,G) pair and points to a sorted list of B values.
 */
typedef cell * histo;

/**
 * @brief Opaque iterator type to traverse all colors present in the histogram.
 *
 * The real struct `struct histo_iter` is private and defined in histogram.c.
 */
typedef struct histo_iter * histo_iter;

/**
 * @brief Boolean type required by the project statement.
 */
typedef enum { false, true } boolean;


/**
 * @brief Create a new list cell (B value + next pointer).
 *
 * @param B Blue component value (0..255).
 * @param next Pointer to the next cell (may be NULL).
 * @return Pointer to the newly allocated cell, or NULL if allocation fails.
 */
cell create_cell(int B, cell next);

/**
 * @brief Insert B into a sorted list (ascending order on B).
 *
 * - If B already exists in the list, its frequency is incremented.
 * - Otherwise, a new cell is created and inserted at the right position.
 *
 * @param head List head (may be NULL).
 * @param B Value to insert (0..255).
 * @return New list head (may change if insertion happens at the head).
 */
cell insert_cell(cell head,int B);

/**
 * @brief Free an entire linked list.
 *
 * @param list List head.
 * @return Always NULL (useful for: list = delete_list(list);).
 */
cell delete_list(cell list);

/**
 * @brief Allocate a 256x256 histogram initialized with NULL entries (empty lists).
 *
 * @return Allocated histogram, or NULL if allocation fails.
 */
histo create_histo(void);

/**
 * @brief Build/initialize the histogram from an image.
 *
 * @param h Histogram to fill.
 * @param img Input image (expected RGB PPM).
 */
void init_histo(histo h, image img);

/**
 * @brief Free all lists contained in the histogram, then free the histogram itself.
 *
 * @param h Histogram to destroy.
 */
void delete_histo(histo h);

/**
 * @brief Get the frequency of color (R,G,B) in the histogram.
 *
 * @param h Histogram.
 * @param R Red component (0..255).
 * @param G Green component (0..255).
 * @param B Blue component (0..255).
 * @return Number of occurrences of (R,G,B). Returns 0 if the color is not present.
 */
int give_freq_histo(histo h, int R, int G, int B);

/**
 * @brief Allocate an iterator positioned on the first existing color in the histogram.
 *
 * Scan order is "row by row":
 * - increasing R, then increasing G
 * - for each (R,G), B is scanned in linked-list order.
 *
 * @return Allocated iterator, or NULL if allocation fails.
 */
histo_iter create_histo_iter(void);

/**
 * @brief Reset the iterator to the beginning (first existing color).
 *
 * @param it Iterator.
 */
void start_histo_iter(histo_iter it);

/**
 * @brief Move the iterator to the next color.
 *
 * @param it Iterator.
 * @return true if the iterator could move (a color remains), false if end is reached.
 */
boolean next_histo_iter(histo_iter iter);

/**
 * @brief Return the current color (R,G,B) pointed by the iterator.
 *
 * @param it Iterator.
 * @param tab Output array of 3 ints: tab[0]=R, tab[1]=G, tab[2]=B.
 */
void give_color_histo_iter(histo_iter iter, int *tab);

/**
 * @brief Free the iterator structure.
 *
 * @param it Iterator to free.
 */
void delete_histo_iter(histo_iter iter);

/* ----- /src/quantification.c ------ */

/**
 * @brief Select the K most frequent colors and store them into tab.
 *
 * @param h Histogram.
 * @param tab Output array of size K*3: (R0,G0,B0), (R1,G1,B1), ...
 * @param K Number of colors to keep.
 */
void quantification(histo h, int* tab, int K);

/* ------ src/mapping.c ------- */ 

/**
 * @brief Replace each input pixel with its nearest color among the K colors in tab.
 *
 * @param input Input image.
 * @param output Output image (initialized to same size as input inside mapping).
 * @param tab Array of K colors (size K*3).
 * @param K Number of colors in tab.
 */
void mapping(image input, image output, int* tab, int K);


#endif
