#include "kernel.h"
/***************************************************************************
**函数名称：BQresults(void)
**函数功能：后台发送的电子标签结果。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-11-22
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void BQresults(void)
{
	Fyd12864ClearLine(1); 
    Fyd12864ClearLine(3);
	Fyd12864ClearLine(4); 
	if(globalvar.startstate==0)
	{
		 Fyd12864DispPrintf(2,1,"已经授权，请等待");
		 TaskMessage(20, StartPrecooling);
		 DispNextUI=StartFueling;
	}
	else if(globalvar.startstate==1)
	{
		Fyd12864DispPrintf(2,1,"  非法标签...");
		globalvar.startstate=0xff;          			//清标签命令。
	}
	else if(globalvar.startstate==2)
	{
		Fyd12864DispPrintf(2,1,"  钢瓶故障...");
		globalvar.startstate=0xff;          			//清标签命令。
	}
	else if(globalvar.startstate==3)
	{
		Fyd12864DispPrintf(2,1,"  钢瓶过期...");
		globalvar.startstate=0xff;          			//清标签命令。
	}
	if(globalvar.startstate!=0)
	{
		TaskMessage(20, DispFirst);
	}
}

/***************************************************************************
**函数名称：WaitBQresults(void)
**函数功能：定量加气时，等待授权。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-11-22
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void WaitBQresults(void)
{
	Fyd12864ClearLine(1); 
    Fyd12864ClearLine(3);
	Fyd12864ClearLine(4);
	
	Fyd12864DispPrintf(2,1,"尚未授权，请等待");
	if(globalvar.startstate!=0xff)
	{
		DispUI=BQresults;
	}
	TaskAdd(KEY_RET, DispFirst,NULL);
}
