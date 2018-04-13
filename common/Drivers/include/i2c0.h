#ifndef  __I2C0_H
#define  __I2C0_H

void I2C0Init(uint32_t ulFi2c1);
uint8_t I2C0ReadNByte(uint8_t sla, uint32_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);
uint8_t I2C0WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);
#endif


