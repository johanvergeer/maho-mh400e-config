#ifndef MH400E_COMMON_H
#define MH400E_COMMON_H

/* structure that allows to group pins together */
#define MH400E_PINS_IN_GROUP 4
typedef struct {
    hal_bit_t *p[MH400E_PINS_IN_GROUP];
} PinGroupT;

/* description of a particular gear/speed setting */
typedef struct {
    unsigned key;
    unsigned value;
} PairT;

/* Each shaft is described by 4 bits:
 *                3        2       1      0
 * msb "left center" "center" "right" "left" lsb
 *
 * The table below provides the mask for all three shafts.
 * The helper macros should be used on an extracted 4 bit value.
 */
#define MH400E_STAGE_IS_LEFT(mask) (mask & 1)
#define MH400E_STAGE_IS_RIGHT(mask) ((mask >> 1) & 1)
#define MH400E_STAGE_IS_CENTER(mask) ((mask >> 2) & 1)
#define MH400E_STAGE_IS_LEFT_CENTER(mask) ((mask >> 3) & 1)

/* lookup table from rpm to gearbox status pin values */
static PairT mh400e_gears[] = {
    /* rpm   bitmask                msb 11 10 9 8 7 6 5 4 3 2 1 0 lsb */
    {0, 4},       /* neutral           0 1 0 0 */
    {80, 1097},   /*   0 1 0 0 0 1 0 0 1 0 0 1 */
    {100, 2377},  /*   1 0 0 1 0 1 0 0 1 0 0 1 */
    {125, 585},   /*   1 0 1 0 0 1 0 0 1 0 0 1 */
    {160, 1177},  /*   0 1 0 0 1 0 0 1 1 0 0 1 */
    {200, 2457},  /*   1 0 0 1 1 0 0 1 1 0 0 1 */
    {250, 665},   /*   0 0 1 0 1 0 0 1 1 0 0 1 */
    {315, 1065},  /*   0 1 0 0 0 0 1 0 1 0 0 1 */
    {400, 2345},  /*   1 0 0 1 0 0 1 0 1 0 0 1 */
    {500, 553},   /*   0 0 1 0 0 0 1 0 1 0 0 1 */
    {630, 1090},  /*   0 1 0 0 0 1 0 0 0 0 1 0 */
    {800, 2370},  /*   1 0 0 1 0 1 0 0 0 0 1 0 */
    {1000, 578},  /*   0 0 1 0 0 1 0 0 0 0 1 0 */
    {1250, 1170}, /*   0 1 0 0 1 0 0 1 0 0 1 0 */
    {1600, 2450}, /*   1 0 0 1 1 0 0 1 0 0 1 0 */
    {2000, 658},  /*   0 0 1 0 1 0 0 1 0 0 1 0 */
    {2500, 1058}, /*   0 1 0 0 0 0 1 0 0 0 1 0 */
    {3150, 2338}, /*   1 0 0 1 0 0 1 0 0 0 1 0 */
    {4000, 546}   /*   0 0 1 0 0 0 1 0 0 0 1 0 */
};

/* Furthest CCW position, marked as "red" on the MAHO   */
#define MH400E_STAGE_POS_LEFT 9 /* 1001 */

/* Middle shaft position, marked as "bloue" on the MAHO */
#define MH400E_STAGE_POS_CENTER 4 /* 0100 */

/* Furthest CW position, marked as "yellow" on the MAHO */
#define MH400E_STAGE_POS_RIGHT 2 /* 0010 */

/* total number of selectable gears including neutral */
#define MH400E_NUM_GEARS (sizeof(mh400e_gears) / sizeof(PairT))
/* max gear index in array */
#define MH400E_MAX_GEAR_INDEX MH400E_NUM_GEARS - 1
/* index of neutral gear */
#define MH400E_NEUTRAL_GEAR_INDEX 0
/* min spindle rpm > 0 supported by the MH400E */
#define MH400E_MIN_RPM 80
/* index of the first non 0 rpm setting in the gears array */
#define MH400E_MIN_RPM_INDEX 1
/* max spindle rpm supported by the MH400E */
#define MH400E_MAX_RPM 4000

#define MH400E_TWITCH_KEEP_PIN_ON 800 * 1000000L  /* 800ms in nanoseconds */
#define MH400E_TWITCH_KEEP_PIN_OFF 200 * 1000000L /* 200ms in nanoseconds */

/* When shifting, poll the stage pins each 5ms. Picking a lower value here
 * to make sure that we do not "miss" and do not overshoot our target
 * position. */
#define MH400E_GEAR_STAGE_POLL_INTERVAL 5 * 1000000L /* 5ms in nanoseconds */

/* If reverse direction needs to be activated, we have to wait 100ms before
 * we activate the motor after the reverse pin has been activated or
 * deactivated */
#define MH400E_REVERSE_MOTOR_INTERVAL 100 * 1000000L /* 100ms in nanoseconds */

/* Interval between all remaining pin operations related to gear shifting */
#define MH400E_GENERIC_PIN_INTERVAL 100 * 1000000L /* 100ms in nanoseconds */

/* TODO: make this a module parameter */
#define MH400E_WAIT_SPINDLE_AT_SPEED 500 * 1000000L /* 500ms in nanoseconds */
/* generic state function */
typedef void (*statefunc)(long period);

#endif // MH400E_COMMON_H
