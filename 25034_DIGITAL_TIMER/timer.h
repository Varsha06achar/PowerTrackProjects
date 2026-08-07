#ifndef TIMER_H
#define TIMER_H

/**
 * Incremented every 50 ms by the Timer1 ISR.
 * unsigned char → wraps at 255 (≈12.75 s); use subtraction for
 * all comparisons so wrapping is handled correctly.
 */
extern volatile unsigned char timer1_ticks;

void initTimer0(void);
void init_timer1(void);

#endif /* TIMER_H */