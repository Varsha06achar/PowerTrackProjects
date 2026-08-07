/**
 * @file  state_machine.c
 * @brief Complete state machine for the Digital Timer project.
 *
 * State diagram
 * =============
 *  DEFAULT   ?(short UP/DOWN)??? CONFIG_MENU
 *  CONFIG_MENU ?(long UP, sel=0)??? EVENT_MENU
 *              ?(long UP, sel=1)??? TIMEDATE_MENU
 *              ?(long DOWN / 5 s idle)??? DEFAULT
 *  EVENT_MENU ?(long UP, sel=0)??? SET_EVENT
 *              ?(long UP, sel=1)??? VIEW_EVENT
 *              ?(long DOWN / 5 s idle)??? CONFIG_MENU
 *  SET_EVENT  ?(long UP)??? EVENT_MENU (saves entry)
 *              ?(long DOWN)??? EVENT_MENU (discards)
 *  VIEW_EVENT ?(long UP/DOWN)??? EVENT_MENU
 *  TIMEDATE_MENU ?(long UP, sel=0)??? SET_TIME
 *                ?(long UP, sel=1)??? SET_DATE
 *                ?(long DOWN / 5 s idle)??? CONFIG_MENU
 *  SET_TIME   ?(long UP)??? TIMEDATE_MENU (saves to RTC)
 *              ?(long DOWN)??? TIMEDATE_MENU (discards)
 *  SET_DATE   ?(long UP)??? TIMEDATE_MENU (saves to RTC)
 *              ?(long DOWN)??? TIMEDATE_MENU (discards)
 *
 * Key conventions (shared by every editing state)
 * ================================================
 *  UP  short  ? increment active field (with rollover)
 *  DOWN short ? advance to next field (circular)
 *  UP  long   ? confirm / go back to parent menu
 *  DOWN long  ? cancel / go back to parent menu
 */

#include "main.h"

/* ==================================================================
 * Alarm event storage (RAM only ? power-cycle clears alarms)
 * ================================================================== */
#define MAX_EVENTS  5U

typedef struct {
    unsigned char hour;     /* BCD 01?12                              */
    unsigned char minute;   /* BCD 00?59                              */
    unsigned char ampm;     /* 0 = AM, 1 = PM                         */
    unsigned char repeat;   /* 0 = Once, 1 = Daily, 2 = Weekly        */
    unsigned char dur_min;  /* BCD 00?59  alarm-duration minutes       */
    unsigned char dur_sec;  /* BCD 00?59  alarm-duration seconds       */
    unsigned char valid;    /* 1 = slot occupied                      */
} event_t;

static event_t        events[MAX_EVENTS];
static unsigned char num_events = 0;

/* ==================================================================
 * Module-level globals
 * ================================================================== */
extern volatile unsigned char timer1_ticks; /* 50 ms tick, timer.c   */

static app_state_t   current_state = STATE_DEFAULT;
static unsigned char needs_redraw  = 1;

/* Shared LCD line buffers ? only one state handler active at a time  */
static char g_line1[17];
static char g_line2[17];

/* Repeat-type character lookup                                       */
static const char RCHAR[3] = { 'O', 'D', 'W' };

/* Timing constants                                                   */
#define IDLE_TIMEOUT_TICKS  100U   /* 100 × 50 ms = 5 s               */
#define BLINK_HALF_TICKS     10U   /*  10 × 50 ms = 500 ms            */
#define RTC_READ_TICKS        2U   /*   2 × 50 ms = 100 ms            */
#define BUZZER_DURATION_TICKS 200U /* 200 × 50 ms = 10 s            */

/* Alarm state tracker variables */
static unsigned char alarm_active = 0;
static unsigned char buzzer_start_tick = 0;
static unsigned char triggered_event_idx = 0xFFU;

/* ==================================================================
 * BCD helper
 * ================================================================== */
/**
 * Increment a BCD value and roll over: max_v ? min_v.
 * Intermediate carry may temporarily produce non-BCD bytes; the final
 * > max_v check corrects everything.
 */
static unsigned char bcd_inc(unsigned char val,
                              unsigned char min_v,
                              unsigned char max_v)
{
    unsigned char lo = val & 0x0FU;
    unsigned char hi = val >> 4U;

    lo++;
    if (lo > 9U) { lo = 0U; hi++; }
    val = (unsigned char)((hi << 4U) | lo);

    if (val > max_v) val = min_v;
    return val;
}

/* ==================================================================
 * STATE_DEFAULT
 *
 * Display cycle (7 s, driven by timer1_ticks):
 *   0?4 s : LINE1 = "DATE DD-MM-YY    "
 *           LINE2 = "TIME HH:MM AM/PM"
 *   5?6 s : LINE1 = "TIME HH:MM AM/PM"
 *           LINE2 = "EVENT HH:MM P R "  (next alarm, if any)
 * ================================================================== */
