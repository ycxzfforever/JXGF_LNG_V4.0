#include "kernel.h"
/* 定义用于和I2C中断传递信息的  */
/* 全局变量                     */
volatile uint8_t     I2C0_SlaveAddr;                  /* I2C器件地址                */
volatile uint32_t    I2C0_SlaveSubAddr;               /* I2C器件存储字节地址            */
volatile uint8_t     I2C0_SlaveSubAddrNum;            /* I2C子地址字节数              */
volatile uint8_t     *I2C0_DataBuf;                   /* 数据缓冲区指针               */
volatile uint32_t    I2C0_DataNum;                    /* 要读取/写入的数据个数        */
volatile uint8_t     I2C0_EndFlag;                    /* I2C总线结束标志：结束总线总是置1  */
volatile uint8_t     I2C0_OP_Type;                    /* 子地址控制。
																							           0--子地址已经处理或者不需要子地址
																												 1--读取操作
																												 2--写操作
																											*/

/********************************************************
**函数名称:I2C0WaitEndFlag
**函数功能:等待结束标志
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t  I2C0WaitEndFlag(uint32_t  dly)
{
    uint32_t  i;
    if(I2C0_EndFlag==1) return (1);
    for(; dly>0; dly--)
    for(i=0;i<5000; i++)
    {
        if(I2C0_EndFlag==1) return (1);
    }
    return (0);
}

/********************************************************
**函数名称:I2C0WriteByte
**函数功能:向无子地址器件发送1字节数据。
**输入参数:sla     器件地址
**         dat     要发送的数据
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/

uint8_t  I2C0WriteByte(uint8_t slaveaddr, uint8_t data)
{                                                                       /* 参数设置                     */
    I2C0_SlaveAddr   = slaveaddr;                                       /* 写操作的器件地址             */
    I2C0_DataBuf     = &data;                                           /* 待发送的数据                 */
    I2C0_DataNum     = 1;                                               /* 发送1字节数据                */
    I2C0_OP_Type = 0;                                                   /* 无子地址                     */
    I2C0_EndFlag     = 0;
    LPC_I2C0->CONCLR = 0x2C;
    LPC_I2C0->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */
    return( I2C0WaitEndFlag(20));
}

/********************************************************
**函数名称:I2C0ReadByte
**函数功能:向无子地址器件读取1字节数据。
**输入参数:sla     器件地址
**         dat     接受的数据
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t  I2C0ReadByte(uint8_t slaveaddr, uint8_t *data)
{                                                                       /* 参数设置                     */
    I2C0_SlaveAddr     = slaveaddr+1;                                   /* 读操作的器件地址             */
    I2C0_DataBuf     = data;
    I2C0_DataNum     = 1;
    I2C0_OP_Type = 0;                                                    /* 无子地址                     */
    I2C0_EndFlag     = 0;
    LPC_I2C0->CONCLR = 0x2C;
    LPC_I2C0->CONSET = 0x60;                                            /* 设置为主机，并启动总线       */
    return( I2C0WaitEndFlag(20));
}

