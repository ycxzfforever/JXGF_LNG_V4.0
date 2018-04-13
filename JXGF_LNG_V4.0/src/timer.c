#include "kernel.h"

Timer0  timer0;

/*********************************************************************************************************
** 函数名称：timer0Init
** 函数描述：定时器0初始化程序   fighting
** 输入参数：无                                  SystemCoreClock
** 返回值  ：无
*********************************************************************************************************/
void timer0Init(void)
{
	LPC_SC->PCONP |=(0x01<<1); //打开定时器0功能模块
    LPC_TIM0->TCR  = 0x02;  /*定时器控制寄存器。TCR用来控制定时器计数器的功能。定时器计数器可以通过TCR来禁能或复位。*/
    LPC_TIM0->IR   = 1;  	/*中断寄存器。可向IR写入相应的值来清除中断。也可读IR来确定中断源中哪个中断源被挂起。*/             /*中断使能*/
    LPC_TIM0->CTCR = 0;  	/*CTCR选择在定时器模式还是计数器模式下工作，在计数器模式中用于选择要计数的信号和沿。*/
    LPC_TIM0->TC   = 0;  	/*定时器计数器。32位的TC每隔（PR+1）个PCLK周期递增一次。TC通过TCR来控制。*/
    LPC_TIM0->PR   = 0;  	/*预分频寄存器。 当预分频计数器（如下）的值与这个寄存器的值相等时，下个时钟TC加这个TC，PC清零。*/
    LPC_TIM0->MR0  = 100*(PeripheralClock/1000-1);  /* 100MS中断1次                  */
    LPC_TIM0->MCR  = 0x03;                    /*匹配控制寄存器。MCR用来控制在匹配出现时是否产生中断和是否复位TC。*/       /* 匹配后产生中断               */

    NVIC_EnableIRQ(TIMER0_IRQn);                                          /* 设置中断并使能               */
    NVIC_SetPriority(TIMER0_IRQn, 3);
    LPC_TIM0->TCR  = 0x01;                                                /* 启动定时器                   */
}

uint32_t Timer0GetTick(void)
{
	return(timer0.tick_100ms);
}

void Timer0Reset(void)
{
	LPC_SC->PCONP &=~(0x01<<1);
	timer0.tick_100ms=0;
	timer0Init();
}

/*********************************************************************************************************
** 函数名称：TIMER0_IRQHandler
** 函数描述：TIMER0 中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void TIMER0_IRQHandler(void)
{  
    //LPC_TIM0->IR = 0x01;                                        /* 清除中断标志                 */
    timer0.tick_100ms++;
    timer0.flag_100ms=1;
    if(timer0.tick_100ms%5==0)
    {
        timer0.flag_500ms=1;
        if(m1card.debittimes>0)
        {
            m1card.debittimes--;
        }
    }    
    if(timer0.tick_100ms%10==0)
    {
        timer0.flag_1s=1;
        globalvar.comfailcount++;  //后台与主板通信是否正常标志。
        if(globalvar.comfailcount>200)
        {
            globalvar.comfailcount=200;
        }
        globalvar.timeout++;//计算超时时间       
        if(globalvar.precoolfinished==1)
        {
            globalvar.finishedtimeout++;
            if(globalvar.finishedtimeout > sysparas.precoolfinishedtime)//预冷完成后10s都不起泵则需重新预冷
            {
                globalvar.finishedtimeout=0;
                globalvar.precoolfinished=0;
            }
        }
    }
    
    LPC_TIM0->IR = 0x01;                                        /* 清除中断标志                 */
    if(timer0.tick_100ms>1000000)  timer0.tick_100ms=0;    
}


void timer1on(void)
{
   LPC_TIM1->TCR  = 0x01; 
}

void timer1off(void)
{
   LPC_TIM1->TCR  = 0x00; 
}

/*********************************************************************************************************
** 函数名称：timer1Init
** 函数描述：定时器1初始化程序
** 输入参数：无                                  
** 返回值  ：无
*********************************************************************************************************/
void timer1Init(void)
{
	LPC_SC->PCONP |=(0x01<<2); //打开定时器1功能模块
    LPC_TIM1->TCR  = 0x02;  /*定时器控制寄存器。TCR用来控制定时器计数器的功能。定时器计数器可以通过TCR来禁能或复位。*/
    LPC_TIM1->IR   = 1;  	/*中断寄存器。可向IR写入相应的值来清除中断。也可读IR来确定中断源中哪个中断源被挂起。*/             /*中断使能*/
    LPC_TIM1->CTCR = 0;  	/*CTCR选择在定时器模式还是计数器模式下工作，在计数器模式中用于选择要计数的信号和沿。*/
    LPC_TIM1->TC   = 0;  	/*定时器计数器。32位的TC每隔（PR+1）个PCLK周期递增一次。TC通过TCR来控制。*/
    LPC_TIM1->PR   = 0;  	/*预分频寄存器。 当预分频计数器（如下）的值与这个寄存器的值相等时，下个时钟TC加这个TC，PC清零。*/
    LPC_TIM1->MR0  = 100*(PeripheralClock/1000-1);  /* 100MS中断1次                  */
    LPC_TIM1->MCR  = 0x03;                    /*匹配控制寄存器。MCR用来控制在匹配出现时是否产生中断和是否复位TC。*/       /* 匹配后产生中断               */
    NVIC_EnableIRQ(TIMER1_IRQn);                                          /* 设置中断并使能               */
    NVIC_SetPriority(TIMER1_IRQn, 4);
    LPC_TIM1->TCR  = 0x0;                                                /* 启动定时器                   */
}

//读流量计和压力传感器 ycx20170329
void ReadModbusAndPressure(void)
{
    if(sysparas.simulation==0)
    {    
        //读取液相流量计数据，每次必须读
        ModbusReadScanBuffer(_MODBUS_L_);
        globalvar.modbus_l_error++;
        if(globalvar.modbus_l_error>5)
        {
            SetBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);//液相流量计通讯错误。
            globalvar.stoptimes=1000;//液象流量计异常直接停机
            stopreason=2;
        }   
        
        //读取气相流量计数据，回气计量时才读
        if((sysparas.backgascalc==1)||(GetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR)==1))//回气计量或者气象异常时读
        {
            MsDelay(10);//必须的延时，否则不能处理气象流量计数据
            ModbusReadScanBuffer(_MODBUS_G_);  
            globalvar.modbus_g_error++;
            if(globalvar.modbus_g_error>5)
            {
                SetBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);//气相流量计通讯错误。
                globalvar.stoptimes=1000;//气象流量计异常直接停机
                stopreason=1;
            }          
        }
        
        //读取压力变送器
        if(!PressureGetValue())
		{
		    globalvar.pressure_error++;
            if(globalvar.pressure_error>10)
            {
    			SetBit(syserr,SYSERR_PRESSURE);
                globalvar.stoptimes=1000;//压力传感器异常直接停机
                stopreason=4;
            }
		}
		else
		{
			globalvar.pressure_error=0;
            ClrBit(syserr,SYSERR_PRESSURE);
		}        
    }
}

/*********************************************************************************************************
** 函数名称：TIMER1_IRQHandler
** 函数描述：TIMER1 中断处理函数,用于读取流量计数据
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void TIMER1_IRQHandler(void)
{      
    //ReadModbusAndPressure();
    LPC_TIM1->IR = 0x01; 
}



