#ifndef __SST25VF064C_H 
#define __SST25VF064C_H                                                                /* __cplusplus                 */

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define     SPI_FLASH_A_ENABLE()         LPC_GPIO4->CLR |= 1<<9
#define     SPI_FLASH_A_DISABLE()        LPC_GPIO4->SET |= 1<<9

#define     SPI_FLASH_B_ENABLE()         LPC_GPIO2->CLR |= 1<<9
#define     SPI_FLASH_B_DISABLE()        LPC_GPIO2->SET |= 1<<9

/*  
** SPI Flash命令(针对SST25VF064C)  
 */
#define   SPI_FLASH_WREN          0x06        /* write enable                 */
#define   SPI_FLASH_WRDI          0x04        /* write disable                */
#define   SPI_FLASH_JEDEC         0x9F        /* JEDEC ID读。                 */
#define   SPI_FLASH_RDSR          0x05        /* 读取状态寄存器。              */
#define   SPI_FLASH_WRSR          0x01        /* write status register        */
#define   SPI_FLASH_READ          0x03        /* read data                    */
#define   SPI_FLASH_FAST_READ     0x0B        /* fast read data               */
#define   SPI_FLASH_SE            0x20        /* 擦除4k的存储器阵列。          */
#define   SPI_FLASH_BE            0xD8        /* 擦除64k的存储器阵列。         */
#define   SPI_FLASH_CE            0x60        /* 擦除整片存储器阵列。          */
#define   SPI_FLASH_PP            0x02        /* 页编程。                     */
#define   SPI_FLASH_RDID          0x90        /* read electronic ID           */
#define   SPI_FLASH_EWSR          0x50        /* 使能写状态寄存器。            */
                                                                  

#define   MAX_ADDR                0x7FFFFF    /* 芯片内部最大地址             */
#define   SEC_SIZE                0x1000      /* 扇区大小 4096                  */
#define   PAGE_SIZE               256         /* 页大小                     */
#define   MAX_BLOCK_NUM           127         /* 最大块号                     */
#define   MAX_SECTOR_NUM	      2047        /* 最大扇区号                   */ 


/****************流水记录******************/
#define  FlashRecSADDR		      0x0         //流水记录开始地址
#define  FlashRecLen		      256         //每条流水记录所占的字节数
#define  FlashRecMaxID		      20000       //最大20000条流水记录

/****************班累记录******************/
#define  FlashShiftSADDR	      0x4E2000    //班累记录开始地址
#define  FlashShiftLen		      128         //每条班累记录所占的字节数
#define  FlashShiftMaxID		  1000        //最大1000条班累记录(注:最大可以为1024)

/*************参数修改记录***************/
#define  FlashModSADDR	          0x502000    //参数修改记录开始地址
#define  FlashModLen		      128         //每条参数修改记录所占的字节数
#define  FlashModMaxID		      1000        //最大1000条参数修改记录(注:最大可以为1024)

/*****************黑名单********************/
#define  FlashBlkSADDR	          0x522000    //黑名单记录开始地址
#define  FlashBlkLen		      8           //每条黑名单记录所占的字节数
#define  FlashBlkMaxID		      20000       //最大20000条黑名单记录(注:最大可以为20480)

/**************新增黑名单*****************/
#define  FlashAddBlkSADDR	      0x54A000    //新增黑名单记录开始地址
#define  FlashAddBlkLen		      8           //每条新增黑名单记录所占的字节数
#define  FlashAddBlkMaxID		  200         //最大200条新增黑名单记录(注:最大可以为512)

/**************新删黑名单*****************/
#define  FlashDelBlkSADDR	      0x54B000    //新删黑名单记录开始地址
#define  FlashDelBlkLen		      8           //每条新删黑名单记录所占的字节数
#define  FlashDelBlkMaxID		  200         //最大200条新删黑名单记录(注:最大可以为512)

/*****************白名单********************/
#define  FlashWhiteSADDR	      0x54C000    //白名单记录开始地址
#define  FlashWhiteLen		      8           //每条白名单记录所占的字节数
#define  FlashWhiteMaxID		  200         //最大200条白名单记录(注:最大可以为512)

/*****************测试区域********************/
#define  FlashTestSADDR	          0x54D000    //flash测试地址
#define  FlashTestSector          1357        //0x54D000/4096

void    SpiSimpleCmd(uint8_t flashnum,uint8_t cmd);
uint8_t SSTReadID(uint8_t flashnum);
uint8_t SSTReadFlashStatus(uint8_t flashnum);
void    WriteSpiFlashStatus(uint8_t flashnum,uint8_t status);
uint8_t SSTEraseSector(uint8_t flashnum,uint32_t addr);
uint8_t SSTEraseBlock(uint8_t flashnum,uint32_t addr);
void    SSTEraseChip(uint8_t flashnum);
uint8_t SSTReadNbyte(uint8_t flashnum,
                   uint32_t addr, 
                   uint8_t *dst, 
                   uint32_t num, 
                   uint8_t readmode);
uint8_t SSTWriteNbyte( uint8_t  flashnum,
                      uint32_t addr, 
                      uint8_t  *src, 
                      uint32_t num );
uint8_t SSTFlashTest(uint8_t  flashnum);

uint8_t SaveFuelRecord(uint8_t select);
uint8_t ReadFuelRecord(uint32_t id,uint8_t select);
uint8_t SaveFlash(uint8_t  flashnum,uint32_t addr, uint8_t  *src, uint32_t num );
uint8_t ReadFlash(uint8_t flashnum,uint32_t addr,uint8_t *pt,uint32_t num);
                                                             
#endif
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
