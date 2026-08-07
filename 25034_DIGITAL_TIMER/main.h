#ifndef MAIN_H
#define MAIN_H

#include <xc.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  20000000UL
#endif

#ifndef FOSC
#define FOSC        20000000UL
#endif
/* Buzzer Hardware Configuration -------------------------------------- */
#define BUZZER_PIN       PORTCbits.RC2
#define BUZZER_TRIS      TRISCbits.TRISC2

#define BUZZER_ON        1
#define BUZZER_OFF       0
/* Application states ------------------------------------------------ */
typedef enum {
    STATE_DEFAULT,
    STATE_CONFIG_MENU,
    STATE_EVENT_MENU,
    STATE_SET_EVENT,
    STATE_VIEW_EVENT,
    STATE_TIMEDATE_MENU,
    STATE_SET_TIME,
    STATE_SET_DATE
} app_state_t;

/* Global RTC shadow -------------------------------------------------- */
typedef struct {
    unsigned char hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char date;
    unsigned char month;
    unsigned char year;
} SYSTEM_STATE;

extern SYSTEM_STATE sys;   /* defined once in main.c */

/* Timer1 preload for 50 ms @ 20 MHz, 1:8 prescaler ------------------ */
#define TMR1_PRELOAD_HIGH  0x85U
#define TMR1_PRELOAD_LOW   0xEEU

/* Sub-module headers (order matters: each may depend on types above) - */
#include "i2c.h"
#include "clcd.h"
#include "ds1307.h"
#include "timer.h"
#include "digital_keypad.h"   /* defines key_event_t, EDGE, LEVEL      */
#include "state_machine.h"

#endif /* MAIN_H */