static void handle_default(key_event_t evt)
{
    static unsigned char last_sec       = 0xFFU;
    static unsigned char last_phase     = 0xFFU;
    static unsigned char last_read_tick = 0xFFU;

    unsigned char d, is_pm, tick_sec, cycle, phase;

    /* Rate-limit RTC reads to once per 100 ms                         */
    if ((unsigned char)(timer1_ticks - last_read_tick) >= RTC_READ_TICKS)
    {
        last_read_tick = timer1_ticks;
        if (ds1307_i2c_read(SEC_ADDRESS,  &d)) sys.seconds = d;
        if (ds1307_i2c_read(MIN_ADDRESS,  &d)) sys.minutes = d;
        if (ds1307_i2c_read(HOUR_ADDRESS, &d)) sys.hours   = d;
        if (ds1307_i2c_read(DATE_ADDRESS, &d)) sys.date    = d;
        if (ds1307_i2c_read(MON_ADDRESS,  &d)) sys.month   = d;
        if (ds1307_i2c_read(YEAR_ADDRESS, &d)) sys.year    = d;

        /* Check for Event Match & Trigger Buzzer Alarm */
        if (!alarm_active)
        {
            unsigned char ei;
            unsigned char cur_h = sys.hours & 0x1FU;   /* BCD hour 01-12 */
            unsigned char cur_m = sys.minutes & 0x7FU; /* BCD min 00-59  */
            unsigned char cur_pm = (sys.hours >> 5) & 0x01U;

            for (ei = 0; ei < MAX_EVENTS; ei++)
            {
                if (events[ei].valid &&
                    events[ei].hour == cur_h &&
                    events[ei].minute == cur_m &&
                    events[ei].ampm == cur_pm)
                {
                    BUZZER_PIN = BUZZER_ON;
                    alarm_active = 1;
                    buzzer_start_tick = timer1_ticks;
                    triggered_event_idx = ei;
                    break;
                }
            }
        }
    }

    /* Handle 10-Second Buzzer Auto Turn-Off */
    if (alarm_active)
    {
        if ((unsigned char)(timer1_ticks - buzzer_start_tick) >= BUZZER_DURATION_TICKS)
        {
            BUZZER_PIN = BUZZER_OFF;
            alarm_active = 0;
            
            /* If repeat mode was 'Once' (0), mark event invalid after ringing */
            if (triggered_event_idx < MAX_EVENTS && events[triggered_event_idx].repeat == 0)
            {
                events[triggered_event_idx].valid = 0;
                if (num_events > 0) num_events--;
            }
            triggered_event_idx = 0xFFU;
        }
    }

    /* 7-second display cycle                                         */
    tick_sec = (unsigned char)(timer1_ticks / 20U);
    cycle    = (unsigned char)(tick_sec % 7U);
    phase    = (cycle >= 5U && num_events > 0U) ? 1U : 0U;

    if (sys.seconds != last_sec || phase != last_phase || needs_redraw)
    {
        last_sec     = sys.seconds;
        last_phase   = phase;
        needs_redraw = 0;
        is_pm        = (sys.hours >> 5) & 0x01U;

        if (phase == 0U)
        {
            /* ---- DATE line ---- */
            g_line1[0]='D'; g_line1[1]='A'; g_line1[2]='T';
            g_line1[3]='E'; g_line1[4]=' ';
            g_line1[5]  = (char)('0' + ((sys.date  >> 4) & 0x03U));
            g_line1[6]  = (char)('0' + ( sys.date        & 0x0FU));
            g_line1[7]  = '-';
            g_line1[8]  = (char)('0' + ((sys.month >> 4) & 0x01U));
            g_line1[9]  = (char)('0' + ( sys.month       & 0x0FU));
            g_line1[10] = '-';
            g_line1[11] = (char)('0' + ((sys.year  >> 4) & 0x0FU));
            g_line1[12] = (char)('0' + ( sys.year        & 0x0FU));
            g_line1[13] = g_line1[14] = g_line1[15] = ' ';
            g_line1[16] = '\0';

            /* ---- TIME line ---- */
            g_line2[0]='T'; g_line2[1]='I'; g_line2[2]='M';
            g_line2[3]='E'; g_line2[4]=' ';
            g_line2[5]  = (char)('0' + ((sys.hours   >> 4) & 0x01U));
            g_line2[6]  = (char)('0' + ( sys.hours         & 0x0FU));
            g_line2[7]  = ':';
            g_line2[8]  = (char)('0' + ((sys.minutes >> 4) & 0x07U));
            g_line2[9]  = (char)('0' + ( sys.minutes       & 0x0FU));
            g_line2[10] = ' ';
            g_line2[11] = is_pm ? 'P' : 'A';
            g_line2[12] = 'M';
            g_line2[13] = g_line2[14] = g_line2[15] = ' ';
            g_line2[16] = '\0';
        }
        else
        {
            unsigned char ei;
            event_t *ev = 0;

            /* ---- TIME line (top) ---- */
            g_line1[0]='T'; g_line1[1]='I'; g_line1[2]='M';
            g_line1[3]='E'; g_line1[4]=' ';
            g_line1[5]  = (char)('0' + ((sys.hours   >> 4) & 0x01U));
            g_line1[6]  = (char)('0' + ( sys.hours         & 0x0FU));
            g_line1[7]  = ':';
            g_line1[8]  = (char)('0' + ((sys.minutes >> 4) & 0x07U));
            g_line1[9]  = (char)('0' + ( sys.minutes       & 0x0FU));
            g_line1[10] = ' ';
            g_line1[11] = is_pm ? 'P' : 'A';
            g_line1[12] = 'M';
            g_line1[13] = g_line1[14] = g_line1[15] = ' ';
            g_line1[16] = '\0';

            /* find first valid event                                   */
            for (ei = 0; ei < MAX_EVENTS; ei++) {
                if (events[ei].valid) { ev = &events[ei]; break; }
            }

            if (ev)
            {
                /* "EVENT HH:MM PM R" */
                g_line2[0]='E'; g_line2[1]='V'; g_line2[2]='E';
                g_line2[3]='N'; g_line2[4]='T'; g_line2[5]=' ';
                g_line2[6]  = (char)('0' + ((ev->hour   >> 4) & 0x01U));
                g_line2[7]  = (char)('0' + ( ev->hour         & 0x0FU));
                g_line2[8]  = ':';
                g_line2[9]  = (char)('0' + ((ev->minute >> 4) & 0x07U));
                g_line2[10] = (char)('0' + ( ev->minute       & 0x0FU));
                g_line2[11] = ' ';
                g_line2[12] = ev->ampm ? 'P' : 'A';
                g_line2[13] = 'M';
                g_line2[14] = ' ';
                g_line2[15] = RCHAR[ev->repeat];
                g_line2[16] = '\0';
            }
            else
            {
                /* Shouldn't reach here (phase==1 only if events>0)   */
                g_line2[0]='N'; g_line2[1]='O'; g_line2[2]=' ';
                g_line2[3]='E'; g_line2[4]='V'; g_line2[5]='E';
                g_line2[6]='N'; g_line2[7]='T'; g_line2[8]='S';
                g_line2[9]=g_line2[10]=g_line2[11]=g_line2[12]=' ';
                g_line2[13]=g_line2[14]=g_line2[15]=' ';
                g_line2[16]='\0';
            }
        }

        clcd_print(g_line1, LINE1(0));
        clcd_print(g_line2, LINE2(0));
    }

    /* Any short press ? enter Config Menu                             */
    if (evt == KEY_EVENT_UP_SHORT || evt == KEY_EVENT_DOWN_SHORT)
    {
        last_sec    = 0xFFU;
        last_phase  = 0xFFU;
        current_state = STATE_CONFIG_MENU;
        needs_redraw  = 1;
        clcd_clear();
    }
}

