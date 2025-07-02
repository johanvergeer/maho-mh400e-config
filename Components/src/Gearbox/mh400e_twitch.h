/* Implementation of the twitching functionality. */

#ifndef __MH400E_TWITCH_H__
#define __MH400E_TWITCH_H__

#include "mh400e_common.h"

#include <rtapi.h>

/* Call only once, sets up the global twitch state data structure */
FUNCTION(twitch_setup);

/* Call this function to start twitching.
 *
 * Makes sure that we are in a defined state (both pins are off) and
 * sets up twitch_do() */
static void twitch_start(long period);

/* Call this function once per each thread cycle to handle twitching */
static void twitch_handle(long period);

/* Call this function to stop twitching.
 *
 * Stops twitching, respecting the specified delay, always sets the
 * next function pointer to twitch_stop(). */
static void twitch_stop(long period);

/* Returns true if stop twitching operation completed. */
static bool twitch_stop_completed(void);

#endif //__MH400E_TWITCH_H__
