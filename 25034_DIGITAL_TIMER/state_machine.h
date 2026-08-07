#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

/* key_event_t and app_state_t are defined in digital_keypad.h / main.h
   which must be included before this header (main.h handles ordering). */


void init_state_machine(void);
void state_machine_run(key_event_t evt);

#endif /* STATE_MACHINE_H */