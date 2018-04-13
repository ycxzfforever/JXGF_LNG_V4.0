/*
 * GetPassword.h
 *
 * Created: 2016-5-11 15:40:13
 *  Author: Administrator
 */ 


#ifndef GETPASSWORD_H_
#define GETPASSWORD_H_


#define S1_IN  LPC_GPIO4->DIR &= ~(1ul<<26)     // 方向-输入。
#define SI_INDATE   ((LPC_GPIO4->PIN)&(1ul<<26))//加密板设置端口。

extern uint8_t Password[6];

void LockPortInit(void);
unsigned char LockDataCompare(void);
void LockDataStore(void);
void GetLockdata(void);
#endif 
