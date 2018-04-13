#include "kernel.h"


uint16_t Fm25l04TestAdd = 0x100;
/***************************************************************************
**函数名称：FmSpiWriteByte
**函数功能：通过spi0向FM25L04写入一个数据。
**入口参数：tem--要写入的数据。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t FmSpiWriteByte(uint8_t tem)
{  
    Fm25L04AndFlashSpiSend(&tem, 1);
	return 0;
}

/***************************************************************************
**函数名称：FmSpiReadByte
**函数功能：通过spi0向FM25L04读出一个数据。
**入口参数：无
**返 回 值：tem--读出的数据。
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t FmSpiReadByte(void)
{
	uint8_t tem = 0;
    Fm25L04AndFlashSpiReceive(&tem,1);
	return tem;
}

/***************************************************************************
**函数名称：Fm25l04Write
**函数功能：向FM25L04的特定地址写入一定长度的数据。
**入口参数：Addr--存储首地址；
            *Data--存储数据；
            Length--数据长度。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void Fm25l04Write(uint16_t Addr,uint8_t *Data,uint16_t Length)
{
	uint32_t i;
	uint16_t temp1 = 0xfff;
	uint16_t temp2 = 0xfff;

	SPI0_CS_ENABLE(); 
	FmSpiWriteByte(SPI_WREN);          //写使能。
	SPI0_CS_DISABLE();

	SPI0_CS_ENABLE();  

	temp1=(uint8_t)((Addr&0x100)>>5);
	temp2=SPI_WRITE|temp1;           //得到写存储寄存器操作和9位地址的MSB。    
	FmSpiWriteByte(temp2);             //写存储寄存器。
	FmSpiWriteByte((uint8_t)(Addr));	 //写地址。

	for(i=0;i<Length;i++)
	{
		FmSpiWriteByte(*(Data+i));     //写入数据。
	}
	SPI0_CS_DISABLE();

}

/***************************************************************************
**函数名称：Fm25l04Read
**函数功能：向FM25L04的特定地址读出一定长度的数据。
**入口参数：Addr--存储首地址；
            *Data--存储数据；
            Length--数据长度。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void Fm25l04Read(uint16_t Addr,uint8_t *Data,uint16_t Length)
{
	uint32_t i;
    uint16_t temp1 = 0xfff;
	uint16_t temp2 = 0xfff;
    
	SPI0_CS_ENABLE(); 
    temp1=(uint8_t)((Addr&0x100)>>5);
    temp2=SPI_READ|temp1;             //得到读存储寄存器操作和9位地址的MSB。
	FmSpiWriteByte(temp2);             
	FmSpiWriteByte((uint8_t)(Addr));	  //写地址。

	for(i=0;i<Length;i++)
	{
		*(Data+i)=FmSpiReadByte();      //写数据。
	}
	SPI0_CS_DISABLE();
}

/***************************************************************************
**函数名称：Fm25l04Test
**函数功能：向FM25L04写入指定的数据，并将其读出，比较二者数据，判断正误。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：杨朝旭
**修改日期：2016-12-8 10:47:46
**修改内容：测试铁电次数buf使用联合体
***************************************************************************/
uint8_t Fm25l04Test(void)
{
 	uint8_t DetData[10]={0x11,0x22,0x33,0x44,0x55,0xAA,0xBB,0xCC,0xDD,0xEE};
	uint8_t DetData_Temp[10]={0};
	uint8_t i,ErrorFlag=0;
    UnionU64    tmp;
    
	Fm25l04Read(Fm25l04TestAdd,&tmp.rdcv[0],8);//读出测试铁电的总次数
	tmp.data++;
	Fm25l04Write(Fm25l04TestAdd,&tmp.rdcv[0],8); 
	
	memset(DetData_Temp,0,sizeof(DetData_Temp)); 
	Fm25l04Write(Fm25l04TestAdd+8,DetData,10);      //写测试数据   
	Fm25l04Read(Fm25l04TestAdd+8,DetData_Temp,10);	//读测试数据  
	for(i=0;i<10;i++)
	{
		if(DetData_Temp[i]!=DetData[i])
		{
			ErrorFlag=1;
			Fm25l04Read(Fm25l04TestAdd+18,&tmp.rdcv[0],8);//如果出错读出测试铁电出错总次数
			tmp.data++;
			Fm25l04Write(Fm25l04TestAdd+18,&tmp.rdcv[0],8);
            SetBit(syserr,SYSERR_T_FM25L04);
			break;
		}
		else
        {  
			ErrorFlag=0;            
        }
	}
    if((ErrorFlag==0)&&((GetBit(syserr,SYSERR_T_FM25L04))==1))
    {
        ClrBit(syserr,SYSERR_T_FM25L04);
    }
	return(ErrorFlag); 
}