/********************************************************
**函数名称:I2C0ReadNByte
**函数功能:从有子地址器件任意地址开始读取N字节数据
**输入参数:slaveaddr      器件地址
**         subaddr_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**         subaddr        器件子地址
**         s           数据接收缓冲区指针
**        num         读取的个数
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t I2C0ReadNByte(uint8_t slaveaddr, uint32_t subaddr_type, uint32_t subaddr, uint8_t *s, uint32_t num)
{
    if(num>0)                                                        /* 判断num个数的合法性          */
    {                                                                   /* 参数设置                     */
        if(subaddr_type == 1)
        {                                                               /* 子地址为单字节               */
            I2C0_SlaveAddr         = slaveaddr + 1;                                  /* 读器件的从地址，R=1          */
            I2C0_SlaveSubAddr      = subaddr;                                     /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 1;                                        /* 器件子地址为1字节            */
        }
        if(subaddr_type == 2)
        {                                                               /* 子地址为2字节                */
            I2C0_SlaveAddr         = slaveaddr + 1;                                  /* 读器件的从地址，R=1          */
            I2C0_SlaveSubAddr      = subaddr;                                     /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 2;                                        /* 器件子地址为2字节            */
        }
        if(subaddr_type == 3)
        {                                                               /* 子地址结构为8+X              */
            I2C0_SlaveAddr         = slaveaddr + ((subaddr >> 7 )& 0x0e) + 1;           /* 读器件的从地址，R=1          */
            I2C0_SlaveSubAddr      = subaddr & 0x0ff;                             /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 1;                                        /* 器件子地址为8+x              */
        }
        I2C0_DataBuf     = s;                                                /* 数据接收缓冲区指针           */
        I2C0_DataNum     = num;                                              /* 要读取的个数                 */
        I2C0_OP_Type = 1;                                                /* 有子地址读                   */
        I2C0_EndFlag     = 0;
                                                                        /* 清除STA,SI,AA标志位          */
        LPC_I2C0->CONCLR = (1 << 2)|                                    /* AA                           */
                           (1 << 3)|                                    /* SI                           */
                           (1 << 5);                                    /* STA                          */
                                                                        /* 置位STA,启动I2C总线          */
        LPC_I2C0->CONSET = (1 << 5)|                                    /* STA                          */
                           (1 << 6);                                    /* I2CEN                        */
                                                                        /* 等待I2C操作完成              */
        return(I2C0WaitEndFlag(20));
    }
    return (FALSE);
}
/********************************************************
**函数名称:I2C0WriteNByte
**函数功能:向有子地址器件写入N字节数据
**输入参数:slaveaddr      器件地址
**         subaddr_type   子地址结构    1－单字节地址    2－8+X结构    2－双字节地址
**         subaddr        器件子地址
**         s              将要写入的数据的指针
**        num             将要写入的数据的个数
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t I2C0WriteNByte(uint8_t slaveaddr, uint8_t subaddr_type, uint32_t subaddr, uint8_t *s, uint32_t num)
{
    if (num > 0)                                                        /* 如果读取的个数为0，则返回错误*/
    {                                                                   /* 设置参数                     */
        if (subaddr_type == 1){                                            /* 子地址为单字节               */
            I2C0_SlaveAddr         = slaveaddr;                                      /* 读器件的从地址               */
            I2C0_SlaveSubAddr      = subaddr;                                     /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 1;                                        /* 器件子地址为1字节            */
        }
        if (subaddr_type == 2){                                            /* 子地址为2字节                */
            I2C0_SlaveAddr         = slaveaddr;                                      /* 读器件的从地址               */
            I2C0_SlaveSubAddr      = subaddr;                                     /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 2;                                        /* 器件子地址为2字节            */
        }
        if (subaddr_type == 3){                                            /* 子地址结构为8+X              */
            I2C0_SlaveAddr         = slaveaddr + ((subaddr >> 7 )& 0x0e);               /* 读器件的从地址               */
            I2C0_SlaveSubAddr      = subaddr & 0x0ff;                             /* 器件子地址                   */
            I2C0_SlaveSubAddrNum   = 1;                                        /* 器件子地址为8+X              */
        }
        I2C0_DataBuf     = s;                                                /* 数据                         */
        I2C0_DataNum     = num;                                              /* 数据个数                     */
        I2C0_OP_Type = 2;                                                /* 有子地址，写操作             */
        I2C0_EndFlag     = 0;                                                                        
        LPC_I2C0->CONCLR = (1 << 2)|                                    /* 清除STA,SI,AA标志位          */
                           (1 << 3)|
                           (1 << 5);
        LPC_I2C0->CONSET = (1 << 5)|                                    /* 置位STA,启动I2C总线          */
                           (1 << 6);                                    /* I2CEN                        */
        return( I2C0WaitEndFlag(20));                                      /* 等待I2C操作完成              */
    }
    return (FALSE);
}

