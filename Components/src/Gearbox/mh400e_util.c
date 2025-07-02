/*
Various utilities and helper functions.
Unfortunately it is not possible to provide halcompile with multiple
sources, so we have to go the somewhat ugly way of implementing those
functions directly in the header files.
*/

/* Allocate a tree node and assign it the given key and value.
 *
 * Note: hal_malloc() does not have a corresponding free() function,
 * this is the reason why there are no corresponding tree deallocaters.
 *
 * For our use case it's anyway not a problem, because the trees are
 * built up during intialzation and not modified anymore.
 */
#include "mh400e_util.h"

#include "mh400e_common.h"

#include <math.h>
#include <stddef.h>

static TreeNodeT *tree_node_allocate(unsigned key, unsigned value) {
    TreeNodeT *tmp = (TreeNodeT *)hal_malloc(sizeof(TreeNodeT));
    if (tmp == NULL) {
        rtapi_print_msg(RTAPI_MSG_ERR, "MH400E_GEARBOX: failed to allocate memory for tree node!");
        return NULL;
    }
    tmp->key = key;
    tmp->value = value;
    tmp->left = NULL;
    tmp->right = NULL;
    return tmp;
};

static int tree_leaf_left(TreeNodeT *node) {
    TreeNodeT *temp = node;
    while (temp->left != NULL) {
        temp = temp->left;
    }
    return temp->key;
}

static int tree_leaf_right(TreeNodeT *node) {
    TreeNodeT *temp = node;
    while (temp->right != NULL) {
        temp = temp->right;
    }
    return temp->key;
}

static TreeNodeT *tree_from_sorted_array(const PairT *array, const size_t length) {
    int i;
    unsigned p = 1;

    /* find the smallest power of two larger than length */
    while (p < length) {
        p = p << 1;
    }

    TreeNodeT *ptr[p];
    for (i = 0; i < p; i++) {
        ptr[i] = NULL;
    }

    /* scale array to last "ideal" tree level */
    for (i = 0; i < length; i++) {
        unsigned j = i * p / length;
        ptr[j] = tree_node_allocate(array[i].key, array[i].value);
    }

    while (p > 1) {
        for (i = 0; i < p; i += 2) {
            /* three cases */
            if (ptr[i] && ptr[i + 1]) /* (leaf, leaf) */
            {
                TreeNodeT *dn = tree_node_allocate(0, 0);
                if (dn == NULL) {
                    /* Fatal: allocation failed, error message will
                     * be printed by the allocate function. */
                    return NULL;
                }

                dn->left = ptr[i];
                dn->right = ptr[i + 1];
                dn->key = ((tree_leaf_left(dn->right) + tree_leaf_right(dn->left)) / 2);
                ptr[i / 2] = dn;
            } else if (ptr[i]) /* (leaf, dummy) */
            {
                ptr[i / 2] = ptr[i];
            } else if (ptr[i + 1]) /* (dummy, leaf) */
            {
                ptr[i / 2] = ptr[i + 1];
            } /* (dummy, dummy) not possible due to scaling */
        }
        p = p >> 1;
    }
    /* return root node */
    return ptr[0];
}

static TreeNodeT *tree_search_closest_match(TreeNodeT *root, unsigned key) {
    if (root == NULL) {
        return NULL;
    }

    /* our values are only stored in leafs */
    if ((root->left == NULL) && (root->right == NULL)) {
        return root;
    }

    if (root->key <= key) {
        return tree_search_closest_match(root->right, key);
    }

    return tree_search_closest_match(root->left, key);
}

static TreeNodeT *tree_search(TreeNodeT *root, unsigned key) {
    TreeNodeT *result = tree_search_closest_match(root, key);
    if ((result != NULL) && (result->key == key)) {
        return result;
    }
    return NULL;
}

/* Helper function for array sorting. */
static void swap_elements(PairT *p1, PairT *p2) {
    PairT tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

static void sort_array_by_key(PairT array[], size_t length) {
    int i, j;
    for (i = 0; i < length - 1; i++) {
        for (j = 0; j < length - i - 1; j++) {
            if (array[j].key > array[j + 1].key) {
                swap_elements(&(array[j]), &(array[j + 1]));
            }
        }
    }
}

static PairT *select_gear_from_rpm(TreeNodeT *tree, float rpm) {
    TreeNodeT *result;

    /* handle two cases that do not need extra searching */
    if (rpm <= 0) {
        return &(mh400e_gears[MH400E_NEUTRAL_GEAR_INDEX]);
    } else if (rpm >= MH400E_MAX_RPM) {
        return &(mh400e_gears[MH400E_MAX_GEAR_INDEX]);
    } else if ((rpm > 0) && (rpm <= mh400e_gears[MH400E_MIN_RPM_INDEX].key)) {
        /* special case: everything >0 but lower than the lowest gear
         * should still return the lowest gear, because >0 means we want
         * the spindle to rotate */
        return &(mh400e_gears[MH400E_MIN_RPM_INDEX]);
    }

    result = tree_search_closest_match(tree, (unsigned)round(rpm));

    return &(mh400e_gears[result->value]);
}
