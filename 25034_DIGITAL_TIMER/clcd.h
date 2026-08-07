#ifndef CLCD_H
#define CLCD_H

#define CLCD_DATA_PORT_DDR  TRISD
#define CLCD_RS_DDR         TRISE2
#define CLCD_EN_DDR         TRISE1
#define CLCD_DATA_PORT      PORTD
#define CLCD_RS             RE2
#define CLCD_EN             RE1

#define INST_MODE   0
#define DATA_MODE   1

#define HI   1
#define LOW  0

#define LINE1(x)  (0x80U + (x))
#define LINE2(x)  (0xC0U + (x))

#define EIGHT_BIT_MODE              0x33U
#define TWO_LINES_5x8_8_BIT_MODE    0x38U
#define CLEAR_DISP_SCREEN           0x01U
#define DISP_ON_AND_CURSOR_OFF      0x0CU

void init_clcd(void);
void clcd_putch(const char data, unsigned char addr);
void clcd_print(const char *str, unsigned char addr);
void clcd_clear(void);

#endif