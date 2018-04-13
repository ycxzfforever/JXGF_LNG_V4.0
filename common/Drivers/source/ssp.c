#include "kernel.h"


/* 
 * 中断状态标志
 */
volatile uint32_t interrupt0RxStat = 0;
volatile uint32_t interrupt0OverRunStat = 0;
volatile uint32_t interrupt0RxTimeoutStat = 0;
volatile uint32_t interrupt1RxStat = 0;
volatile uint32_t interrupt1OverRunStat = 0;
volatile uint32_t interrupt1RxTimeoutStat = 0;
volatile uint32_t interrupt2RxStat = 0;
volatile uint32_t interrupt2OverRunStat = 0;
volatile uint32_t interrupt2RxTimeoutStat = 0;


/*********************************************************************************************************
** 函数名称：SSP0_IRQHandler
** 函数描述：SSP0中断函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void SSP0_IRQHandler(void)
{
    uint32_t regValue;

    regValue = LPC_SSP0->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt0OverRunStat++;
        LPC_SSP0->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt0RxTimeoutStat++;
        LPC_SSP0->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt0RxStat++;
    }
    return;
}


/*********************************************************************************************************
** 函数名称：SSP1_IRQHandler
** 函数描述：SSP1中断函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void SSP1_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP1->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt1OverRunStat++;
        LPC_SSP1->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt1RxTimeoutStat++;
        LPC_SSP1->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt1RxStat++;
    }
    return;
}


/*********************************************************************************************************
** 函数名称：SSP2_IRQHandler
** 函数描述：SSP2中断函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void SSP2_IRQHandler(void) 
{
    uint32_t regValue;

    regValue = LPC_SSP2->MIS;
    if (regValue & SSPMIS_RORMIS) {                                     /* 溢出中断                     */
        interrupt2OverRunStat++;
        LPC_SSP2->ICR = SSPICR_RORIC;
    }
    if (regValue & SSPMIS_RTMIS) {                                      /* 接收超时中断                 */
        interrupt2RxTimeoutStat++;
        LPC_SSP2->ICR = SSPICR_RTIC;
    }
    if (regValue & SSPMIS_RXMIS) {                                      /* RX FIFO至少有一半为满        */
        interrupt2RxStat++;
    }
    return;
}


/*********************************************************************************************************
** 函数名称：SSP_SSELSetDirection
** 函数描述：选择SSP的CS管脚，实现对其电平可控
** 输入参数：uiPortNum      SSP端口号
**           uiLocation   CS管脚位置
** 返回值  ：无
*********************************************************************************************************/
void SSP_SSELSetDirection(uint32_t uiPortNum, uint32_t uiLocation)
{
    if (uiPortNum == 0) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P2_23 &= ~0x07;                                  /* P2.23为SSP0的CS管脚          */
            LPC_GPIO2->DIR |= (0x1<<23);
            LPC_GPIO2->SET |= (0x1<<23);
            break;
        case 1:
            LPC_IOCON->P1_21 &= ~0x07;                                  /* P1.21为SSP0的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<21);
            LPC_GPIO1->SET |= (0x1<<21);
            break;
        case 2:
            LPC_IOCON->P1_28 &= ~0x07;                                  /* P1.28为SSP0的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<28);
            LPC_GPIO1->SET |= (0x1<<28);
            break;
        case 3:
            LPC_IOCON->P0_16 &= ~0x07;                                  /* P0.16为SSP0的CS管脚          */
            LPC_GPIO0->DIR |= (0x1<<16);
            LPC_GPIO0->SET |= (0x1<<16);
            break;
        }
    }
    else if (uiPortNum == 1) {
        switch (uiLocation) {
        case 0:
            LPC_IOCON->P0_14 &= ~0x07;                                  /* P0.14为SSP1的CS管脚          */
            LPC_GPIO0->DIR |= (0x1<<14);
            LPC_GPIO0->SET |= (0x1<<14);
            break;
        case 1:
            LPC_IOCON->P1_26 &= ~0x07;                                  /* P1.26为SSP1的CS管脚          */
            LPC_GPIO1->DIR |= (0x1<<26);
            LPC_GPIO1->SET |= (0x1<<26);
            break;
        case 2:
            LPC_IOCON->P4_21 &= ~0x07;                                  /* P4.21为SSP1的CS管脚          */
            LPC_GPIO4->DIR |= (0x1<<21);
            LPC_GPIO4->SET |= (0x1<<21);
            break;
        case 3:
            LPC_IOCON->P0_6 &= ~0x07;                                  /* P0.6为SSP1的CS管脚           */
            LPC_GPIO0->DIR |= (0x1<<6);
            LPC_GPIO0->SET |= (0x1<<6);
            break;
       }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            LPC_IOCON->P5_3 &= ~0x07;                                  /* P5.3为SSP2的CS管脚           */
            LPC_GPIO5->DIR |= (0x1<<3);
            LPC_GPIO5->SET |= (0x1<<3);
            break;
        case 1:
            LPC_IOCON->P1_8 &= ~0x07;                                  /* P1.8为SSP2的CS管脚           */
            LPC_GPIO1->DIR |= (0x1<<8);
            LPC_GPIO1->SET |= (0x1<<8);
            break;
        default:
            break;
        }
    }
    return;
}


