
#ifndef __CTRL_H__
#define __CTRL_H__


/*********************************************************************************************************
  使能端口管脚定义
*********************************************************************************************************/
#define BEEP      		        (1ul << 5) 	 //P1.5			 
#define BEEP_ON()     	        LPC_GPIO1->DIR |= BEEP;LPC_GPIO1->SET = BEEP 
#define BEEP_OFF()              LPC_GPIO1->DIR |= BEEP;LPC_GPIO1->CLR = BEEP

#define LED1       		        (1ul << 16)  //P4.16			 
#define LED1_OFF()     	        LPC_GPIO4->DIR |= LED1;LPC_GPIO4->SET = LED1 
#define LED1_ON()      	        LPC_GPIO4->DIR |= LED1;LPC_GPIO4->CLR = LED1
#define LED1STATUS              (LPC_GPIO4->SET & LED1)

#define ZIGBEE_CFG              (1ul << 4)  //p2.4 zigbee模块cfg
#define ZIGBEE_CFGH             LPC_GPIO2->DIR |= ZIGBEE_CFG;LPC_GPIO2->SET = ZIGBEE_CFG    
#define ZIGBEE_CFGL             LPC_GPIO2->DIR |= ZIGBEE_CFG;LPC_GPIO2->CLR = ZIGBEE_CFG

#define ZIGBEE_RET              (1ul << 5)  //p2.5 zigbee模块ret
#define ZIGBEE_RETH             LPC_GPIO2->DIR |= ZIGBEE_RET;LPC_GPIO2->SET = ZIGBEE_RET    
#define ZIGBEE_RETL             LPC_GPIO2->DIR |= ZIGBEE_RET;LPC_GPIO2->CLR = ZIGBEE_RET

//以下为P2和P14两路开关量输入
#define SwitchInput1            (1ul << 0)  //p5.0
#define SI1STATUS               (LPC_GPIO5->PIN & SwitchInput1)

#define SwitchInput2            (1ul << 30) //p2.30
#define SI2STATUS               (LPC_GPIO2->PIN & SwitchInput2)


#define WDI				        (1ul << 1) 	 //P5.1
#define	WDI_H()	 		        LPC_GPIO5->SET = WDI
#define	WDI_L()	 		        LPC_GPIO5->CLR = WDI 
#define	WATCH_DOG()	 	        WDI_H();WDI_L()	

#define WDDIR()					(LPC_GPIO5->DIR |= WDI)//看门狗方向。
#define FEEDDOG()				(LPC_GPIO5->PIN ^= WDI)//喂狗。

/********************************************************************
说明：PLC端口方向控制。
*********************************************************************/
#define PUMPCTRL_HIGH            (1ul << 24)//p3.24
#define PUMPCTRL_HIGH_OPEN()     LPC_GPIO3->DIR |= PUMPCTRL_HIGH;LPC_GPIO3->CLR = PUMPCTRL_HIGH
#define PUMPCTRL_HIGH_CLOSE()    LPC_GPIO3->DIR |= PUMPCTRL_HIGH;LPC_GPIO3->SET = PUMPCTRL_HIGH

#define PUMPCTRL_LOW             (1ul << 25)//p3.25
#define PUMPCTRL_LOW_OPEN()      LPC_GPIO3->DIR |= PUMPCTRL_LOW;LPC_GPIO3->CLR = PUMPCTRL_LOW
#define PUMPCTRL_LOW_CLOSE()     LPC_GPIO3->DIR |= PUMPCTRL_LOW;LPC_GPIO3->SET = PUMPCTRL_LOW


/********************************************************************
说明：电磁阀端口方向控制。
*********************************************************************/
#define VCTRL_HIGH              (1ul << 22)//p3.22
#define VCTRL_HIGH_OPEN()       LPC_GPIO3->DIR |= VCTRL_HIGH;LPC_GPIO3->CLR = VCTRL_HIGH
#define VCTRL_HIGH_CLOSE()      LPC_GPIO3->DIR |= VCTRL_HIGH;LPC_GPIO3->SET = VCTRL_HIGH

#define VCTRL_MID               (1ul << 21)//p3.21
#define VCTRL_MID_OPEN()        LPC_GPIO3->DIR |= VCTRL_MID;LPC_GPIO3->CLR = VCTRL_MID
#define VCTRL_MID_CLOSE()       LPC_GPIO3->DIR |= VCTRL_MID;LPC_GPIO3->SET = VCTRL_MID

#define VCTRL_LOW               (1ul << 20)//p3.20
#define VCTRL_LOW_OPEN()        LPC_GPIO3->DIR |= VCTRL_LOW;LPC_GPIO3->CLR = VCTRL_LOW
#define VCTRL_LOW_CLOSE()       LPC_GPIO3->DIR |= VCTRL_LOW;LPC_GPIO3->SET = VCTRL_LOW

#define VCTRL_KEY               (1ul << 23)//p3.23
#define VCTRL_OPENKEY()         LPC_GPIO3->DIR |= VCTRL_KEY;LPC_GPIO3->CLR = VCTRL_KEY
#define VCTRL_CLOSEKEY()	    LPC_GPIO3->DIR |= VCTRL_KEY;LPC_GPIO3->SET = VCTRL_KEY


//串口号定义
#define CardPort       0
#define PrintPort      0
#define ModbusPort     1
#define LCDPort        2
#define PCPort         3
#define RfidPort       4


#define Ulong unsigned long long 
	

enum    Sys_Err                //系统异常原因
{
    SYSERR_FLASH_A_CHK,         //主存Flash检测失败 0
    SYSERR_FLASH_B_CHK,         //副存Flash检测失败 1
    SYSERR_W_FLASH_A,           //写主存错2
    SYSERR_R_FLASH_A,           //读主存错3
    SYSERR_W_FLASH_B,           //写副存错4
    SYSERR_R_FLASH_B,           //读副存错5
    SYSERR_W_IN_EEPROM,         //写内部eeprom错误 6   
    SYSERR_R_IN_EEPROM,         //读内部eeprom错误 7
    SYSERR_W_EX_EEPROM,         //写外部eeprom错误 8
    SYSERR_R_EX_EEPROM,         //读外部eeprom错误 9
    SYSERR_INANDEX_EEPROM,      //内部eeprom和外部eeprom读出数据不一致 10  
    SYSERR_T_FM25L04,           //铁电测试错误 11
    SYSERR_MODBUS_G_AUTO_ERR,   //气相流量计通讯错误 12
    SYSERR_MODBUS_L_AUTO_ERR,   //液相流量计通讯错误 13
    SYSERR_PRESSURE,            //压力传感器异常14
};


/********************************/
void    GPIOInit(void);
void    BeepOut(uint8_t N_100ms);
void    RunLed1(void);


#endif

