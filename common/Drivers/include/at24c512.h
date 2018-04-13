#ifndef __AT24C512_H
#define __AT24C512_H
void    At24c512Init(void);
uint8_t At24C512WriteNByte(uint32_t addr, uint8_t *data, uint16_t num);
uint8_t At24C512ReadNByte(uint32_t addr, uint8_t *data, uint32_t num);
#endif