/*********************************************************************************************************
** 函数名称：SSP_SSELToggle
** 函数描述：控制SSP的CS管脚电平
** 输入参数：uiPortNum    SSP端口号
**           uiLocation   SSEL管脚位置
**           toggle       SSEL管脚电平控制，0为低电平，1为高电平
** 返回值  ：无
*********************************************************************************************************/
void SSP_SSELToggle( uint32_t uiPortNum, uint32_t uiLocation, uint32_t toggle )
{
    if ( uiPortNum == 0 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO2->CLR |= (0x1<<23);                            /* P2.23为CS，且为低电平        */
            else
                LPC_GPIO2->SET |= (0x1<<23);                            /* P2.23为CS，且为高电平        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<21);                            /* P1.21为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<21);                            /* P1.21为CS，且为高电平        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<28);                            /* P1.28为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<28);                            /* P1.28为CS，且为高电平        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<16);                            /* P0.16为CS，且为低电平        */
            else
                LPC_GPIO0->SET |= (0x1<<16);                            /* P0.16为CS，且为高电平        */
            break;
        }
    }
    else if ( uiPortNum == 1 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<14);                            /* P0.14为CS，且为低电平        */
            else
                LPC_GPIO0->SET |= (0x1<<14);                            /* P0.17为CS，且为高电平        */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<26);                            /* P1.26为CS，且为低电平        */
            else
                LPC_GPIO1->SET |= (0x1<<26);                            /* P1.26为CS，且为高电平        */
            break;
        case 2:
            if ( !toggle )
                LPC_GPIO4->CLR |= (0x1<<21);                            /* P4.21为CS，且为低电平        */
            else
                LPC_GPIO4->SET |= (0x1<<21);                            /* P4.21为CS，且为高电平        */
            break;
        case 3:
            if ( !toggle )
                LPC_GPIO0->CLR |= (0x1<<6);                            /* P0.6为CS，且为低电平         */
            else
                LPC_GPIO0->SET |= (0x1<<6);                            /* P0.6为CS，且为高电平         */
            break;
        }
    }
    else if ( uiPortNum == 2 ) {
        switch ( uiLocation ) {
        case 0:
            if ( !toggle )
                LPC_GPIO5->CLR |= (0x1<<3);                            /* P5.3为CS，且为低电平         */
            else
                LPC_GPIO5->SET |= (0x1<<3);                            /* P5.3为CS，且为高电平         */
            break;
        case 1:
            if ( !toggle )
                LPC_GPIO1->CLR |= (0x1<<8);                            /* P1.8为CS，且为低电平         */
            else
                LPC_GPIO1->SET |= (0x1<<8);                            /* P1.8为CS，且为高电平         */
            break;
        default:
            break;
        }
    }
    return;
}