/* ==================================================================
 * STATE_CONFIG_MENU
 *
 * LINE1: [?] SET/VIEW EVENT
 * LINE2: [?] SET TIME/DATE
 * Arrow moves between items.
 * 5-second idle ? back to DEFAULT.
 * ================================================================== */
static void draw_config_menu(unsigned char sel)
{
    /* 0x7E = right-arrow on most HD44780 displays                     */
    clcd_print(sel == 0U ? "\x7e SET/VIEW EVENT"
                         : "  SET/VIEW EVENT", LINE1(0));
    clcd_print(sel == 0U ? "  SET TIME/DATE "
                         : "\x7e SET TIME/DATE ", LINE2(0));
}

static void handle_config_menu(key_event_t evt)
{
    static unsigned char inited     = 0;
    static unsigned char sel        = 0;
    static unsigned char idle_start = 0;

    if (!inited)
    {
        inited     = 1;
        sel        = 0;
        idle_start = timer1_ticks;
        draw_config_menu(sel);
    }

    /* 5-second idle timeout ? back to DEFAULT                        */
    if ((unsigned char)(timer1_ticks - idle_start) >= IDLE_TIMEOUT_TICKS)
    {
        inited        = 0;
        current_state = STATE_DEFAULT;
        needs_redraw  = 1;
        clcd_clear();
        return;
    }

    if (evt == KEY_EVENT_NONE) return;

    idle_start = timer1_ticks;   /* any key resets the idle timer     */

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
        case KEY_EVENT_DOWN_SHORT:
            sel ^= 1U;
            draw_config_menu(sel);
            break;

        case KEY_EVENT_UP_LONG:
            inited        = 0;
            current_state = (sel == 0U) ? STATE_EVENT_MENU
                                        : STATE_TIMEDATE_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_DEFAULT;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_EVENT_MENU
 *
 * LINE1: [?] SET EVENT
 * LINE2: [?] VIEW EVENT
 * ================================================================== */
static void draw_event_menu(unsigned char sel)
{
    clcd_print(sel == 0U ? "\x7e SET EVENT     "
                         : "  SET EVENT     ", LINE1(0));
    clcd_print(sel == 0U ? "  VIEW EVENT    "
                         : "\x7e VIEW EVENT    ", LINE2(0));
}

static void handle_event_menu(key_event_t evt)
{
    static unsigned char inited     = 0;
    static unsigned char sel        = 0;
    static unsigned char idle_start = 0;

    if (!inited)
    {
        inited     = 1;
        sel        = 0;
        idle_start = timer1_ticks;
        draw_event_menu(sel);
    }

    if ((unsigned char)(timer1_ticks - idle_start) >= IDLE_TIMEOUT_TICKS)
    {
        inited        = 0;
        current_state = STATE_DEFAULT;
        needs_redraw  = 1;
        clcd_clear();
        return;
    }

    if (evt == KEY_EVENT_NONE) return;
    idle_start = timer1_ticks;

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
        case KEY_EVENT_DOWN_SHORT:
            sel ^= 1U;
            draw_event_menu(sel);
            break;

        case KEY_EVENT_UP_LONG:
            inited        = 0;
            current_state = (sel == 0U) ? STATE_SET_EVENT
                                        : STATE_VIEW_EVENT;
            needs_redraw  = 1;
            clcd_clear();
            break;

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_CONFIG_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_SET_EVENT
 *
 * LINE1:  "TIME- HH:MM PM R"   (R = repeat: O/D/W)
 * LINE2:  "DUR - MM:SS      "
 *
 * Fields (cycled with DOWN short):
 *   0=hour  1=min  2=ampm  3=repeat  4=dur_min  5=dur_sec
 *
 * UP  short ? increment active field
 * DOWN short ? next field
 * UP  long  ? save new event, back to EVENT_MENU
 * DOWN long ? discard, back to EVENT_MENU
 * ================================================================== */
#define SET_EVENT_FIELDS  6U

static void draw_set_event(unsigned char field, unsigned char blink,
                            unsigned char eh,  unsigned char em,
                            unsigned char ap,  unsigned char rep,
                            unsigned char dm,  unsigned char ds)
{
    /* LINE1: "TIME- HH:MM PM R" ------------------------------------ */
    g_line1[0]='T'; g_line1[1]='I'; g_line1[2]='M';
    g_line1[3]='E'; g_line1[4]='-'; g_line1[5]=' ';

    g_line1[6]  = (field==0U && !blink) ? ' '
                : (char)('0' + ((eh >> 4) & 0x01U));
    g_line1[7]  = (field==0U && !blink) ? ' '
                : (char)('0' + ( eh       & 0x0FU));
    g_line1[8]  = ':';
    g_line1[9]  = (field==1U && !blink) ? ' '
                : (char)('0' + ((em >> 4) & 0x07U));
    g_line1[10] = (field==1U && !blink) ? ' '
                : (char)('0' + ( em       & 0x0FU));
    g_line1[11] = ' ';
    g_line1[12] = (field==2U && !blink) ? ' ' : (ap ? 'P' : 'A');
    g_line1[13] = (field==2U && !blink) ? ' ' : 'M';
    g_line1[14] = ' ';
    g_line1[15] = (field==3U && !blink) ? ' ' : RCHAR[rep];
    g_line1[16] = '\0';

    /* LINE2: "DUR - MM:SS      " ----------------------------------- */
    g_line2[0]='D'; g_line2[1]='U'; g_line2[2]='R';
    g_line2[3]='-'; g_line2[4]=' ';

    g_line2[5]  = (field==4U && !blink) ? ' '
                : (char)('0' + ((dm >> 4) & 0x07U));
    g_line2[6]  = (field==4U && !blink) ? ' '
                : (char)('0' + ( dm       & 0x0FU));
    g_line2[7]  = ':';
    g_line2[8]  = (field==5U && !blink) ? ' '
                : (char)('0' + ((ds >> 4) & 0x07U));
    g_line2[9]  = (field==5U && !blink) ? ' '
                : (char)('0' + ( ds       & 0x0FU));
    g_line2[10]=g_line2[11]=g_line2[12]=g_line2[13]=g_line2[14]=' ';
    g_line2[15]=' '; g_line2[16]='\0';

    clcd_print(g_line1, LINE1(0));
    clcd_print(g_line2, LINE2(0));
}

static void handle_set_event(key_event_t evt)
{
    static unsigned char inited      = 0;
    static unsigned char field       = 0;
    static unsigned char e_hour      = 0x01U;
    static unsigned char e_min       = 0x00U;
    static unsigned char e_ampm      = 0;
    static unsigned char e_repeat    = 0;
    static unsigned char e_dur_min   = 0x00U;
    static unsigned char e_dur_sec   = 0x00U;
    static unsigned char blink       = 1;
    static unsigned char last_blink  = 0;

    if (!inited)
    {
        inited      = 1;
        field       = 0;
        e_hour      = 0x01U;
        e_min       = 0x00U;
        e_ampm      = 0;
        e_repeat    = 0;
        e_dur_min   = 0x00U;
        e_dur_sec   = 0x00U;
        blink       = 1;
        last_blink  = timer1_ticks;
        draw_set_event(field, blink, e_hour, e_min,
                       e_ampm, e_repeat, e_dur_min, e_dur_sec);
    }

    /* Blink active field at 1 Hz */
    if ((unsigned char)(timer1_ticks - last_blink) >= BLINK_HALF_TICKS)
    {
        last_blink = timer1_ticks;
        blink ^= 1U;
        draw_set_event(field, blink, e_hour, e_min,
                       e_ampm, e_repeat, e_dur_min, e_dur_sec);
    }

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
            switch (field)
            {
                case 0: e_hour    = bcd_inc(e_hour,   0x01U, 0x12U); break;
                case 1: e_min     = bcd_inc(e_min,    0x00U, 0x59U); break;
                case 2: e_ampm   ^= 1U;                               break;
                case 3: e_repeat  = (unsigned char)((e_repeat+1U)%3U);break;
                case 4: e_dur_min = bcd_inc(e_dur_min,0x00U, 0x59U); break;
                case 5: e_dur_sec = bcd_inc(e_dur_sec,0x00U, 0x59U); break;
                default: break;
            }
            blink = 1;
            draw_set_event(field, blink, e_hour, e_min,
                           e_ampm, e_repeat, e_dur_min, e_dur_sec);
            break;

        case KEY_EVENT_DOWN_SHORT:
            field = (unsigned char)((field + 1U) % SET_EVENT_FIELDS);
            blink = 1;
            draw_set_event(field, blink, e_hour, e_min,
                           e_ampm, e_repeat, e_dur_min, e_dur_sec);
            break;

        case KEY_EVENT_UP_LONG:
        {
            /* Save event to first free slot */
            unsigned char i;
            if (num_events < MAX_EVENTS)
            {
                for (i = 0; i < MAX_EVENTS; i++)
                {
                    if (!events[i].valid)
                    {
                        events[i].hour    = e_hour;
                        events[i].minute  = e_min;
                        events[i].ampm    = e_ampm;
                        events[i].repeat  = e_repeat;
                        events[i].dur_min = e_dur_min;
                        events[i].dur_sec = e_dur_sec;
                        events[i].valid   = 1;
                        num_events++;
                        break;
                    }
                }
            }
            inited        = 0;
            current_state = STATE_EVENT_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;
        }

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_EVENT_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_VIEW_EVENT
 *
 * LINE1: "N T - HH:MM PM R"  (N = slot index 0-4)
 * LINE2: "  D - MM:SS      "
 *
 * UP  short ? next event (wraps)
 * DOWN short ? delete current event
 * UP/DOWN long ? back to EVENT_MENU
 * ================================================================== */
static void draw_view_event(unsigned char idx)
{
    event_t *ev;

    if (num_events == 0U)
    {
        clcd_print("  NO EVENTS     ", LINE1(0));
        clcd_print("                ", LINE2(0));
        return;
    }

    ev = &events[idx];

    /* LINE1 */
    g_line1[0]  = (char)('0' + idx);
    g_line1[1]  = ' ';
    g_line1[2]  = 'T'; g_line1[3]=' '; g_line1[4]='-'; g_line1[5]=' ';
    g_line1[6]  = (char)('0' + ((ev->hour   >> 4) & 0x01U));
    g_line1[7]  = (char)('0' + ( ev->hour         & 0x0FU));
    g_line1[8]  = ':';
    g_line1[9]  = (char)('0' + ((ev->minute >> 4) & 0x07U));
    g_line1[10] = (char)('0' + ( ev->minute       & 0x0FU));
    g_line1[11] = ' ';
    g_line1[12] = ev->ampm ? 'P' : 'A';
    g_line1[13] = 'M';
    g_line1[14] = ' ';
    g_line1[15] = RCHAR[ev->repeat];
    g_line1[16] = '\0';

    /* LINE2 */
    g_line2[0]=' '; g_line2[1]=' ';
    g_line2[2]='D'; g_line2[3]=' '; g_line2[4]='-'; g_line2[5]=' ';
    g_line2[6]  = (char)('0' + ((ev->dur_min >> 4) & 0x07U));
    g_line2[7]  = (char)('0' + ( ev->dur_min       & 0x0FU));
    g_line2[8]  = ':';
    g_line2[9]  = (char)('0' + ((ev->dur_sec >> 4) & 0x07U));
    g_line2[10] = (char)('0' + ( ev->dur_sec       & 0x0FU));
    g_line2[11]=g_line2[12]=g_line2[13]=g_line2[14]=g_line2[15]=' ';
    g_line2[16] = '\0';

    clcd_print(g_line1, LINE1(0));
    clcd_print(g_line2, LINE2(0));
}

/** Scan forward from 'from' to find next valid slot. */
static unsigned char next_valid(unsigned char from)
{
    unsigned char i, idx;
    idx = (unsigned char)((from + 1U) % MAX_EVENTS);
    for (i = 0; i < MAX_EVENTS; i++)
    {
        if (events[idx].valid) return idx;
        idx = (unsigned char)((idx + 1U) % MAX_EVENTS);
    }
    return from;   /* only one (or zero) event left */
}

static void handle_view_event(key_event_t evt)
{
    static unsigned char inited = 0;
    static unsigned char ev_idx = 0;
    unsigned char i;

    if (!inited)
    {
        inited  = 1;
        ev_idx  = 0;
        for (i = 0; i < MAX_EVENTS; i++) {
            if (events[i].valid) { ev_idx = i; break; }
        }
        draw_view_event(ev_idx);
    }

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
            if (num_events > 0U) {
                ev_idx = next_valid(ev_idx);
                draw_view_event(ev_idx);
            }
            break;

        case KEY_EVENT_DOWN_SHORT:
            /* Delete current event */
            if (num_events > 0U)
            {
                events[ev_idx].valid = 0;
                num_events--;
                if (num_events > 0U)
                    ev_idx = next_valid(ev_idx);
                draw_view_event(ev_idx);
            }
            break;

        case KEY_EVENT_UP_LONG:
        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_EVENT_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_TIMEDATE_MENU
 *
 * LINE1: [?] SET TIME
 * LINE2: [?] SET DATE
 * ================================================================== */
static void draw_timedate_menu(unsigned char sel)
{
    clcd_print(sel == 0U ? "\x7e SET TIME      "
                         : "  SET TIME      ", LINE1(0));
    clcd_print(sel == 0U ? "  SET DATE      "
                         : "\x7e SET DATE      ", LINE2(0));
}

static void handle_timedate_menu(key_event_t evt)
{
    static unsigned char inited     = 0;
    static unsigned char sel        = 0;
    static unsigned char idle_start = 0;

    if (!inited)
    {
        inited     = 1;
        sel        = 0;
        idle_start = timer1_ticks;
        draw_timedate_menu(sel);
    }

    if ((unsigned char)(timer1_ticks - idle_start) >= IDLE_TIMEOUT_TICKS)
    {
        inited        = 0;
        current_state = STATE_DEFAULT;
        needs_redraw  = 1;
        clcd_clear();
        return;
    }

    if (evt == KEY_EVENT_NONE) return;
    idle_start = timer1_ticks;

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
        case KEY_EVENT_DOWN_SHORT:
            sel ^= 1U;
            draw_timedate_menu(sel);
            break;

        case KEY_EVENT_UP_LONG:
            inited        = 0;
            current_state = (sel == 0U) ? STATE_SET_TIME : STATE_SET_DATE;
            needs_redraw  = 1;
            clcd_clear();
            break;

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_CONFIG_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_SET_TIME
 *
 * LINE1: "    HH:MM:SS AM " (static label)
 * LINE2: "    HH:MM:SS AM " (live values, active field blinks)
 *
 * Fields: 0=hour  1=min  2=sec  3=ampm
 *
 * DS1307 hour byte (12-h mode):  bit6=1  bit5=PM  bit4=H10  bits3:0=H1
 * We store:  e_hour = bits4:0  (BCD 01?12 in 5 bits)
 *            e_ampm = bit5
 * Write back: 0x40 | (e_ampm<<5) | e_hour
 * ================================================================== */
#define SET_TIME_FIELDS  4U

static void draw_set_time(unsigned char field, unsigned char blink,
                           unsigned char eh, unsigned char em,
                           unsigned char es, unsigned char ap)
{
    unsigned char i;

    /* LINE1: static label */
    g_line1[0]=g_line1[1]=g_line1[2]=g_line1[3]=' ';
    g_line1[4]='H'; g_line1[5]='H'; g_line1[6]=':';
    g_line1[7]='M'; g_line1[8]='M'; g_line1[9]=':';
    g_line1[10]='S'; g_line1[11]='S'; g_line1[12]=' ';
    g_line1[13]='A'; g_line1[14]='M'; g_line1[15]=' ';
    g_line1[16]='\0';

    /* LINE2: live values */
    g_line2[0]=g_line2[1]=g_line2[2]=g_line2[3]=' ';

    g_line2[4]  = (field==0U && !blink) ? ' '
                : (char)('0' + ((eh >> 4) & 0x01U));
    g_line2[5]  = (field==0U && !blink) ? ' '
                : (char)('0' + ( eh       & 0x0FU));
    g_line2[6]  = ':';
    g_line2[7]  = (field==1U && !blink) ? ' '
                : (char)('0' + ((em >> 4) & 0x07U));
    g_line2[8]  = (field==1U && !blink) ? ' '
                : (char)('0' + ( em       & 0x0FU));
    g_line2[9]  = ':';
    g_line2[10] = (field==2U && !blink) ? ' '
                : (char)('0' + ((es >> 4) & 0x07U));
    g_line2[11] = (field==2U && !blink) ? ' '
                : (char)('0' + ( es       & 0x0FU));
    g_line2[12] = ' ';
    g_line2[13] = (field==3U && !blink) ? ' ' : (ap ? 'P' : 'A');
    g_line2[14] = (field==3U && !blink) ? ' ' : 'M';
    g_line2[15] = ' '; g_line2[16] = '\0';

    clcd_print(g_line1, LINE1(0));
    clcd_print(g_line2, LINE2(0));
}

static void handle_set_time(key_event_t evt)
{
    static unsigned char inited     = 0;
    static unsigned char field      = 0;
    static unsigned char e_hour     = 0x01U;
    static unsigned char e_min      = 0x00U;
    static unsigned char e_sec      = 0x00U;
    static unsigned char e_ampm     = 0;
    static unsigned char blink      = 1;
    static unsigned char last_blink = 0;
    unsigned char d;

    if (!inited)
    {
        inited     = 1;
        field      = 0;
        blink      = 1;
        last_blink = timer1_ticks;

        /* Load current RTC values */
        if (ds1307_i2c_read(HOUR_ADDRESS, &d))
        {
            e_ampm = (d >> 5) & 0x01U;
            e_hour = d & 0x1FU;   /* bits 4:0 = BCD hour (01?12) */
        }
        if (ds1307_i2c_read(MIN_ADDRESS, &d)) e_min = d;
        if (ds1307_i2c_read(SEC_ADDRESS, &d)) e_sec = d & 0x7FU; /* mask CH */

        draw_set_time(field, blink, e_hour, e_min, e_sec, e_ampm);
    }

    if ((unsigned char)(timer1_ticks - last_blink) >= BLINK_HALF_TICKS)
    {
        last_blink = timer1_ticks;
        blink ^= 1U;
        draw_set_time(field, blink, e_hour, e_min, e_sec, e_ampm);
    }

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
            switch (field)
            {
                case 0: e_hour = bcd_inc(e_hour, 0x01U, 0x12U); break;
                case 1: e_min  = bcd_inc(e_min,  0x00U, 0x59U); break;
                case 2: e_sec  = bcd_inc(e_sec,  0x00U, 0x59U); break;
                case 3: e_ampm ^= 1U;                              break;
                default: break;
            }
            blink = 1;
            draw_set_time(field, blink, e_hour, e_min, e_sec, e_ampm);
            break;

        case KEY_EVENT_DOWN_SHORT:
            field = (unsigned char)((field + 1U) % SET_TIME_FIELDS);
            blink = 1;
            draw_set_time(field, blink, e_hour, e_min, e_sec, e_ampm);
            break;

        case KEY_EVENT_UP_LONG:
        {
            /* Write back to DS1307
               hour byte: bit6=1(12h) | bit5=PM | bits4:0=BCD hour   */
            unsigned char hour_byte =
                0x40U | (unsigned char)(e_ampm << 5U) | e_hour;
            ds1307_i2c_write(e_sec,     SEC_ADDRESS);
            ds1307_i2c_write(e_min,     MIN_ADDRESS);
            ds1307_i2c_write(hour_byte, HOUR_ADDRESS);
            inited        = 0;
            current_state = STATE_TIMEDATE_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;
        }

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_TIMEDATE_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * STATE_SET_DATE
 *
 * LINE1: "    DD-MM-YY    " (static label)
 * LINE2: "    DD-MM-YY    " (live values, active field blinks)
 *
 * Fields: 0=day  1=month  2=year(2-digit BCD)
 * ================================================================== */
#define SET_DATE_FIELDS  3U

static void draw_set_date(unsigned char field, unsigned char blink,
                           unsigned char ed, unsigned char em,
                           unsigned char ey)
{
    /* LINE1: static label */
    g_line1[0]=g_line1[1]=g_line1[2]=g_line1[3]=' ';
    g_line1[4]='D'; g_line1[5]='D'; g_line1[6]='-';
    g_line1[7]='M'; g_line1[8]='M'; g_line1[9]='-';
    g_line1[10]='Y'; g_line1[11]='Y';
    g_line1[12]=g_line1[13]=g_line1[14]=g_line1[15]=' ';
    g_line1[16]='\0';

    /* LINE2: live values */
    g_line2[0]=g_line2[1]=g_line2[2]=g_line2[3]=' ';

    g_line2[4]  = (field==0U && !blink) ? ' '
                : (char)('0' + ((ed >> 4) & 0x03U));
    g_line2[5]  = (field==0U && !blink) ? ' '
                : (char)('0' + ( ed       & 0x0FU));
    g_line2[6]  = '-';
    g_line2[7]  = (field==1U && !blink) ? ' '
                : (char)('0' + ((em >> 4) & 0x01U));
    g_line2[8]  = (field==1U && !blink) ? ' '
                : (char)('0' + ( em       & 0x0FU));
    g_line2[9]  = '-';
    g_line2[10] = (field==2U && !blink) ? ' '
                : (char)('0' + ((ey >> 4) & 0x0FU));
    g_line2[11] = (field==2U && !blink) ? ' '
                : (char)('0' + ( ey       & 0x0FU));
    g_line2[12]=g_line2[13]=g_line2[14]=g_line2[15]=' ';
    g_line2[16]='\0';

    clcd_print(g_line1, LINE1(0));
    clcd_print(g_line2, LINE2(0));
}

static void handle_set_date(key_event_t evt)
{
    static unsigned char inited     = 0;
    static unsigned char field      = 0;
    static unsigned char e_day      = 0x01U;
    static unsigned char e_mon      = 0x01U;
    static unsigned char e_year     = 0x24U;   /* default 2024 */
    static unsigned char blink      = 1;
    static unsigned char last_blink = 0;
    unsigned char d;

    if (!inited)
    {
        inited     = 1;
        field      = 0;
        blink      = 1;
        last_blink = timer1_ticks;

        if (ds1307_i2c_read(DATE_ADDRESS, &d)) e_day  = d;
        if (ds1307_i2c_read(MON_ADDRESS,  &d)) e_mon  = d;
        if (ds1307_i2c_read(YEAR_ADDRESS, &d)) e_year = d;

        draw_set_date(field, blink, e_day, e_mon, e_year);
    }

    if ((unsigned char)(timer1_ticks - last_blink) >= BLINK_HALF_TICKS)
    {
        last_blink = timer1_ticks;
        blink ^= 1U;
        draw_set_date(field, blink, e_day, e_mon, e_year);
    }

    switch (evt)
    {
        case KEY_EVENT_UP_SHORT:
            switch (field)
            {
                case 0: e_day  = bcd_inc(e_day,  0x01U, 0x31U); break;
                case 1: e_mon  = bcd_inc(e_mon,  0x01U, 0x12U); break;
                case 2: e_year = bcd_inc(e_year, 0x00U, 0x99U); break;
                default: break;
            }
            blink = 1;
            draw_set_date(field, blink, e_day, e_mon, e_year);
            break;

        case KEY_EVENT_DOWN_SHORT:
            field = (unsigned char)((field + 1U) % SET_DATE_FIELDS);
            blink = 1;
            draw_set_date(field, blink, e_day, e_mon, e_year);
            break;

        case KEY_EVENT_UP_LONG:
            ds1307_i2c_write(e_day,  DATE_ADDRESS);
            ds1307_i2c_write(e_mon,  MON_ADDRESS);
            ds1307_i2c_write(e_year, YEAR_ADDRESS);
            inited        = 0;
            current_state = STATE_TIMEDATE_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        case KEY_EVENT_DOWN_LONG:
            inited        = 0;
            current_state = STATE_TIMEDATE_MENU;
            needs_redraw  = 1;
            clcd_clear();
            break;

        default: break;
    }
}

/* ==================================================================
 * Public API
 * ================================================================== */
void init_state_machine(void)
{
    unsigned char i;
    current_state = STATE_DEFAULT;
    needs_redraw  = 1;
    num_events    = 0;
    for (i = 0; i < MAX_EVENTS; i++) events[i].valid = 0;
}

void state_machine_run(key_event_t evt)
{
    switch (current_state)
    {
        case STATE_DEFAULT:       handle_default(evt);       break;
        case STATE_CONFIG_MENU:   handle_config_menu(evt);   break;
        case STATE_EVENT_MENU:    handle_event_menu(evt);    break;
        case STATE_SET_EVENT:     handle_set_event(evt);     break;
        case STATE_VIEW_EVENT:    handle_view_event(evt);    break;
        case STATE_TIMEDATE_MENU: handle_timedate_menu(evt); break;
        case STATE_SET_TIME:      handle_set_time(evt);      break;
        case STATE_SET_DATE:      handle_set_date(evt);      break;
        default:
            current_state = STATE_DEFAULT;
            needs_redraw  = 1;
            clcd_clear();
            break;
    }
}