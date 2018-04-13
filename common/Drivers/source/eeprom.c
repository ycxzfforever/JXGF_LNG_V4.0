#include "kernel.h"
uint32_t    SYSPARA_ADDR=0;

/********************************************************
**函数名称:EEPROMInit
**函数功能:内部EEPROM初始化
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void EEPROMInit(void) 
{
  uint32_t regVal;

  LPC_EEPROM->PWRDWN = 0x0;
/* 
* 设置时钟，EEPROM 需要一个375kHz的时钟。这个时钟由系统时钟分频获得。
*/
  regVal =( SystemCoreClock / 375000 )-1;
  LPC_EEPROM->CLKDIV = regVal;
  regVal = LPC_EEPROM->CLKDIV;

  regVal  = ((((SystemCoreClock / 1000000) * 15) / 1000) + 1);
  regVal |= (((((SystemCoreClock / 1000000) * 55) / 1000) + 1) << 8);
  regVal |= (((((SystemCoreClock / 1000000) * 35) / 1000) + 1) << 16);
  LPC_EEPROM->WSTATE = regVal;
  return;
}

/********************************************************
**函数名称:EepromInterWriteNByte
**函数功能:从内部EEPROM的addr开始，写num个数据，数据内容是data
**输入参数:addr---EEPROM开始写的起始地址
           data---写入数据指针
           num----写入数据个数
**输出值  :0---表示起始地址加上写入个数超过4032
           1----正常操作

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t EepromInterWriteNByte(uint16_t addr,uint8_t *data,uint16_t num)
{
	uint16_t i;
	uint8_t result=1;
	uint8_t page_offset,page_address;
	if((addr+num)>4032)//内部EEPROM大小为(4K-64)字节。
	{
		return 0;
	}
	page_offset=(uint8_t)(addr&0x003f);//页偏移。
	page_address=(uint8_t)(addr>>6);   //页地址。
	LPC_EEPROM->INT_CLR_STATUS=((1<<EEPROM_ENDOF_RW)|(1<<EEPROM_ENDOF_PROG));
	LPC_EEPROM->ADDR=EEPROM_PAGE_OFFSET(page_offset);
																							/* 设置写数据地址              */
	LPC_EEPROM->INT_CLR_STATUS=(1<<EEPROM_ENDOF_RW);
	for(i=0;i<num;i++)
	{
		LPC_EEPROM->CMD=EEPROM_CMD_8_BIT_WRITE;                           //8位写入。
																			 /* 把数据写入页寄存器           */
		LPC_EEPROM->WDATA=*(data)++;
		page_offset+=1;
		if((page_offset>=EEPROM_PAGE_NUM)|(i==num-1)){
																				/* 把数据写入EEPROM中           */
			LPC_EEPROM->INT_CLR_STATUS=(0x1<< EEPROM_ENDOF_PROG);
			LPC_EEPROM->ADDR=EEPROM_PAGE_ADRESS(page_address);
			LPC_EEPROM->CMD=EEPROM_CMD_ERASE_PRG_PAGE;
			while(!((LPC_EEPROM->INT_STATUS>>28)&0x01));
																				/* 等待写完成       c            */
		}
		if(page_offset>=EEPROM_PAGE_NUM)
		{
			page_offset=0;
			page_address+=1;
			LPC_EEPROM->ADDR =0;
			if(page_address>EEPROM_PAGE_NUM-1) page_address=0;
		}
	}
	return result;
}

/********************************************************
**函数名称:EepromInterReadNByte
**函数功能:从内部EEPROM的addr开始，读um个数据，存入data
**输入参数:addr---EEPROM开始读的起始地址
           data---读出数据存放指针
           num----读出数据个数
**输出值  :0---表示起始地址加读出个数超过4032
           1----正常操作

**创建者  :黄豪民
**创建日期:2016-04-13

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t EepromInterReadNByte(uint16_t addr,uint8_t *data,uint16_t num)
{
	uint16_t i;
	uint8_t result=1;
	if((addr+num)>4032)
	{
		return 0;
	}
	LPC_EEPROM->INT_CLR_STATUS=((1<<EEPROM_ENDOF_RW)|(1<< EEPROM_ENDOF_PROG));
	LPC_EEPROM->ADDR=addr;
	LPC_EEPROM->CMD=EEPROM_CMD_8_BIT_READ|EEPROM_CMD_RDPREFETCH;
	for(i=0;i<num;i++){                  /* 读EEPROM中的数据                */
		LPC_EEPROM->INT_CLR_STATUS=(1<<EEPROM_ENDOF_RW);
		*data++=(uint8_t)(LPC_EEPROM->RDATA);			/* 等待读完成                     */
		while(!((LPC_EEPROM->INT_STATUS >> 26)&0x01));
	}
	return(result);
}