/***************************************************************************
**函数名称：Fm25L04AndFlashSpiInit
**函数功能：初始化Fm25L04和Flash的共用Spi0端口。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void Fm25L04AndFlashSpiInit(void)
{
    uint8_t i = 0; 
    uint8_t uiDummy = uiDummy = 0;

    LPC_SC->PCONP |= (0x1 << 21);                                       /* 开启SSP0外设                 */


    LPC_IOCON->P0_15 &= ~0x07;
    LPC_IOCON->P0_15 |=  0x02;                                           /* SSP CLK                      */
    LPC_IOCON->P0_16 &= ~0x07;    
    LPC_IOCON->P0_16 |=  0x02;                                           /* SSP SSEL                     */
    LPC_IOCON->P0_17 &= ~0x07;
    LPC_IOCON->P0_17 |=  0x02;                                           /* SSP MISO                     */
    LPC_IOCON->P0_18 &= ~0x07;    
    LPC_IOCON->P0_18 |=  0x02;                                           /* SSP MOSI                     */


    /* 
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    #if USE_CS
        SSP_SSELSetDirection( 0,3 );
    #endif

    LPC_SSP0->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x01 << 6 |                                         /* CPOL为1                      */
                    0x01 << 7 |                                         /* CPHA为1  ZLG9518S工作在模式3 */
                    0x07 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP0->CPSR = 0x02;                                              /* 时钟预分频值为2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP0->DR;                                         /* 清0接收FIFO                  */
    }

	NVIC_EnableIRQ(SSP0_IRQn);                                          /* 使能SSP0中断                 */
	  
                                                                        /* 主机模式                     */
    LPC_SSP0->CR1 = (0x00 << 3) |                                       /*	从机输出禁能。该位只与从机模式有关（MS=1）。如果该位为1，禁止SSP控制器驱动发送数据线（MISO）*/                                        
                    (0x00 << 2) |                                       /*	SSP控制器用作一个总线主机，驱动SCLK、MOSI和SSEL线并接收MISO线	*/                                        
                    (0x01 << 1) |                                       /*	SSP控制器使能	*/                                        
                    (0x00 << 0);                                        /*	正常操作模式	*/                                       
    LPC_SSP0->ICR  = 0x03;        	                                    /* SSPICR中断清零寄存器  */                                           
   

    LPC_SSP0->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}


/*********************************************************************************************************
** 函数名称：SSP1Init
** 函数描述：SSP1初始化
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void SSP1Init( uint32_t uiLocation )
{
    uint8_t i = 0, uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0; 
    
    LPC_SC->PCONP |= (0x1<<10);                                         /* 开启SSP1外设                 */
    
    /* 
     * 设定外设分频值
     */
    uiRegVal = LPC_SC->PCLKSEL;
    if ( uiRegVal < 4 ) {
        LPC_SC->PCLKSEL = 4;
    }

    /* 
     * 选定特定位置功能管脚
     */
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P0_7  &= ~0x07;
        LPC_IOCON->P0_7  |=  0x02;                                      /* SSP CLK                      */
        LPC_IOCON->P0_14 &= ~0x07;    
        LPC_IOCON->P0_14 |=  0x02;                                      /* SSP SSEL                     */
        LPC_IOCON->P0_12 &= ~0x07;
        LPC_IOCON->P0_12 |=  0x02;                                      /* SSP MISO                     */
        LPC_IOCON->P0_13 &= ~0x07;    
        LPC_IOCON->P0_13 |=  0x02;                                      /* SSP MOSI                     */
        break;

    case 1:
        LPC_IOCON->P1_19 &= ~0x07;
        LPC_IOCON->P1_19 |=  0x05;                                      /* SSP CLK                      */
        LPC_IOCON->P1_26 &= ~0x07;    
        LPC_IOCON->P1_26 |=  0x05;                                      /* SSP SSEL                     */
        LPC_IOCON->P1_18 &= ~0x07;
        LPC_IOCON->P1_18 |=  0x05;                                      /* SSP MISO                     */
        LPC_IOCON->P1_22 &= ~0x07;    
        LPC_IOCON->P1_22 |=  0x05;                                      /* SSP MOSI                     */
        break;

    case 2:
        LPC_IOCON->P4_20 &= ~0x07;
        LPC_IOCON->P4_20 |=  0x03;                                      /* SSP CLK                      */
        LPC_IOCON->P4_21 &= ~0x07;    
        LPC_IOCON->P4_21 |=  0x03;                                      /* SSP SSEL                     */
        LPC_IOCON->P4_22 &= ~0x07;
        LPC_IOCON->P4_22 |=  0x03;                                      /* SSP MISO                     */
        LPC_IOCON->P4_23 &= ~0x07;    
        LPC_IOCON->P4_23 |=  0x03;                                      /* SSP MOSI                     */
        break;

    case 3:
        LPC_IOCON->P0_7 &= ~0x07;
        LPC_IOCON->P0_7 |=  0x02;                                       /* SSP CLK                      */
        LPC_IOCON->P0_6 &= ~0x07;    
        LPC_IOCON->P0_6 |=  0x02;                                       /* SSP SSEL                     */
        LPC_IOCON->P0_8 &= ~0x07;
        LPC_IOCON->P0_8 |=  0x02;                                       /* SSP MISO                     */
        LPC_IOCON->P0_9 &= ~0x07;    
        LPC_IOCON->P0_9 |=  0x02;                                       /* SSP MOSI                     */
        break;
    default :
        break;
    }
    
    /* 
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    #if USE_CS
        SSP_SSELSetDirection( 1, uiLocation );
    #endif

    LPC_SSP1->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x00 << 6 |                                         /* CPOL为0                      */
                    0x00 << 7 |                                         /* CPHA为0                      */
                    0x07 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP1->CPSR = 0x2;                                               /* 时钟预分频值为2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP1->DR;                                         /* 清0接收FIFO                  */
    }
    

    NVIC_EnableIRQ(SSP1_IRQn);                                          /* 使能SSP1中断                 */
