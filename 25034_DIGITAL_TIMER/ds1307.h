#ifndef DS1307_H
#define DS1307_H

#define SLAVE_WRITE     0xD0U
#define SLAVE_READ      0xD1U
#define SEC_ADDRESS     0x00U
#define MIN_ADDRESS     0x01U
#define HOUR_ADDRESS    0x02U
#define DAY_ADDRESS     0x03U   // day-of-week — not used for display here
#define DATE_ADDRESS    0x04U   // day-of-month — CORRECTED from your 0x03 mix-up
#define MON_ADDRESS     0x05U
#define YEAR_ADDRESS    0x06U

void          init_rtc(void);
unsigned char ds1307_i2c_read(unsigned char address, unsigned char *out_data);
unsigned char ds1307_i2c_write(unsigned char data, unsigned char address);

#endif