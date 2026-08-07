/**
 * @file  main.c
 * @brief Entry point – initialise peripherals and run the state machine.
 */

#include "main.h"

#pragma config WDTE = OFF   /* watchdog off for development */

/* One and only definition of the global RTC shadow */
SYSTEM_STATE sys = {0};

/* ------------------------------------------------------------------ */
static void initConfig(void)
{
    /* Initialize Buzzer Pin as Output and turn off */
    BUZZER_TRIS = 0;
    BUZZER_PIN = BUZZER_OFF;
    init_clcd();
    initI2C(100000UL);
    init_rtc();
    init_keypad();
    initTimer0();
    init_timer1();
    init_state_machine();   /* set initial state and clear event table */
}

/* ------------------------------------------------------------------ */
int main(void)
{
    initConfig();

    while (1)
    {
        key_event_t evt = keypad_get_event();
        state_machine_run(evt);
    }

    return 0;
}