#ifndef __FYD12864_H
#define __FYD12864_H

#define Fyd12864_CS_Low     (LPC_GPIO2->CLR |=1ul<<20)     //CS输出低电平
#define Fyd12864_CS_High    (LPC_GPIO2->SET |=1ul<<20)     //CS输出高电平
#define Fyd12864_CS_Out     (LPC_GPIO2->DIR |=1ul<<20)     //设置CS输出
#define Fyd12864_CS_In      (LPC_GPIO2->DIR &=~(1ul<<20) //设置CS输入

#define Fyd12864_SDI_Low    (LPC_GPIO1->CLR |=1ul<<22)     //SDI输出低电平
#define Fyd12864_SDI_High   (LPC_GPIO1->SET |=1ul<<22)     
#define Fyd12864_SDI_Out    (LPC_GPIO1->DIR |=1ul<<22)     //设置SDI输出
#define Fyd12864_SDI_In     (LPC_GPIO1->DIR &=~(1ul<<22))

#define Fyd12864_CLK_Low    (LPC_GPIO4->CLR |=1ul<<0)      //CLK输出低电平
#define Fyd12864_CLK_High   (LPC_GPIO4->SET |=1ul<<0)      
#define Fyd12864_CLK_Out    (LPC_GPIO4->DIR |=1ul<<0)      //设置CLK输出

void Fyd12864Init(void);
void Fyd12864DispPrintf(uint8_t row, uint8_t col,char *fmt,...);
void Fyd12864DisChar(uint8_t row,uint8_t col,char ch);
void Fyd12864ClearScreen(void);
void Fyd12864ClearLine(uint8_t row);
void Fyd12864ShowGDRAM(uint8_t* address);
void Fyd12864CurrsorBlink(uint8_t row,uint8_t col,uint8_t flag);


#endif
