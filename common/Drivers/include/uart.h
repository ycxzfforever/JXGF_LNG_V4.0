#ifndef __UART_H 
#define __UART_H

/**********************************Uart0—°‘Ò**********************************************************/
#define S0                      (1ul << 27) 	 //P3.27
#define S1                      (1ul << 28) 	 //P3.28
#define USART_SEL_S0H()			LPC_GPIO3->DIR |= S0;LPC_GPIO3->SET = S0 
#define USART_SEL_S0L()   		LPC_GPIO3->DIR |= S0;LPC_GPIO3->CLR = S0
#define USART_SEL_S1H()			LPC_GPIO3->DIR |= S1;LPC_GPIO3->SET = S1 
#define USART_SEL_S1L()   		LPC_GPIO3->DIR |= S1;LPC_GPIO3->CLR = S1
#define SELECT_CARD()		  	USART_SEL_S1H(); USART_SEL_S0H()		 
#define SELECT_PRINT()			USART_SEL_S1H(); USART_SEL_S0L()
#define SELECT_POS()			USART_SEL_S1L(); USART_SEL_S0H()
#define SELECT_UART0()		 	USART_SEL_S1L(); USART_SEL_S0L()


#define MODEM_TEST    0

#define IER_RBR     0x01
#define IER_THRE    0x02
#define IER_RLS     0x04

#define IIR_PEND    0x01
#define IIR_RLS     0x03
#define IIR_RDA     0x02
#define IIR_CTI     0x06
#define IIR_THRE    0x01

#define LSR_RDR     0x01
#define LSR_OE      0x02
#define LSR_PE      0x04
#define LSR_FE      0x08
#define LSR_BI      0x10
#define LSR_THRE    0x20
#define LSR_TEMT    0x40
#define LSR_RXFE    0x80

#define BUFSIZE     200
#define BAUD_BPS    9600


/*********************uartœ‡πÿ*********************/
extern volatile uint32_t UART0Status, UART1Status;
extern volatile uint32_t UART2Status, UART3Status;
extern volatile uint32_t UART4Status;
extern volatile uint8_t UART0TxEmpty , UART1TxEmpty ;
extern volatile uint8_t UART2TxEmpty , UART3TxEmpty ;
extern volatile uint8_t UART4TxEmpty ;
extern  uint8_t UART0Buffer[BUFSIZE], UART1Buffer[50];
extern volatile uint8_t UART2Buffer[BUFSIZE], UART3Buffer[BUFSIZE];
extern volatile uint8_t UART4Buffer[BUFSIZE];
extern volatile uint32_t UART0Count , UART1Count ;
extern volatile uint32_t UART2Count , UART3Count ;
extern volatile uint32_t UART4Count ;
extern uint8_t UART0_IRQ_FLAG ;
extern uint8_t UART1_IRQ_FLAG ;
extern uint8_t UART2_IRQ_FLAG ;
extern uint8_t UART3_IRQ_FLAG ;
extern uint8_t UART4_IRQ_FLAG ;


uint32_t UARTInit( uint32_t portNum, uint32_t Baudrate );
uint32_t UARTInit_FIQ( uint32_t portNum, uint32_t Baudrate );



void UART2_IRQHandler( void );
void UART3_IRQHandler( void );
void UART4_IRQHandler( void );

void UARTSend( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length );
void UartSendByte( uint32_t portNum,uint8_t ucDat);
void UartSendStr( uint32_t portNum, uint8_t *BufferPtr, uint32_t Length );

void uart0SendStr (uint8_t *puiStr, uint32_t len);
void uart1SendStr (uint8_t *puiStr, uint32_t len);
void uart2SendStr (uint8_t *puiStr, uint32_t len);
void uart3SendStr (uint8_t *puiStr, uint32_t len);
void uart4SendStr (uint8_t *puiStr, uint32_t len);

void OpenUartRcvIRQ(uint8_t  portnum);
void CloseUartRcvIRQ(uint8_t  portnum);


#endif                                                                  /* end __UART_H                 */
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
