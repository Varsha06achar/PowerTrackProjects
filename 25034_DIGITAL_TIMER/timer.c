/**
 * @file  timer.c
 * @brief Timer0 and Timer1 setup + shared ISR.
 *
 * Timer1 period:  20 MHz → Fosc/4 = 5 MHz instruction clock
 *                 Prescaler 1:8 → 625 kHz timer clock
 *                 Preload  0x85EE = 34286
 *                 Counts   65536 − 34286 = 31250
 *                 Period   31250 / 625 000 = 50 ms  ✓
 */

#include "main.h"

/* 50 ms tick counter – read by the state machine for timeouts/blink  */
volatile unsigned char timer1_ticks = 0;

/* ------------------------------------------------------------------ */
void initTimer0(void)
{
    GIE    = 1;
    PEIE   = 1;
    TMR0IE = 1;

    PSA  = 1;    /* prescaler assigned to WDT, Timer0 runs 1:1 */
    T0CS = 0;    /* internal instruction clock                  */

    TMR0   = 6;
    TMR0IF = 0;
}

/* ------------------------------------------------------------------ */
void init_timer1(void)
{
    T1CON   = 0x00U;          /* stop timer, clear all config bits    */
    T1CKPS1 = 1;              /* prescaler 1:8                        */
    T1CKPS0 = 1;
    TMR1CS  = 0;              /* internal instruction clock (Fosc/4)  */
    T1OSCEN = 0;              /* no dedicated external oscillator      */

    TMR1H = TMR1_PRELOAD_HIGH;
    TMR1L = TMR1_PRELOAD_LOW;

    TMR1IF = 0;
    TMR1IE = 1;
    PEIE   = 1;
    GIE    = 1;

    TMR1ON = 1;               /* start counting                       */
}

/* ------------------------------------------------------------------ */
void __interrupt() isr(void)
{
    /* --- Timer0: basic tick (unused in logic, kept for future use) - */
    if (TMR0IF)
    {
        TMR0   = TMR0 + 8U;   /* re-align for next period             */
        TMR0IF = 0;
    }

    /* --- Timer1: 50 ms system tick --------------------------------- */
    if (TMR1IF)
    {
        /* Reload FIRST – Timer1 resumes counting while ISR continues  */
        TMR1H = TMR1_PRELOAD_HIGH;
        TMR1L = TMR1_PRELOAD_LOW;
        TMR1IF = 0;

        timer1_ticks++;        /* unsigned char wraps 255 → 0 safely  */
        button_scan_tick();    /* update button hold counters          */
    }
}