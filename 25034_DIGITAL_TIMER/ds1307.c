#include "main.h"

void init_rtc(void)
{
    unsigned char sec_reg, hour_reg;

    if (ds1307_i2c_read(SEC_ADDRESS, &sec_reg) == 0)  return;
    ds1307_i2c_write(sec_reg & 0x7FU, SEC_ADDRESS);              // clear CH bit

    if (ds1307_i2c_read(HOUR_ADDRESS, &hour_reg) == 0) return;
    ds1307_i2c_write(hour_reg | 0x40U, HOUR_ADDRESS);            // SET bit 6 → force 12-hour mode
}

unsigned char ds1307_i2c_read(unsigned char address, unsigned char *out_data)
{
    unsigned char ack;
    unsigned char success = 1;
    if(i2c_start() == 0)    return 0;
    if(success && (i2c_write(SLAVE_WRITE) == 0))  success = 0;
    if(success && (i2c_write(address) == 0))  success = 0;
    if(success && (i2c_repeat_start() == 0)) success = 0;
    if(success && (i2c_write(SLAVE_READ) == 0))  success = 0;
    if(success && ((ack = i2c_read(1, out_data)) == 0))    success = 0;
    if(i2c_stop() == 0) success = 0;

    if(success == 0)
        return 0;
    return 1;   
}

unsigned char ds1307_i2c_write(unsigned char data, unsigned char address)
{
    unsigned char success = 1;

    if (i2c_start() == 0) return 0;

    if (success && (i2c_write(SLAVE_WRITE) == 0)) success = 0;
    if (success && (i2c_write(address) == 0))     success = 0;
    if (success && (i2c_write(data) == 0))        success = 0;

    if (i2c_stop() == 0) success = 0;   // unconditional, same reasoning as before

    return success;
}