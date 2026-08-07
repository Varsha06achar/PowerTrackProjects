/**
 * @file  i2c.c
 * @brief Blocking I2C master driver for PIC MSSP module.
 */

#include "main.h"

/* ------------------------------------------------------------------ */
void initI2C(unsigned long baud)
{
    SSPM3 = 1; SSPM2 = 0; SSPM1 = 0; SSPM0 = 0;   /* I2C master mode */
    SSPADD  = (unsigned char)((FOSC / (4UL * baud)) - 1UL);
    TRISC3  = 1;   /* SCL – must be input for open-drain */
    TRISC4  = 1;   /* SDA – must be input for open-drain */
    SSPEN   = 1;
}

/* ------------------------------------------------------------------ */
/**
 * Spin until the MSSP is idle.
 * Returns 1 on success, 0 on timeout (bus stuck).
 */
static unsigned char i2c_wait_for_idle(void)
{
    unsigned int timeout = 1000U;

    while ((R_nW || (SSPCON2 & 0x1FU)) && (timeout > 0U))
        timeout--;

    return (timeout == 0U) ? 0U : 1U;
}

/* ------------------------------------------------------------------ */
unsigned char i2c_start(void)
{
    if (i2c_wait_for_idle() == 0U) return 0U;
    SEN = 1;
    return 1U;
}

unsigned char i2c_repeat_start(void)
{
    if (i2c_wait_for_idle() == 0U) return 0U;
    RSEN = 1;
    return 1U;
}

unsigned char i2c_stop(void)
{
    if (i2c_wait_for_idle() == 0U) return 0U;
    PEN = 1;
    return 1U;
}

/**
 * Write one byte.
 * Returns 1 if the slave ACK'd, 0 on NACK or bus error.
 */
unsigned char i2c_write(unsigned char data)
{
    if (i2c_wait_for_idle() == 0U) return 0U;
    SSPBUF = data;
    if (i2c_wait_for_idle() == 0U) return 0U;
    return (unsigned char)(!ACKSTAT);   /* ACKSTAT=0 means ACK received */
}

/**
 * Read one byte.
 * @param ack      1 = send NACK after byte (last byte),
 *                 0 = send ACK  after byte (more to follow).
 * @param out_data pointer to store received byte.
 * Returns 1 on success, 0 on timeout.
 */
unsigned char i2c_read(unsigned char ack, unsigned char *out_data)
{
    if (i2c_wait_for_idle() == 0U) return 0U;
    RCEN = 1;
    if (i2c_wait_for_idle() == 0U) return 0U;
    *out_data = SSPBUF;
    ACKDT = (ack == 1U) ? 1U : 0U;   /* 1 = NACK, 0 = ACK */
    ACKEN = 1;
    return 1U;
}