//    NVIC_DisableIRQ(SSP1_IRQn);                                          /* 使能SSP0中断                 */

    #if LOOPBACK_MODE                                                   /* 回路模式                     */
        LPC_SSP1->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE1                                                     /* 从机模式                     */
        if ( LPC_SSP1->CR1 & SSPCR1_SSE ) {
            LPC_SSP1->CR1 &= ~SSPCR1_SSE;                               /* 禁能SSP1控制器               */
        }
        LPC_SSP1->CR1 = SSPCR1_MS;                                      /* 选择SSP1总线从机             */
        LPC_SSP1->CR1 |= SSPCR1_SSE;                                    /* 使能SSP1控制器               */
    #else                                                               /* 主机模式                     */
        LPC_SSP1->CR1 = SSPCR1_SSE;

//		LPC_SSP1->CR1 = (0x00 << 3) |    /*	从机输出禁能。该位只与从机模式有关（MS=1）。如果该位为1，禁止SSP控制器驱动发送数据线（MISO）*/                                        
//						(0x00 << 2) |    /*	SSP控制器用作一个总线主机，驱动SCLK、MOSI和SSEL线并接收MISO线	*/                                        
//						(0x01 << 1) |    /*	SSP控制器使能	*/                                        
//						(0x00 << 0);     /*	正常操作模式	*/                                       
////    	LPC_SSP1->ICR  = 0x03;        	 /* SSPICR中断清零寄存器  */                                           

    #endif

    LPC_SSP1->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}


