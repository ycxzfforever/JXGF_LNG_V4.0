#include "kernel.h"

/********************************************************
**函数名称:At24c512Init
**函数功能:At24c512初始化
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void At24c512Init(void)
{
	LPC_GPIO3->DIR |=1ul<<26;	               /*片保护*/
	LPC_GPIO3->CLR |=1ul<<26;
	UsDelay(2);
}

/********************************************************
**函数名称:At24C512WriteNByte
**函数功能:向At24C512写num个数据
**输入参数:addr---起始地址
					 data---写入数据指针
           num----写入数据个数
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t At24C512WriteNByte(uint32_t addr, uint8_t *data, uint16_t num)
{
	uint8_t pageoffset,pagenum;
	uint32_t startaddr;
	uint16_t count;
	startaddr=addr;
	count=0;
	pageoffset=(uint8_t)(addr&0x007f);
	while(count<num)
	{
		if((startaddr&0x007f)==0)
		{
			if((num-count)>128)
			{
				pagenum=128;
			}
			else
			{
				pagenum=num-count;
			}
		}
		else
		{
			pagenum=128-pageoffset;
			if(num<=pagenum)
			{
				pagenum=num;
			}
		}
		I2C0WriteNByte(0xa0,2,startaddr,data+count,pagenum);
		MsDelay(50);
		count+=pagenum;
		startaddr+=pagenum;
	}
	return 0x01;
}

/********************************************************
**函数名称:At24C512ReadNByte
**函数功能:从At24C512读num个数据
**输入参数:addr---起始地址
					 data---读数据指针
           num----读数据个数
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t At24C512ReadNByte(uint32_t addr, uint8_t *data, uint32_t num)
{
	return I2C0ReadNByte(0xa0,2,addr,data,num);
}

