#include "kernel.h"

volatile uint32_t UART0Status, UART1Status;
volatile uint32_t UART2Status, UART3Status;
volatile uint32_t UART4Status;

volatile uint8_t UART0TxEmpty = 1, UART1TxEmpty = 1;
volatile uint8_t UART2TxEmpty = 1, UART3TxEmpty = 1;
volatile uint8_t UART4TxEmpty = 1;

uint8_t UART0Buffer[BUFSIZE], UART1Buffer[50];
volatile uint8_t UART2Buffer[BUFSIZE], UART3Buffer[BUFSIZE];
volatile uint8_t UART4Buffer[BUFSIZE];

volatile uint32_t UART0Count = 0, UART1Count = 0;
volatile uint32_t UART2Count = 0, UART3Count = 0;
volatile uint32_t UART4Count = 0;


uint8_t UART2_IRQ_FLAG = 0;
uint8_t UART3_IRQ_FLAG = 0;
uint8_t UART4_IRQ_FLAG = 0;





/*********************************************************************************************************
** 函数名称：UART2_IRQHandler
** 函数描述：UART2中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART2_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART2->IIR;

    uiIIRValue >>= 1;                          /* 中断标志寄存器值右移1位 */
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {             /* 接收线状态中断 */
        uiLSRValue = LPC_UART2->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART2Status = uiLSRValue;
            uiDummy = LPC_UART2->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {          /* 接收FIFO不为空中断*/
            UART2Buffer[UART2Count] = LPC_UART2->RBR;
            UART2Count++;
            if ( UART2Count == BUFSIZE ) {
                UART2Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {       /* 接收数据可用中断 */
        UART2Buffer[UART2Count] = LPC_UART2->RBR;
        UART2Count++;
        if ( UART2Count == BUFSIZE ) {
            UART2Count = 0;
        }
		if(UART2Count>=(UART2Buffer[0]+1)){
		    UART2_IRQ_FLAG = 1;
		}
    }
    else if ( uiIIRValue == IIR_CTI ) {       /* 字符超时中断 */
        UART2Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {      /* 发送保持寄存器空中断*/
        uiLSRValue = LPC_UART2->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART2TxEmpty = 1;
        } else {
            UART2TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** 函数名称：UART3_IRQHandler
** 函数描述：UART3中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART3_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART3->IIR;

    uiIIRValue >>= 1;                       /* 中断标志寄存器值右移1位*/
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {          /* 接收线状态中断*/
        uiLSRValue = LPC_UART3->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART3Status = uiLSRValue;
            uiDummy = LPC_UART3->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {       /* 接收FIFO不为空中断 */
			UART3Buffer[UART3Count++] = LPC_UART3->RBR;
			if(UART3Buffer[0]!=0xaa)
			{
				UART3Count=0;
			}
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {     /* 接收数据可用中断*/
			UART3Buffer[UART3Count++] = LPC_UART3->RBR;
			if(UART3Buffer[0]!=0xaa)
			{
				UART3Count=0;
			}
    }
    else if ( uiIIRValue == IIR_CTI ) {     /* 字符超时中断 */
        UART3Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {    /* 发送保持寄存器空中断 */
        uiLSRValue = LPC_UART3->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART3TxEmpty = 1;
        } else {
            UART3TxEmpty = 0;
        }
    }
}


/*********************************************************************************************************
** 函数名称：UART4_IRQHandler
** 函数描述：UART4中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART4_IRQHandler (void) 
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART4->IIR;

    uiIIRValue >>= 1;                        /* 中断标志寄存器值右移1位*/
    uiIIRValue &= 0x07;

    if ( uiIIRValue == IIR_RLS ) {           /* 接收线状态中断 */
        uiLSRValue = LPC_UART4->LSR;
        /* 
         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
         */
        if ( uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) ) {
            UART4Status = uiLSRValue;
            uiDummy = LPC_UART4->RBR;
            return;
        }
        if ( uiLSRValue & LSR_RDR ) {         /* 接收FIFO不为空中断 */
            UART4Buffer[UART4Count] = LPC_UART4->RBR;
            UART4Count++;
            if ( UART4Count == BUFSIZE ) {
                UART4Count = 0;
            }
        }
    }
    else if ( uiIIRValue == IIR_RDA ) {       /* 接收数据可用中断*/
        UART4Buffer[UART4Count] = LPC_UART4->RBR;
        UART4Count++;
        if ( UART4Count == BUFSIZE ) {
            UART4Count = 0;
        }
		if(UART4Count>=(UART4Buffer[0]+1)){
			UART4_IRQ_FLAG = 1;
		}
    }
    else if ( uiIIRValue == IIR_CTI ) {       /* 字符超时中断 */
        UART4Status |= 0x100;
    }
    else if ( uiIIRValue == IIR_THRE ) {      /* 发送保持寄存器空中断 */
        uiLSRValue = LPC_UART4->LSR;
        if ( uiLSRValue & LSR_THRE ) {
            UART4TxEmpty = 1;
        } else {
            UART4TxEmpty = 0;
        }
    }
}

/*********************************************************************************************************
** 函数名称：UARTInit
** 函数描述：串口初始化
** 输入参数：PortNum    UART端口号
**           bandrate   设定的串口波特率
** 返回值  ：无
*********************************************************************************************************/
uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate )
{
    uint32_t Fdiv;

    if ( PortNum == 0 ) {
        //SELECT_CARD();   
        //外设功率控制寄存器 32位
		LPC_SC->PCONP |= 0x00000008;				 

        LPC_IOCON->P0_2  &= ~0x07;
        LPC_IOCON->P0_2  |= 1;                   /* P0.2为U0_TXD  */
        LPC_IOCON->P0_3  &= ~0x07;
        LPC_IOCON->P0_3  |= 1;                   /* P0.3为U0_RXD  */
        
        /*
                        * 8为数据位，无奇偶检验位，1位停止位
                         */
        LPC_UART0->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;     /*求除数值  */
        LPC_UART0->DLM = Fdiv / 256;
        LPC_UART0->DLL = Fdiv % 256;
        LPC_UART0->LCR = 0x03;                   /* 锁定除数访问*/
        LPC_UART0->FCR = 0x07;                   /* 使能并复位FIFO*/
        NVIC_SetPriority(UART0_IRQn,4); 
        NVIC_EnableIRQ(UART0_IRQn);
        /* 
                        * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
                         */
        LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;
        return (TRUE);
    }
    else if ( PortNum == 1 ) {
        LPC_SC->PCONP |= 0x00000010;

        LPC_IOCON->P2_0  &= ~0x07;
        LPC_IOCON->P2_0  |= 2;                   /* P2.0为U1_TXD */
        LPC_IOCON->P2_1  &= ~0x07;
        LPC_IOCON->P2_1  |= 2;                   /* P2.1为U1_TXD */

        /*
                         * 8位数据位，偶检验位，1位停止位
                        */
//       LPC_UART1->LCR = 0x83;
        LPC_UART1->LCR = 0x03<<0|0x01<<3|0x01<<4|0x01<<7;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ;   /*求除数值 */
        LPC_UART1->DLM = Fdiv / 256;
        LPC_UART1->DLL = Fdiv % 256;
//        LPC_UART1->LCR = 0x03;                                                                 /* 锁定除数访问 */
		LPC_UART1->LCR = 0x03<<0|0x01<<3|0x01<<4;
        LPC_UART1->FCR = 0x07;                   /* 使能并复位FIFO  */

        LPC_IOCON->P2_5 &=~0x07;
        LPC_IOCON->P2_5 |= 0x02;

        LPC_UART1->RS485CTRL = 0x38;    //电路上相比其他串口多了三极管
        LPC_UART1->RS485DLY = 0;
        NVIC_SetPriority(UART0_IRQn,3); 
        NVIC_EnableIRQ(UART1_IRQn);
        /* 
                        * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
                         */
//        LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;
        LPC_UART1->IER = IER_RBR  | IER_RLS;				
        return (TRUE);
    }
    else if ( PortNum == 2 ) {
        LPC_SC->PCONP |= 0x01000000;

        LPC_IOCON->P0_10 &= ~0x07;
        LPC_IOCON->P0_10 |= 1;                   /* P0.10为U2_TXD  */
        LPC_IOCON->P0_11 &= ~0x07;
        LPC_IOCON->P0_11 |= 1;                   /* P0.11为U2_RXD  */
        LPC_IOCON->P1_19 &= ~0x07;
        LPC_IOCON->P1_19 |= 6;                   /* P1.19为U2_485自动控制 */
        /*
                         * 8为数据位，无奇偶检验位，1位停止位
                        */
        LPC_UART2->LCR = 0x83;        
        Fdiv = ( PeripheralClock / 16 ) / baudrate ; /*求除数值 */
        LPC_UART2->DLM = Fdiv / 256;
        LPC_UART2->DLL = Fdiv % 256;
        LPC_UART2->LCR = 0x03;                   /* 锁定除数访问 */
        LPC_UART2->FCR = 0x07;                   /* 使能并复位FIFO*/
        LPC_UART2->RS485CTRL = 0x10;
        LPC_UART2->RS485DLY = 0;
        NVIC_EnableIRQ(UART2_IRQn);
        /* 
                         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
                         */
        LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;
				
        return (TRUE);
    }
    else if ( PortNum == 3 ) {
        LPC_SC->PCONP |= 0x02000000;

        LPC_IOCON->P4_28 &= ~0x07;
        LPC_IOCON->P4_28 |= 2;                   /* P4.28为U3_TXD */
        LPC_IOCON->P4_29 &= ~0x07;
        LPC_IOCON->P4_29 |= 2;                   /* P4.29为U3_RXD*/

        /*
                        * 8为数据位，无奇偶检验位，1位停止位
                          */
        LPC_UART3->LCR = 0x83;
        Fdiv = ( PeripheralClock / 16 ) / baudrate ; /*求除数值 */
        LPC_UART3->DLM = Fdiv / 256;
        LPC_UART3->DLL = Fdiv % 256;
        LPC_UART3->LCR = 0x03;                   /* 锁定除数访问 */
        LPC_UART3->FCR = 0x07;                   /* 使能并复位FIFO*/
        
        NVIC_EnableIRQ(UART3_IRQn);
        NVIC_SetPriority(UART3_IRQn, 5);
        /* 
                         * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
                        */
        LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;				
        return (TRUE);
    }
    else if ( PortNum == 4 ) {
        LPC_SC->PCONP |= 0x00000100;

        LPC_IOCON->P1_29 &= ~0x07;
        LPC_IOCON->P1_29 |= 5;                   /* P1.29为U4_TXD */
		LPC_IOCON->P5_3  &= ~0x07;
        LPC_IOCON->P5_3  |= 4;                   /* P5.3为U4_RXD  */            
        LPC_IOCON->P0_21 &= ~0x07;
        LPC_IOCON->P0_21 |= 3;                   /* P0.21为U4_OE  */ 
        /*
                        * 8为数据位，无奇偶检验位，1位停止位
                        */
        LPC_UART4->LCR = 0x83;                          
        Fdiv = ( PeripheralClock / 16 ) / baudrate ; /*求除数值 */
        LPC_UART4->DLM = Fdiv / 256;
        LPC_UART4->DLL = Fdiv % 256;
        LPC_UART4->LCR = 0x03;                   /* 锁定除数访问 */
        LPC_UART4->FCR = 0x07;                   /* 使能并复位FIFO */
        LPC_UART4->RS485CTRL = 0x10;
        LPC_UART4->RS485DLY = 0;
        NVIC_EnableIRQ(UART4_IRQn);
        /* 
                        * 使能接收FIFO不为空中断、发送保持寄存器空中断、接收线状态中断
                         */
        LPC_UART4->IER = IER_RBR | IER_THRE | IER_RLS;
        return (TRUE);
    }
    return( FALSE );
}


/*********************************************************************************************************
** 函数名称：UARTSend
** 函数描述：串口发送数据
** 输入参数：PortNum    UART端口号
**           BufferPtr  发送数据缓冲区
**           Length     发送数据长度
** 返回值  ：无
*********************************************************************************************************/
void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
    if ( portNum == 0 ) {
        while ( Length != 0 ) {
            while ( !(UART0TxEmpty & 0x01) );   /* 等待发送标志为1  */
            LPC_UART0->THR = *BufferPtr;        /* 发送数据*/
            UART0TxEmpty = 0;                   /* 发送标志清0 */
            BufferPtr++;                        /* 指针后移 */
            Length--;                           /* 发送数目减1 */
        }
    }
    else if ( portNum == 1 ) {
        while ( Length != 0 ) {
            while ( !(UART1TxEmpty & 0x01) );   /* 等待发送标志为1 */
            LPC_UART1->THR = *BufferPtr;        /* 发送数据 */
            UART1TxEmpty = 0;                   /* 发送标志清0 */
            BufferPtr++;                        /* 指针后移 */
            Length--;                           /* 发送数目减1  */
        }
    }
    else if ( portNum == 2 ) {
        while ( Length != 0 ) {
            while ( !(UART2TxEmpty & 0x01) );   /* 等待发送标志为1 */
            LPC_UART2->THR = *BufferPtr;        /* 发送数据 */
            UART2TxEmpty = 0;                   /* 发送标志清0 */
            BufferPtr++;                        /* 指针后移*/
            Length--;                           /* 发送数目减1*/
        }
    }
    else if ( portNum == 3 ) {
        while ( Length != 0 ) {
            while ( !(UART3TxEmpty & 0x01) );   /* 等待发送标志为1 */
            LPC_UART3->THR = *BufferPtr;        /* 发送数据*/
            UART3TxEmpty = 0;                   /* 发送标志清0*/
            BufferPtr++;                        /* 指针后移 */
            Length--;                           /* 发送数目减1 */
        }
    }
    else if ( portNum == 4 ) {
        while ( Length != 0 ) {
            while ( !(UART4TxEmpty & 0x01) );  /* 等待发送标志为1  */
            LPC_UART4->THR = *BufferPtr;        /* 发送数据 */
            UART4TxEmpty = 0;                   /* 发送标志清0 */
            BufferPtr++;                        /* 指针后移*/
            Length--;                           /* 发送数目减1*/
        }
    }
    return;
}

/*********************************************************************************************************
** 函数名称：uart0SendByte
** 函数描述：向串口0发送字节数据，并等待数据发送完成，使用查询方式
** 输入参数：ucDat:   要发送的数据
** 返回值  ：无
*********************************************************************************************************/
void uart0SendByte(uint8_t ucDat)
{
    LPC_UART0->THR = ucDat;                 /* 写入数据*/
    while((LPC_UART0->LSR & 0x40) == 0);    /* 等待数据发送完毕*/
}

/*********************************************************************************************************
** 函数名称：uart0SendStr
** 函数描述：向串口0发送字符串
** 输入参数：puiStr:  要发送的字符串指针
** 返回值  ：无
*********************************************************************************************************/
void uart0SendStr(uint8_t *puiStr, uint32_t len)
{
    while(len -- > 0)                       /* 发送条件 */
    {
        uart0SendByte(*puiStr++);           /* 发送字节数据*/
    }
}
void uart1SendByte(uint8_t ucDat)
{
    LPC_UART1->THR = ucDat;               /* 写入数据*/
    while((LPC_UART1->LSR & 0x40) == 0);  /* 等待数据发送完毕*/
}

void uart1SendStr(uint8_t *puiStr, uint32_t len)
{
    while(len -- > 0)                     /* 发送条件*/
    {
        uart1SendByte(*puiStr++);         /* 发送字节数据*/
    }  
}

void uart2SendByte (uint8_t ucDat)
{
    LPC_UART2->THR = ucDat;                 /* 写入数据 */
    while ((LPC_UART2->LSR & 0x40) == 0);   /* 等待数据发送完毕 */
}
void uart2SendStr (uint8_t *puiStr, uint32_t len)
{
	while (len -- > 0) {                                                  /* 发送条件                     */
        uart2SendByte (*puiStr++);          /* 发送字节数据*/
    }
}
void uart3SendByte (uint8_t ucDat)
{
    LPC_UART3->THR = ucDat;                 /* 写入数据 */
    while ((LPC_UART3->LSR & 0x40) == 0);   /* 等待数据发送完毕 */
}
void uart3SendStr (uint8_t *puiStr, uint32_t len)
{
	while (len -- > 0) {                                                  /* 发送条件                     */
        uart3SendByte (*puiStr++);          /* 发送字节数据*/
    }
}
void uart4SendByte (uint8_t ucDat)
{
    LPC_UART4->THR = ucDat;                 /* 写入数据 */
    while ((LPC_UART4->LSR & 0x40) == 0);   /* 等待数据发送完毕 */
}

void uart4SendStr (uint8_t *puiStr, uint32_t len)
{
	while (len -- > 0) {                                                  /* 发送条件                     */
        uart4SendByte (*puiStr++);          /* 发送字节数据*/
    }
}

void UartSendByte( uint32_t portNum,uint8_t ucDat)
{
    if((portNum==CardPort)||(portNum==PrintPort)){uart0SendByte(ucDat);}
    else if(portNum==ModbusPort){uart1SendByte(ucDat);}
    else if(portNum==LCDPort){uart2SendByte(ucDat);}
    else if(portNum==PCPort){uart3SendByte(ucDat);}
    else if(portNum==RfidPort){uart4SendByte(ucDat);}
}

/*********************************************************************************************************
** 函数名称：UartSendStr
** 函数描述：串口发送数据
** 输入参数：
**           PortNum    UART端口号
**           BufferPtr  发送数据缓冲区
**           Length     发送数据长度
** 返回值  ：无
*********************************************************************************************************/
void UartSendStr( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length )
{
    if((portNum==CardPort)||(portNum==PrintPort)){uart0SendStr(BufferPtr, Length);}
    else if(portNum==ModbusPort){uart1SendStr(BufferPtr, Length);}
    else if(portNum==LCDPort){uart2SendStr(BufferPtr, Length);}
    else if(portNum==PCPort){uart3SendStr(BufferPtr, Length);}
    else if(portNum==RfidPort){uart4SendStr(BufferPtr, Length);}
}


/***************************************************************************
   **函数名称： OpenUartRcvIRQ
   **函数功能：打开串口接收中断
   **入口参数：portnum:串口号
   **返回值：    无
   **创建者：    杨朝旭
   **创建日期：2016-3-31 09:47:41

   **修改者：
   **修改日期：
   **修改内容：
   ***************************************************************************/
void OpenUartRcvIRQ(uint8_t  portnum)
{   
    if(portnum==0)
        LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;
    else if(portnum==1)
        LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;
    else if(portnum==2)
        LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;
    else if(portnum==3)
        LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;
    else if(portnum==4)
        LPC_UART4->IER = IER_RBR | IER_THRE | IER_RLS;    
}

/***************************************************************************
   **函数名称： CloseUartRcvIRQ
   **函数功能：关闭串口接收中断
   **入口参数：portnum:串口号
   **返回值：    无
   **创建者：    杨朝旭
   **创建日期：2016-3-31 09:47:41

   **修改者：
   **修改日期：
   **修改内容：
   ***************************************************************************/
void CloseUartRcvIRQ(uint8_t  portnum)
{   
    if(portnum==0)
        LPC_UART0->IER = 0;
    else if(portnum==1)
        LPC_UART1->IER = 0;
    else if(portnum==2)
        LPC_UART2->IER = 0;
    else if(portnum==3)
        LPC_UART3->IER = 0;
    else if(portnum==4)
        LPC_UART4->IER = 0;
}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/

