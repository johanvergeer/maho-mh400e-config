/*
Various utilities and helper functions.
Unfortunately it is not possible to provide halcompile with multiple
sources, so we have to go the somewhat ugly way of implementing those
functions directly in the header files.
*/

#ifndef MH400E_UTIL_H
#define MH400E_UTIL_H

#include <rtapi.h>

/* Binary search tree node. */
typedef struct TreeNode {
    unsigned key;
    unsigned value;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNodeT;

/* Allocate a tree node and assign it the given key and value.
 *
 * Note: hal_malloc() does not have a corresponding free() function,
 * this is the reason why there are no corresponding tree deallocaters.
 *
 * For our use case it's anyway not a problem, because the trees are
 * built up during intialzation and not modified anymore.
 */
static TreeNodeT *tree_node_allocate(unsigned key, unsigned value);

/* Find the key of the left leaf node. */
static int tree_leaf_left(TreeNodeT *node);

/* Find the key of the right leaf node. */
static int tree_leaf_right(TreeNodeT *node);

/* Build up a tree from a sorted array. */
static TreeNodeT *tree_from_sorted_array(const PairT *array, size_t length);

/* Return tree node by the given key.
 * If no exact match of the key was found, return the closest available.
 * This is useful when we get spindle rpm values as user in put, but
 * need to quantize them to the speeds supported by the machine. */
static TreeNodeT *tree_search_closest_match(TreeNodeT *root, unsigned key);

/* Return tree node by the given key, if there is no exact match (i.e.
 * key not found), return NULL. */
static TreeNodeT *tree_search(TreeNodeT *root, unsigned key);

/* Simple bubble sort to get our gear arrays in order. */
static void sort_array_by_key(PairT array[], size_t length);

/* Find the closest matching gear that is supported by the MH400E.
 *
 * Everything <= 0 is matched to 0. Everything >4000 is matched to 4000,
 * which is the maximum supported speed.
 * Everything in the range 0 < rpm < 80 is matched to 80 (lowest supported
 * speed), since we assume that a value higher than zero implies spindle
 * movement.
 *
 * Note, that this function does no actually perform any pin writing operations,
 * is only quantizes the input rpm to a supported speed.
 *
 * Returns speed "pair" where rpm is stored in the "key" and the pin bitmask
 * is stored in "value".
 */
static PairT *select_gear_from_rpm(TreeNodeT *tree, float rpm);

#endif // MH400E_UTIL_H