/********************************************************
**函数名称:SaveEeprom
**函数功能:蒋数据保存到内部EEPROM和外部EEPROM
**输入参数:addr---EEPROM开始写的起始地址
            data---写入数据指针
           num----写入数据个数
**输出值  :0---表示起始地址加读出个数超过4032
                            1----正常操作

**创建者  :杨朝旭
**创建日期:2016-12-9 12:55:38

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t SaveEeprom(uint16_t addr,uint8_t *data,uint16_t num)
{
    uint8_t     *t1,*t2;
    uint16_t    i;
    Sysparas    tmp1,tmp2;
    t1=(uint8_t*)&tmp1;
    t2=(uint8_t*)&tmp2;
    if(EepromInterWriteNByte(addr,data,num))//写内部eeprom正确
    {
        ClrBit(syserr,SYSERR_W_IN_EEPROM);
        if(At24C512WriteNByte(addr,data,num))//写外部eeprom正确
        {
            ClrBit(syserr,SYSERR_W_EX_EEPROM);
            if(EepromInterReadNByte(addr, t1, num))//读内部eeprom正确
            {
                if(At24C512ReadNByte(addr, t2, num))//读外部eeprom正确
                {
                    for(i=0;i<num;i++)
                    {
                        if(t1[i]!=t2[i])    break;                            
                    }
                    if(i==num)
                    {
                        return 1;
                    }
                    else if(i<num)
                    {
                        return 0;
                    }
                }
                else
                {
                    SetBit(syserr,SYSERR_R_EX_EEPROM);
                    return 0;
                }
            }
            else
            {
                SetBit(syserr,SYSERR_R_IN_EEPROM);
                return 0;
            }
        }
        else
        {
            SetBit(syserr,SYSERR_W_EX_EEPROM);//写外部eeprom错误。系统错误0x40。
            return 0;
        }
    }
    else
    {
        SetBit(syserr,SYSERR_W_IN_EEPROM);
        return 0;
    }  
    return 0;
}

/********************************************************
**函数名称:SaveSysparas
**函数功能:蒋参数保存到内部EEPROM和外部EEPROM
**输入参数:addr---EEPROM开始写的起始地址
            data---写入数据指针
           num----写入数据个数
**输出值  :0---保存失败
                            1----保存成功
**创建者  :杨朝旭
**创建日期:2016-4-19 09:10:38

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t SaveSysparas(uint16_t addr,uint8_t *data,uint16_t num)
{
    uint8_t     ret=0;
    uint16_t    crc;
    sysparas.writeE2num++;
    crc=CRC16Calculate(data, num-2);
    sysparas.crc=crc;
    if(sysparas.writeE2num<EEPROM_W_TIMES)
    {
        ret=SaveEeprom(addr, data, num);        
    }
    else//当eeprom写次数大于等于最大写次数时，先在本地址保存一次，然后地址进行偏移，随后再保存一次
    {
        ret=SaveEeprom(addr, data, num);
        addr +=sizeof(Sysparas); 
        SYSPARA_ADDR=addr;
        sysparas.writeE2num=1;
        crc=CRC16Calculate(data, num-2);
        sysparas.crc=crc;
        ret=SaveEeprom(addr, data, num);
    }
    if(SYSPARA_ADDR > SYSPARA_MAX_ADDR) globalvar.eepromflag=1;
	return ret;
}

/********************************************************
**函数名称:ReadSysparas
**函数功能:蒋参数从内部EEPROM和外部EEPROM读出
**输入参数:addr---EEPROM开始读的起始地址
            data---读取数据指针
           num---读取数据个数
**输出值  :0---表示起始地址加读出个数超过4032
                            1----正常操作

**创建者  :杨朝旭
**创建日期:2016-4-19 09:10:38

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint8_t ReadSysparas(uint16_t addr,uint8_t *data,uint16_t num)
{
    uint16_t    i,crc;
    Sysparas    tmp1,tmp2;
    uint8_t     *t1,*t2,ret=FALSE;
    t1=(uint8_t*)&tmp1;
    t2=(uint8_t*)&tmp2;
    if(EepromInterReadNByte(addr, t1, num))//读内部eeprom正确
    {
        crc=CRC16Calculate(t1, num-2);
        if(tmp1.crc==crc)
        {
            ClrBit(syserr,SYSERR_R_IN_EEPROM);
            ret=TRUE;//内部eeprom数据正确
        }
        else
        {
            if(tmp1.writeE2num>100000) tmp1.writeE2num=0;//防止首次读一块新板子，数据全为0xFF
            SetBit(syserr,SYSERR_R_IN_EEPROM);
            ret=FALSE;//内部eeprom数据错误
        }
        //即使CRC错误也赋值
        for(i=0;i<num;i++)
        {
            data[i]=t1[i];            
        }        
    }
    if(ret==FALSE)//如果内部eeprom读错，再读外部eeprom
    {
        if(At24C512ReadNByte(addr, t2, num))//读外部eeprom正确
        {
            crc=CRC16Calculate(t2, num-2);
            if(tmp2.crc==crc)
            {
                ClrBit(syserr,SYSERR_R_EX_EEPROM);
                ret=TRUE;//外部eeprom数据正确
            }
            else
            {
                if(tmp2.writeE2num>100000) tmp2.writeE2num=0;//防止首次读一块新板子，数据全为0xFF
                SetBit(syserr,SYSERR_R_EX_EEPROM);
                ret=FALSE;//外部eeprom数据错误
            }  
            //即使CRC错误也赋值
            for(i=0;i<num;i++)
            {
                data[i]=t2[i];            
            }
        }
    }
    return ret;
}

