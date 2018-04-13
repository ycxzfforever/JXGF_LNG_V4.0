#ifndef  __I2C1_H
#define  __I2C1_H

#define    AT24C128     0xA0                                       /* AT24C128器件从机地址  0xA0写操作 0xA1读操作   */

#define ONE_BYTE_SUBA   1
#define TWO_BYTE_SUBA   2
#define X_ADD_8_SUBA    3


void I2C1_Test(void);

uint8_t At24C128WriteNByte(uint32_t addr, uint8_t *data, uint16_t num);
uint8_t At24C128ReadNByte(uint32_t addr, uint8_t *data, uint16_t num);

uint8_t I2C1_IRcvByte(uint8_t sla, uint8_t *dat);
uint8_t I2C1_ISendByte(uint8_t sla, uint8_t dat);
uint8_t I2C1_ReadNByte (uint8_t sla, uint32_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);
uint8_t I2C1_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);
void    I2C1_IRQHandler(void);
void    I2C1Init(uint32_t ulFI2C1);    

#endif
