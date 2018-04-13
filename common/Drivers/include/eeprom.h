#ifndef __EEPROM_H 
#define __EEPROM_H

/*********************************************
添加用户宏定义
**********************************************/
#define READ8               0
#define READ16              1
#define READ32              2
#define WRITE8              3
#define WRITE16             4
#define WRITE32             5
#define ERASE_PROG          6
#define RD_PFETCH           (1<<3)

#define END_OF_RDWR         26
#define END_OF_PROG         28

#define RDPREFETCH          1
#define STALLED             1


#define EEPROM_CMD_8_BIT_READ           (0)
#define EEPROM_CMD_16_BIT_READ          (1)
#define EEPROM_CMD_32_BIT_READ          (2)
#define EEPROM_CMD_8_BIT_WRITE          (3)
#define EEPROM_CMD_16_BIT_WRITE         (4)
#define EEPROM_CMD_32_BIT_WRITE         (5)
#define EEPROM_CMD_ERASE_PRG_PAGE       (6)
#define EEPROM_CMD_RDPREFETCH           (1<<3)
#define EEPROM_PAGE_SIZE                64
#define EEPROM_PAGE_NUM                 64
#define EEPROM_PWRDWN                   (1<<0)
#define EEPROM_ENDOF_RW                 (26)
#define EEPROM_ENDOF_PROG               (28)

#define EEPROM_PAGE_OFFSET(n)           (n&0x3F)
#define EEPROM_PAGE_ADRESS(n)           ((n&0x3F)<<6)

//lpc1788内部eeprom地址划分，大小为(4K-64)字节
extern uint32_t    SYSPARA_ADDR;                                        //系统参数地址
#define     EEPROM_W_TIMES                  90000                       //eeprom最大写次数
#define     SYSPARA_MAX_ADDR                2048                        //eeprom参数最大地址
#define     BLACK_INFO_ADDR					3072                        //黑名单信息存储地址
#define     BLACK_DEL_INFO_ADDR				(BLACK_INFO_ADDR + 16)	    //新删黑名单信息存储地址
#define     BLACK_ADD_INFO_ADDR				(BLACK_DEL_INFO_ADDR + 16)  //新增黑名单信息存储地址
#define     WHITE_INFO_ADDR					(BLACK_ADD_INFO_ADDR + 16)  //白名单信息存储地址
#define     LOCK_ADDR                       (WHITE_INFO_ADDR+16)        //加密板存储地址。
#define     LOCKVER_ADDR                    (LOCK_ADDR+6)               //密钥存储地址。


void        EEPROMInit(void);
uint8_t     EepromInterWriteNByte(uint16_t addr,uint8_t *data,uint16_t num);
uint8_t     EepromInterReadNByte(uint16_t addr,uint8_t *data,uint16_t num);
uint8_t     SaveEeprom(uint16_t addr,uint8_t *data,uint16_t num);
uint8_t     SaveSysparas(uint16_t addr,uint8_t *data,uint16_t num);
uint8_t     ReadSysparas(uint16_t addr,uint8_t *data,uint16_t num);

#endif 
