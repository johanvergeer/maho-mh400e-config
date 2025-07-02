/* Implementation of the twitching functionality. */

#include "mh400e_twitch.h"

#include <stddef.h>

/* group twitch related data and states */
static struct {
    bool want_cw;             /* next direction we want to twitch to */
    bool finished;            /* set by twitch_stop to signal when operation has
                                 completed (twitch_stop is meant to be called repeatedly
                                 in order to stop twitching while still respecting the
                                 configured delays. twitch_start() */
    long delay;               /* delay in ns to do "nothing", counted down to 0 */
    hal_bit_t *cw;            /* pointer to twitch_cw pin */
    hal_bit_t *ccw;           /* pointer to twitch_ccw pin */
    hal_bit_t *trigger_estop; /* set to true to trigger an emergency stop */
    statefunc next;           /* next twitch state function to call */
} GTwitchData;

/* Call only once, sets up the global twitch state data structure */
FUNCTION(twitch_setup) {
    /* Initialize twitch data structure */
    GTwitchData.want_cw = true;
    GTwitchData.delay = 0;
    GTwitchData.cw = &twitch_cw;
    GTwitchData.ccw = &twitch_ccw;
    GTwitchData.trigger_estop = &estop_out;
    GTwitchData.next = twitch_stop;
    GTwitchData.finished = true;
}

/* Call this function to stop twitching.
 *
 * Stops twitching, respecting the specified delay, always sets the
 * next function pointer to twitch_stop(). Returns "true" if stopping
 * is done (i.e. all delays have elapsed and both pins are off). */
static void twitch_stop(long period) {
    /* Both are off - nothing to do */
    if ((*GTwitchData.cw == false) && (*GTwitchData.ccw == false)) {
        GTwitchData.delay = 0;
        GTwitchData.next = twitch_stop;
        GTwitchData.finished = true;
    }

    /* At least one of the pins is on, respect the delay */
    if (GTwitchData.delay > 0) {
        GTwitchData.delay = GTwitchData.delay - period;
        GTwitchData.next = twitch_stop;
    }

    *GTwitchData.cw = false;
    *GTwitchData.ccw = false;
    GTwitchData.next = twitch_stop;
    GTwitchData.delay = 0;
    GTwitchData.finished = true;
}

/* Do not call this function directly, it will be setup by twitch_start().
 * Alternates between twitch_cw and twitch_ccw pins, respecting the
 * MH400E_TWITCH_KEEP_PIN_ON and MH400E_TWITCH_KEEP_PIN_OFF delays. */
static void twitch_do(long period) {
    if (GTwitchData.delay > 0) {
        GTwitchData.delay = GTwitchData.delay - period;
        GTwitchData.next = twitch_do;
        return;
    }

    if ((*GTwitchData.cw == false) && (*GTwitchData.ccw == false)) {
        if (GTwitchData.want_cw) {
            *GTwitchData.cw = true;
            GTwitchData.want_cw = false;
        } else {
            *GTwitchData.ccw = true;
            GTwitchData.want_cw = true;
        }

        GTwitchData.delay = MH400E_TWITCH_KEEP_PIN_ON;
        GTwitchData.next = twitch_do;
        return;
    } else if (*GTwitchData.cw == true) {
        *GTwitchData.cw = false;
        GTwitchData.want_cw = false;
        GTwitchData.delay = MH400E_TWITCH_KEEP_PIN_OFF;
        GTwitchData.next = twitch_do;
        return;
    } else if (*GTwitchData.ccw == true) {
        *GTwitchData.ccw = false;
        GTwitchData.want_cw = true;
        GTwitchData.delay = MH400E_TWITCH_KEEP_PIN_OFF;
        GTwitchData.next = twitch_do;
        return;
    } else /* both are never allowed to be on */
    {
        rtapi_print_msg(
            RTAPI_MSG_ERR,
            "mh400e_gearbox FATAL ERROR: twitch cw + ccw are on, triggering emergency stop!\n"
        );
        *GTwitchData.trigger_estop = true;
    }
}

/* Call this function to start twitching.
 *
 * Makes sure that we are in a defined state (both pins are off) and
 * sets up twitch_do() */
static void twitch_start(long period) {
    /* Precondition: both pins must be off before we start,
     * if they are not - stop twitching in order to get into a defined
     * state */
    if ((*GTwitchData.cw != false) || (*GTwitchData.ccw != false)) {
        twitch_stop(period);
        /* stop function always resets the next pointer to twitch_stop */
        GTwitchData.next = twitch_start;
        return;
    }

    /* Precondition is met, we can do the actual twitching now. */
    GTwitchData.next = twitch_do;
    GTwitchData.finished = false;
}

/* Wrapper to "hide" the global twitch_data structure */
static void twitch_handle(long period) {
    if (GTwitchData.next == NULL) {
        rtapi_print_msg(
            RTAPI_MSG_ERR,
            "mh400e_gearbox FATAL ERROR: twitch function not set up, triggering emergency stop!\n"
        );
        *GTwitchData.trigger_estop = true;
        return;
    }
    GTwitchData.next(period);
}

/* Returns true if stop twitching operation completed. */
static bool twitch_stop_completed(void) {
    return GTwitchData.finished;
}
