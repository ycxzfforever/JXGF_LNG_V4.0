#ifndef __MT318_H
#define __MT318_H

#define     CARD_RESPONSE_MAX_TIME  500000  //读卡器通讯超时时间

enum    MT318KeyAB 
{
    KEYA = 0x32,     //A密
    KEYB = 0x39,     //B密
};

typedef struct Mt318Reader
{
  uint8_t   recbuf[120];    //读卡器接收数据buffer
  uint8_t   flag;           //等于1时表示接收的数据可用
  uint8_t   err_code;       //读卡器返回的错误代码
}Mt318Reader;

extern  Mt318Reader mt318reader;

void     UART0_IRQHandler( void );
void     Mt318RecDataCheck(void);

uint8_t Mt318CrcVerify(uint8_t *array,uint8_t length);
uint8_t Mt318GetCardStatus(void);
uint8_t Mt318GetCardSerialNum(void);
uint8_t Mt318VerifyKey(uint8_t keyAB,uint8_t * key,uint8_t secter_num);
uint8_t Mt318ReadM1Card(uint8_t keyAB,uint8_t * key,uint8_t blocks, uint8_t* rdbuf);
uint8_t Mt318WriteM1Card(uint8_t keyAB,uint8_t * key,uint8_t blocks, uint8_t *wtbuf);



#endif

