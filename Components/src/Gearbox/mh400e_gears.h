/* Functions related to gear switching. */

#ifndef MH400E_GEARS_H
#define MH400E_GEARS_H

#include "mh400e_common.h"

/* One time setup function to prepare data structures related to gearbox
 * switching*/
FUNCTION(gearbox_setup);

/* Construct masks from current gearbox status pins, call this function
 * once per iteration */
static void update_current_pingroup_masks(void);

/* Combine masks from each pin group to a value representing the current
 * gear setting. A return of NULL means that a corresponding value could
 * not be found, which may indicate a gearshift being in progress- */
static PairT *get_current_gear(TreeNodeT *tree);

/* Start gear shifting, parameter specifies the target gear that we want
 * to shift to.
 * ATTENTION: this function will set the vlaue of the start_gear_shift pin
 * and also start twitching. */
static void gearshift_start(PairT *target_gear, long period);

/* Call this function once per each thread cycle to handle gearshifting,
 * implies that gearshift_start() has been called in order to set the
 * target gear.
 *
 * Incorporates the twitching handler. */
static void gearshift_handle(long period);

/* Reset pins and state machine if an emergency stop was triggered. */
static void gearbox_handle_estop(void);

/* Returns true if a gear shifting operation is currently in progress */
static bool gearshift_in_progress(void);

#endif // MH400E_GEARS_H
