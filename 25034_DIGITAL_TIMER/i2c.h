#ifndef I2C_H
#define I2C_H

void          initI2C(unsigned long baud);
unsigned char i2c_start(void);
unsigned char i2c_stop(void);
unsigned char i2c_repeat_start(void);
unsigned char i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ack, unsigned char *out_data);

#endif