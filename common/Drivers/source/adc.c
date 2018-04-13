#include "kernel.h"
/********************************************************
**函数名称:PlADCUinit
**函数功能:液相压力采集初始化
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-14
**修改者  :
**修改日期:
**修改内容:
********************************************************/
void PlADCUinit(void)
{
  LPC_SC->PCONP |= (0x1<<12);       /* 使能ADC功率控制器                     */
  LPC_IOCON->P0_23 &= ~0x9F;  
  LPC_IOCON->P0_23 |= 0x01;         /* ADC IN0  选择引脚为AD0模式            */
	
  LPC_ADC->CR = (0x01<<0) |         /* 选则ADC[0]为ADC输入                   */
								(149<<8) |          /* CLKDIV = Fpclk / ADC_Clk - 1  60000000/400000-1=149 设置为400k HZ转换速率*/ 
								( 0 << 16 ) |       /* BURST = 0, 软件控制转换               */
								( 1 << 21 ) |       /* PDN = 1,  处与正常模式                */
								( 0 << 24 ) |       /* START = 0  不启动AD转换               */
								( 0 << 27 );        /* EDGE = 0  上升沿启动                  */ 
  return;
}
/********************************************************
**函数名称:PgADCUinit
**函数功能:液相压力采集初始化
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-14
**修改者  :
**修改日期:
**修改内容:
********************************************************/
void PgADCUinit(void)
{
  LPC_SC->PCONP |= (0x1<<12);       /* 使能ADC功率控制器                     */
  LPC_IOCON->P0_24 &= ~0x9F;  
  LPC_IOCON->P0_24 |= 0x01;         /* ADC IN0  选择引脚为AD0模式            */
	
  LPC_ADC->CR = (0x01<<1) |         /* 选则ADC[0]为ADC输入                   */
								(149<<8) |          /* CLKDIV = Fpclk / ADC_Clk - 1  60000000/400000-1=149 设置为400k HZ转换速率*/ 
								( 0 << 16 ) |       /* BURST = 0, 软件控制转换               */
								( 1 << 21 ) |       /* PDN = 1,  处与正常模式                */
								( 0 << 24 ) |       /* START = 0  不启动AD转换               */
								( 0 << 27 );        /* EDGE = 0  上升沿启动                  */ 
  return;
}

/********************************************************
**函数名称:PlADCGetValueOneTime
**函数功能:液相压力采集一次
**输入参数:无
**输出值  :液相压力AD转换结果

**创建者  :黄豪民
**创建日期:2016-04-14
**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint32_t PlADCGetValueOneTime(void)
{
	uint32_t adcvalue;
	adcvalue=0;
	LPC_ADC->CR |=(0x01<<0)|(1UL<<24);      /* START = 1  启动AD转换*/
	while ((LPC_ADC->STAT&(1<<0))==0);//等待结束
	adcvalue=LPC_ADC->DR[0]; /*读取通道数据寄存器的值*/
	adcvalue=(adcvalue>>4)&0xfff;
	return(adcvalue);
}
/********************************************************
**函数名称:PgADCGetValueOneTime
**函数功能:气相压力采集一次
**输入参数:无
**输出值  :气相压力AD转换结果

**创建者  :黄豪民
**创建日期:2016-04-14
**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint32_t PgADCGetValueOneTime(void)
{
	uint32_t adcvalue;
	adcvalue=0;
	LPC_ADC->CR |=(0x01<<1)|(1UL<<24);      /* START = 1  启动AD转换*/
	while ((LPC_ADC->STAT&(1<<1))==0);//等待结束
	adcvalue=LPC_ADC->DR[1]; /*读取通道数据寄存器的值*/
	adcvalue=(adcvalue>>4)&0xfff;
	return(adcvalue);
}

/********************************************************
**函数名称:PlADCGetValue
**函数功能:液相，20次AD转换均值
**输入参数:无
**输出值  :液相20次AD转换均值

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint32_t PlADCGetValue(void)
{
	uint32_t adcvalue;
	uint8_t count;
	PlADCUinit();
	adcvalue=0;
	for(count=0;count<20;count++)
	{
		adcvalue+=PlADCGetValueOneTime();
	}
	adcvalue=(uint32_t)(adcvalue/20);
	LPC_ADC->CR &= 0xF8FFFFFF;         /* 停止转换 */
	return adcvalue;
}

/********************************************************
**函数名称:PgADCGetValue
**函数功能:气相，20次AD转换均值
**输入参数:无
**输出值  :气相20次AD转换均值

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint32_t PgADCGetValue(void)
{
	uint32_t adcvalue;
	uint8_t count;
	PgADCUinit();
	adcvalue=0;
	for(count=0;count<20;count++)
	{
		adcvalue+=PgADCGetValueOneTime();
	}
	adcvalue=(uint32_t)(adcvalue/20);
	LPC_ADC->CR &= 0xF8FFFFFF;         /* 停止转换 */
	return adcvalue;
}

/********************************************************
**函数名称:PressureGetValue
**函数功能:获取压力值，并赋值给realtime.cur_pressure
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-12-12

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t PressureGetValue(void)	
{
	
	uint32_t curAD=0;
	uint32_t deltaAD=0;
	uint32_t curPressure;
	curAD=PlADCGetValue(); //获取当前压力AD值

	if((curAD+80)<=sysparas.pzero) //压力零点
	{
		return false;
	}    
	else if(curAD<=sysparas.pzero)
	{
		deltaAD=0;
	}
	else
	{
		deltaAD=curAD-sysparas.pzero;  
	}
  
	curPressure = (100*deltaAD)/sysparas.pcoeff; //计算压力值
	realtime.cur_pressure = curPressure;
	return true;  
}