/*********************************************************************************************************
** 函数名称：SSP2Init
** 函数描述：SSP2初始化
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void SSP2Init( uint32_t uiLocation )
{
    uint8_t i = 0, uiRegVal = 0;
    uint8_t uiDummy = uiDummy = 0;
    
    LPC_SC->PCONP |= (0x1<<20);                                         /* 开启SSP2外设                 */
    
    /* 
     * 设定外设分频值
     */
    uiRegVal = LPC_SC->PCLKSEL;
    if (uiRegVal < 4) {
        LPC_SC->PCLKSEL = 4;
    }
    
    /* 
     * 选定特定位置功能管脚
     */
    switch (uiLocation) {
    case 0:
        LPC_IOCON->P5_2 &= ~0x07;
        LPC_IOCON->P5_2 |=  0x02;                                       /* SSP CLK                      */
        LPC_IOCON->P5_3 &= ~0x07;    
        LPC_IOCON->P5_3 |=  0x02;                                       /* SSP SSEL                     */
        LPC_IOCON->P5_1 &= ~0x07;
        LPC_IOCON->P5_1 |=  0x02;                                       /* SSP MISO                     */
        LPC_IOCON->P5_0 &= ~0x07;    
        LPC_IOCON->P5_0 |=  0x02;                                       /* SSP MOSI                     */
        break;

    case 1:
        LPC_IOCON->P1_0 &= ~0x07;
        LPC_IOCON->P1_0 |=  0x04;                                       /* SSP CLK                      */
        LPC_IOCON->P1_8 &= ~0x07;    
        LPC_IOCON->P1_8 |=  0x04;                                       /* SSP SSEL                     */
        LPC_IOCON->P1_4 &= ~0x07;
        LPC_IOCON->P1_4 |=  0x04;                                       /* SSP MISO                     */
        LPC_IOCON->P1_1 &= ~0x07;    
        LPC_IOCON->P1_1 |=  0x04;                                       /* SSP MOSI                     */
        break;
        
    default:
        break;
    }

    /* 
     * 不使用管脚的CS功能时，人工控制实现使能控制
     */
    #if !USE_CS
        SSP_SSELSetDirection( 2, uiLocation );
    #endif

    LPC_SSP2->CR0 = 0x07 << 0 |                                         /* 数据长度为8位                */
                    0x00 << 4 |                                         /* 帧格式为SPI                  */
                    0x00 << 6 |                                         /* CPOL为0                      */
                    0x00 << 7 |                                         /* CPHA为0                      */
                    0x07 << 8;                                          /* 串行时钟速率为7              */

    LPC_SSP2->CPSR = 0x2;                                               /* 时钟预分频值为2              */
    
    for (i = 0; i < FIFOSIZE; i++) {
        uiDummy = LPC_SSP2->DR;                                         /* 清0接收FIFO                  */
    }

    NVIC_EnableIRQ(SSP2_IRQn);                                          /* 使能SSP2中断                 */

    #if LOOPBACK_MODE                                                   /* 回路模式                     */
        LPC_SSP2->CR1 = SSPCR1_LBM | SSPCR1_SSE;
    #elif SSP_SLAVE2                                                    /* 从机模式                     */
        if ( LPC_SSP2->CR1 & SSPCR1_SSE ) {
            LPC_SSP2->CR1 &= ~SSPCR1_SSE;                               /* 禁能SSP1控制器               */
        }
        LPC_SSP2->CR1 = SSPCR1_MS;                                      /* 选择SSP2总线从机             */
        LPC_SSP2->CR1 |= SSPCR1_SSE;                                    /* 使能SSP2控制器               */
    #else                                                               /* 主机模式                     */
        LPC_SSP2->CR1 = SSPCR1_SSE;
    #endif
    LPC_SSP2->IMSC = SSPIMSC_RORIM | SSPIMSC_RTIM;                      /* 使能溢出中断和接收超时中断   */
    return;
}


/***************************************************************************
**函数名称：Fm25L04AndFlashSpiSend
**函数功能：Fm25l04和Flash的Spi数据发送函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void Fm25L04AndFlashSpiSend(uint8_t *puiBuf, uint16_t uiLength)
{
    uint32_t i = 0;
    uint8_t uiDummy = uiDummy = 0;

    for (i = 0; i < uiLength; i++) 
    {
        while ( (LPC_SSP0->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                (SSPSR_TNF | SSPSR_BSY))!=
                 SSPSR_TNF );

        LPC_SSP0->DR = *puiBuf;                                     /* 发送数据                     */
        puiBuf++;                                                   /* 指向下一个发送数据           */

  
        while ( (LPC_SSP0->SR &                                     /* SSP0的RX FIFO未空且SSP0空闲  */
                (SSPSR_BSY | SSPSR_RNE)) !=
                 SSPSR_RNE );

        uiDummy = LPC_SSP0->DR;                                     /* 清空TX FIFO                  */
    }
    return;
}


