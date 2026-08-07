#include "main.h"

#define SW_DOWN   RB0
#define SW_UP     RB1
#define PRESSED   0    // pull-up wiring: switch to ground, pressed reads LOW

#define LONG_PRESS_THRESHOLD_TICKS  20   // 20 x 50ms = 1 second

typedef struct {
    unsigned int  hold_counter;
    unsigned char action_fired;   // latch: prevents re-triggering long-press every tick while held
} button_state_t;

static button_state_t btn_up;
static button_state_t btn_down;
static volatile key_event_t pending_event = KEY_EVENT_NONE;

void init_keypad(void)
{
    TRISB0 = 1;   // input
    TRISB1 = 1;   // input
    OPTION_REGbits.nRBPU = 0;   // enable PORTB internal weak pull-ups

    btn_up.hold_counter   = 0;
    btn_up.action_fired   = 0;
    btn_down.hold_counter = 0;
    btn_down.action_fired = 0;

    pending_event = KEY_EVENT_NONE;
}

static void scan_one_button(unsigned char pin_pressed, button_state_t *btn,
                             key_event_t short_evt, key_event_t long_evt)
{
    if (pin_pressed == PRESSED)
    {
        if (btn->hold_counter < 0xFFFF)
            btn->hold_counter++;

        if (btn->hold_counter == LONG_PRESS_THRESHOLD_TICKS && btn->action_fired == 0)
        {
            pending_event = long_evt;
            btn->action_fired = 1;
        }
    }
    else
    {
        if (btn->hold_counter > 0 && btn->hold_counter < LONG_PRESS_THRESHOLD_TICKS
            && btn->action_fired == 0)
        {
            pending_event = short_evt;
        }
        btn->hold_counter = 0;
        btn->action_fired = 0;
    }
}

void button_scan_tick(void)
{
    scan_one_button(SW_UP,   &btn_up,   KEY_EVENT_UP_SHORT,   KEY_EVENT_UP_LONG);
    scan_one_button(SW_DOWN, &btn_down, KEY_EVENT_DOWN_SHORT, KEY_EVENT_DOWN_LONG);
}

key_event_t keypad_get_event(void)
{
    key_event_t evt = pending_event;
    pending_event = KEY_EVENT_NONE;   // consume it
    return evt;
}