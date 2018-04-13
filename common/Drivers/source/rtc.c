#include "kernel.h"

/********************************************************
**函数名称:RTCSetTime
**函数功能:设置时间
**输入参数:Time---RTCTime结构体(见rtc.h,包括年月日、时分秒等)
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void RTCSetTime(RTCTime Time) 
{
  LPC_RTC->SEC   = Time.RTC_Sec;
  LPC_RTC->MIN   = Time.RTC_Min;
  LPC_RTC->HOUR  = Time.RTC_Hour;
  LPC_RTC->DOM   = Time.RTC_Mday;
  LPC_RTC->DOW   = Time.RTC_Wday;
  LPC_RTC->DOY   = Time.RTC_Yday;
  LPC_RTC->MONTH = Time.RTC_Mon;
  LPC_RTC->YEAR  = Time.RTC_Year;     
}


/********************************************************
**函数名称:RTCGetTime
**函数功能:获取时间
**输入参数:无
**输出值  :返回当前时间，用RTCTime结构体表示

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
RTCTime RTCGetTime(void) 
{    
  RTCTime LocalTime;
  LocalTime.RTC_Sec  = LPC_RTC->SEC;
  LocalTime.RTC_Min  = LPC_RTC->MIN;
  LocalTime.RTC_Hour = LPC_RTC->HOUR;
  LocalTime.RTC_Mday = LPC_RTC->DOM;
  LocalTime.RTC_Wday = LPC_RTC->DOW;
  LocalTime.RTC_Yday = LPC_RTC->DOY;
  LocalTime.RTC_Mon  = LPC_RTC->MONTH;
  LocalTime.RTC_Year = LPC_RTC->YEAR;
  return (LocalTime);   
}

/********************************************************
**函数名称:RTCStart
**函数功能:内部RTC开始工作
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void RTCStart(void) 
{
                                               /* Start RTC counters        */
  LPC_RTC->ILR = (ILR_RTCCIF | ILR_RTCALF);    /* 清除增量、报警中断 标志   */
  LPC_RTC->CCR |= CCR_CLKEN;                   /* 使能                      */
  /*LPC_SC->PCONP &= ~(1 << 9);                   配置完成 不使用外部时钟   */
  return;
}

void RTCStop(void)
/********************************************************
**函数名称:RTCStart
**函数功能:内部RTC停止工作
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
{    
  LPC_RTC->CCR &= ~CCR_CLKEN;                    /* Stop RTC counters       */
  return;
} 

void RTCInit(void)
/********************************************************
**函数名称:RTCInit
**函数功能:内部RTC初始化
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
{
  LPC_SC->PCONP |=(1<<9);                    /* 访问 RTC 要用到外部时钟   */
  if(LPC_RTC->RTC_AUX&(0x1<<4))              /* RTC 振荡器失效探测标志    */
  {
    LPC_RTC->RTC_AUX |=(0x1<<4);               /* 清除                      */
  }
  LPC_RTC->CCR =0;                             /* 禁能                     */
  LPC_RTC->ILR =(ILR_RTCCIF|ILR_RTCALF);     /* 清除增量、报警中断 标志  */
  LPC_RTC->AMR =0xFF;                          /* 全屏蔽报警               */
  LPC_RTC->CIIR =0;                            /* 屏蔽增量报警             */
  LPC_RTC->CALIBRATION =0x0;                   /* 校准寄存器清0            */
  return;
}