/***************************************************************************
**函数名称：Fm25L04AndFlashSpiReceive
**函数功能：Fm25l04和Flash的Spi数据接受函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void Fm25L04AndFlashSpiReceive(uint8_t *puiBuf, uint16_t uiLength )
{
    uint16_t i;
    for (i = 0; i < uiLength; i++)  // 主机模式。
    {                                               
        LPC_SSP0->DR = 0xFF;
        while ( (LPC_SSP0->SR & 
                (SSPSR_BSY | SSPSR_RNE)) !=
                 SSPSR_RNE );
   
        *puiBuf++ = LPC_SSP0->DR; 
    }
    return;
}
/*********************************************************************************************************
** 函数名称: SPSend
** 函数描述：SSP发送数据函数。
** 输入参数：uiPortNum    SSP端口号
**           puiBuf       发送数据指针
**           uiLength     发送数据长度
** 返回值  ：无
*********************************************************************************************************/
void SSPSend( uint32_t uiPortNum, uint8_t *puiBuf, uint32_t uiLength )
{
    uint32_t i = 0;
    uint8_t uiDummy = uiDummy = 0;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            while ( (LPC_SSP0->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF | SSPSR_BSY))!=
                     SSPSR_TNF );

            LPC_SSP0->DR = *puiBuf;                                     /* 发送数据                     */
            puiBuf++;                                                   /* 指向下一个发送数据           */

            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP0->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP0->DR;                                 /* 清空TX FIFO                  */
            #else                                                       /* 回路模式                     */
                while ( LPC_SSP0->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 1 ) {
            while ( (LPC_SSP1->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF | SSPSR_BSY)) != 
                	SSPSR_TNF );

            LPC_SSP1->DR = *puiBuf;                                     /* 发送数据                     */
            puiBuf++;                                                   /* 指向下一个发送数据           */

            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP1->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );

                uiDummy = LPC_SSP1->DR;                                 /* 清空TX FIFO                  */
            #else                                                       /* 回路模式                     */
                while ( LPC_SSP1->SR & SSPSR_BSY );
            #endif
        }
        else if ( uiPortNum == 2 ) {
            while ( (LPC_SSP2->SR &                                     /* SSP0的TX FIFO未满且SSP0空闲  */
                    (SSPSR_TNF|SSPSR_BSY)) !=
                     SSPSR_TNF );
            LPC_SSP2->DR = *puiBuf;                                       /* 发送数据                     */
            puiBuf++;                                                     /* 指向下一个发送数据           */
            #if !LOOPBACK_MODE                                          /* 主机或从机模式               */
                while ( (LPC_SSP2->SR &                                 /* SSP0的RX FIFO未空且SSP0空闲  */
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );

                uiDummy = LPC_SSP2->DR;                                 /* 清空TX FIFO                  */
            #else
                while ( LPC_SSP2->SR & SSPSR_BSY );
            #endif
        }
    }
    return;
}

/*********************************************************************************************************
** 函数名称：SSPRec
** 函数描述：SSP接受数据函数。
** 输入参数：uiPortNum    SSP端口号
**           puiBuf       接收数据指针
**           uiLength     接收数据长度
** 返回值  ：无
*********************************************************************************************************/
void SSPRec( uint32_t uiPortNum, uint8_t *puiBuf, uint32_t uiLength )
{
    uint32_t i;

    for (i = 0; i < uiLength; i++) {
        if ( uiPortNum == 0 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE0                                              /* 从机模式                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #else                                                       /* 主机模式                     */
                LPC_SSP0->DR = 0xFF;
                while ( (LPC_SSP0->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
                while ( !(LPC_SSP0->SR & SSPSR_RNE) );
            #endif
            *puiBuf++ = LPC_SSP0->DR;
        }
        else if ( uiPortNum == 1 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE1                                              /* 从机模式                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #else                                                       /* 主机模式                     */
                LPC_SSP1->DR = 0xCC;
                while ( (LPC_SSP1->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) !=
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
                while ( !(LPC_SSP1->SR & SSPSR_RNE) );
            #endif
            *puiBuf++ = LPC_SSP1->DR;
        }
        else if ( uiPortNum == 2 ) {
            #if !LOOPBACK_MODE
            #if SSP_SLAVE2                                              /* 从机模式                     */
                while (!(LPC_SSP2->SR & SSPSR_RNE));
            #else                                                       /* 主机模式                     */
                LPC_SSP2->DR = 0xFF;
                while ( (LPC_SSP2->SR & 
                        (SSPSR_BSY | SSPSR_RNE)) != 
                         SSPSR_RNE );
            #endif
            #else                                                       /* 回路模式                     */
                while (!(LPC_SSP2->SR & SSPSR_RNE));
            #endif
            *puiBuf++ = LPC_SSP2->DR;
        }
    }
    return;
}

/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/

