#ifndef DIGITAL_KEYPAD_H
#define DIGITAL_KEYPAD_H

#define EDGE 1
#define LEVEL 0

typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_UP_SHORT,
    KEY_EVENT_UP_LONG,
    KEY_EVENT_DOWN_SHORT,
    KEY_EVENT_DOWN_LONG
} key_event_t;

void init_keypad(void);
void button_scan_tick(void);          // called from Timer1 ISR every 50ms
key_event_t keypad_get_event(void);   // called from main loop — consumes one pending event

#endif