/********************************************************
**函数名称:I2C0_IRQHandler
**函数功能:硬件I2C中断服务程序。
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void  I2C0_IRQHandler(void)
{       /*
	 *读取I2C状态寄存器I2DAT,按照全局变量的设置进行操作及设置软件标志,清除中断逻辑,中断返回
	 */
	switch (LPC_I2C0->STAT & 0xF8)
	{   /* 
		 *  根据状态码进行相应的处理     
		 */
			case 0x08:                                                      /* 已发送起始条件,主发送和主接  */
																																			/* 收都有,装入SLA+W或者SLA+R    */
			if(I2C0_OP_Type == 1)/* SLA+R */                                 /* 指定子地址读                 */
			{   LPC_I2C0->DAT = I2C0_SlaveAddr & 0xFE;                             /* 先写入地址                   */
			}
			else                                                            /* SLA+W                        */
			{   LPC_I2C0->DAT = I2C0_SlaveAddr;                                    /* 否则直接发送从机地址         */
			}
																																			/* 清零SI位                     */
			LPC_I2C0->CONCLR = (1 << 3)|                                    /* SI                           */
									 (1 << 5);                                          /* STA                          */
			break;
			case 0x10:                                                      /*已发送重复起始条件            */
																																			/* 主发送和主接收都有           */
																																			/* 装入SLA+W或者SLA+R           */
			LPC_I2C0->DAT = I2C0_SlaveAddr;                                        /* 重起总线后，重发从地址       */
			LPC_I2C0->CONCLR = 0x28;                                        /* 清零SI,STA                   */
			break;
			case 0x18:
			case 0x28:                                                      /* 已发送I2DAT中的数据,已接收ACK*/
			if (I2C0_OP_Type == 0){
					if (I2C0_DataNum > 0){
							LPC_I2C0->DAT = *I2C0_DataBuf++;
							LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
							I2C0_DataNum--;
					}
					else                                                        /* 没有数据发送了               */
					{                                                           /* 停止总线                     */
							LPC_I2C0->CONSET = (1 << 4);                            /* STO                          */
							LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
							I2C0_EndFlag = 1;                                            /* 总线已经停止                 */
					}
			}
			if(I2C0_OP_Type == 1) {                                          /* 若是指定地址读,则重新启动总线*/
					if (I2C0_SlaveSubAddrNum == 2) {
							LPC_I2C0->DAT = ((I2C0_SlaveSubAddr >> 8) & 0xff);
							LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
							I2C0_SlaveSubAddrNum--;
							break;
					}
					if(I2C0_SlaveSubAddrNum == 1) {
							LPC_I2C0->DAT = (I2C0_SlaveSubAddr & 0xff);
							LPC_I2C0->CONCLR = 0x28;                                /* 清零SI,STA                   */
							I2C0_SlaveSubAddrNum--;
							break;
					}
					if (I2C0_SlaveSubAddrNum == 0) {
							LPC_I2C0->CONCLR = 0x08;
							LPC_I2C0->CONSET = 0x20;
							I2C0_OP_Type = 0;                                        /* 子地址己处理                 */
							break;
					}
			}
			if (I2C0_OP_Type == 2) {                                         /* 指定子地址写,子地址尚未指定, */
																																			/* 则发送子地址                 */
					if (I2C0_SlaveSubAddrNum > 0) {
							if (I2C0_SlaveSubAddrNum == 2) {
									LPC_I2C0->DAT = ((I2C0_SlaveSubAddr >> 8) & 0xff);
									LPC_I2C0->CONCLR = 0x28;
									I2C0_SlaveSubAddrNum--;
									break;
							}
							if (I2C0_SlaveSubAddrNum == 1) {
									LPC_I2C0->DAT    = (I2C0_SlaveSubAddr & 0xff);
									LPC_I2C0->CONCLR = 0x28;
									I2C0_SlaveSubAddrNum--;
									I2C0_OP_Type  = 0;
									break;
							}
					}
			}
			break;
			case 0x40:                                                      /* 已发送SLA+R,已接收ACK        */
			if (I2C0_DataNum <= 1) {                                             /* 如果是最后一个字节           */
					 LPC_I2C0->CONCLR = 1 << 2;                                 /* 下次发送非应答信号           */
			}else {
					 LPC_I2C0->CONSET = 1 << 2;                                 /* 下次发送应答信号             */
			}
			LPC_I2C0->CONCLR = 0x28;                                        /* 清零SI,STA                   */
			break;
			case 0x20:                                                      /* 已发送SLA+W,已接收非应答     */
			case 0x30:                                                      /* 已发送I2DAT中的数据，已接收  */
																																			/* 非应答                       */
			case 0x38:                                                      /* 在SLA+R/W或数据字节中丢失仲  */
																																			/* 裁                           */
			case 0x48:                                                      /* 已发送SLA+R,已接收非应答     */
			LPC_I2C0->CONCLR = 0x28;
			I2C0_EndFlag = 0xFF;
			break;
			case 0x50:                                                      /* 已接收数据字节，已返回ACK    */
			*I2C0_DataBuf++ = LPC_I2C0->DAT;
			I2C0_DataNum--;
			if (I2C0_DataNum == 1)                                               /* 接收最后一个字节             */
			{   LPC_I2C0->CONCLR = 0x2C;                                    /* STA,SI,AA = 0                */
			} else
			{   
					LPC_I2C0->CONSET = 0x04;                                    /* AA=1                         */
					LPC_I2C0->CONCLR = 0x28;
			}
			break;
			case 0x58:                                                      /* 已接收数据字节，已返回非应答 */
			*I2C0_DataBuf++ = LPC_I2C0->DAT;                                     /* 读取最后一字节数据           */
			LPC_I2C0->CONSET = 0x10;                                        /* 结束总线                     */
			LPC_I2C0->CONCLR = 0x28;
			I2C0_EndFlag = 1;
			break;
			default:
			break;
	}
}

/********************************************************
**函数名称:I2C0Init
**函数功能:I2C0初始化
**输入参数:ulFi2c0    I2C0总线频率(最大400K)
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void I2C0Init(uint32_t ulFi2c0)
{
    LPC_SC->PCONP |=(0x01<<7);               /* 打开I2C0模块功率控制位       */
    LPC_SC->PCONP |=(0x01<<15);              /* 打开GPIO模块功率控制位       */
    LPC_IOCON->P0_27 &=~0x07 ;               /* P0.27配置为SDA0功能          */
	  LPC_IOCON->P0_27 |=0x01;
    LPC_IOCON->P0_28 &=~0x07 ;               /* P0.28配置为SCL0功能          */
	  LPC_IOCON->P0_28 |=0x01;
    if (ulFi2c0 > 400000) {
        ulFi2c0 = 400000;
    }
    LPC_I2C0->SCLH = (PeripheralClock/ulFi2c0)/2;                   /* 设置I2C时钟为fi2c            */
    LPC_I2C0->SCLL = (PeripheralClock/ulFi2c0)/2;
    NVIC_EnableIRQ(I2C0_IRQn);                                      /* 使能I2C中断                  */
    NVIC_SetPriority(I2C0_IRQn,1);                                  /* 设置I2C1中断优先级并使能中断 */
    LPC_I2C0->CONCLR = 0x2C;
    LPC_I2C0->CONSET = 0x40;                                        /* 使能主I2C0                  */
}


