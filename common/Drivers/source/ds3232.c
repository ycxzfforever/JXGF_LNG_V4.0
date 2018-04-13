#include "kernel.h"

Time    time;

/********************************************************
**函数名称:Ds3232ReadTime
**函数功能:从DS3232读出时间
**输入参数:
**输出值  :输出时间（顺序为：年月日时分秒，年的范围为00~99，格式为BCD）
           返回值为0时表示出错，为1时表示操作正确。
           
**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t Ds3232ReadTime(void)
{
	uint8_t tmpbuffer[7];
	uint8_t result;
	result=I2C0ReadNByte(0xd0,1,0x00,tmpbuffer,7);
    if(result==TRUE)//ycx20161129读正确才更新时间
    {
    	time.year   =tmpbuffer[6];
    	time.month  =tmpbuffer[5];
    	time.day    =tmpbuffer[4];
    	time.hour   =tmpbuffer[2];
    	time.minute =tmpbuffer[1];
    	time.second =tmpbuffer[0];
    }
	return result;
}

/********************************************************
**函数名称:Ds3232SetTime
**函数功能:DS3232设置时间
**输入参数:输入时间（顺序为：年月日时分秒，年的范围为00~99，格式为BCD）
**输出值  :返回值为0时表示出错，为1时表示操作正确。

**创建者  :黄豪民
**创建日期:2016-04-14

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t Ds3232SetTime(uint8_t *buffer)
{
	uint8_t tmpbuffer[7];
	tmpbuffer[0]=buffer[5];
	tmpbuffer[1]=buffer[4];
	tmpbuffer[2]=buffer[3];
	tmpbuffer[3]=0;
	tmpbuffer[4]=buffer[2];
	tmpbuffer[5]=buffer[1];
	tmpbuffer[6]=buffer[0];
	return I2C0WriteNByte(0xd0,1,0x00,tmpbuffer,7);
}

