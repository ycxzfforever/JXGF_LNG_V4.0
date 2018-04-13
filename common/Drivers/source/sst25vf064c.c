#include "kernel.h"

/***************************************************************************
**函数名称：FlashSpiRead
**函数功能：通过Spi0向SST读出数据。
**入口参数：无
**返 回 值：dat--读出数据。
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t FlashSpiRead(void)
{
    uint8_t  dat = 0;
    WATCH_DOG();
    Fm25L04AndFlashSpiReceive(&dat,1 );
    return dat;  
}

/***************************************************************************
**函数名称：FlashSpiWrite
**函数功能：通过Spi0向SST写入数据。
**入口参数：*buff--数据组；
           num--数据个数。
**返 回 值：num
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint32_t FlashSpiWrite(uint8_t *buff, uint32_t num)
{   
    WATCH_DOG();
    Fm25L04AndFlashSpiSend(buff, num);
    return num;
}

/***************************************************************************
**函数名称：SpiSimpleCmd
**函数功能：写状态寄存器。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            cmd--要写入的命令。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void SpiSimpleCmd(uint8_t flashnum,uint8_t cmd)
{
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();  //使能Flash1.
    else
        SPI_FLASH_B_ENABLE();  //使能Flash2.
    FlashSpiWrite(&cmd, 1);    //向Flash写入1个命令。
    if(flashnum==0)
        SPI_FLASH_A_DISABLE(); //禁能Flash1.
    else
        SPI_FLASH_B_DISABLE(); //禁能Flash2.
}  

/***************************************************************************
**函数名称：SSTReadID
**函数功能：读取制造商ID或者器件ID。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTReadID(uint8_t flashnum)
{
    uint8_t  dataout[4], datain[4];
    uint8_t id, i;
    
    dataout[0] = SPI_FLASH_RDID;
    dataout[1] = 0x00;
    dataout[2] = 0x00;
    dataout[3] = 0x00;//0x00将先输出制造商ID--BF，然后输出器件ID--4B；0x01反之。
                      //0x00输出制造商ID（BF值，或者4B值），0x01输出器件ID（输出4B值，或者BF值。）
    
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();
    else
        SPI_FLASH_B_ENABLE();
    
    FlashSpiWrite(dataout, 4);
    
    for(i = 0; i < 4; i++) {
       datain[i] = FlashSpiRead();
    } 
    
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();
    else
        SPI_FLASH_B_DISABLE();
    
    id=datain[0]; 
    return id;

}

/***************************************************************************
**函数名称：SSTReadFlashStatus
**函数功能：读取Flash状态寄存器的状态值。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTReadFlashStatus(uint8_t flashnum)
{
    uint8_t datain, dataout[1];
    
    dataout[0] = SPI_FLASH_RDSR;       //读取状态寄存器。
    
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();          //使能Flash1片选。
    else
        SPI_FLASH_B_ENABLE();          //使能Flash2片选。
         
    FlashSpiWrite(dataout, 1);         //发送读取状态寄存器指令。
 
    datain = FlashSpiRead();           //读取状态寄存器。
  
    
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();         //禁能Flash1片选。
    else
        SPI_FLASH_B_DISABLE();         //禁能Flash2片选。

    return datain;
}

/***************************************************************************
**函数名称：WriteSpiFlashStatus
**函数功能：向状态寄存器写入值。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            status--要写入的状态值。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void WriteSpiFlashStatus(uint8_t flashnum,uint8_t status)
{
    uint8_t dataout[2];
    
    SpiSimpleCmd(flashnum,SPI_FLASH_WREN);                                      /*  发送写使能信号               */ 
    
    dataout[0] = SPI_FLASH_WRSR;
    dataout[1] = status;
    
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();
    else
        SPI_FLASH_B_ENABLE();
    
    FlashSpiWrite(dataout, 2);    

    if(flashnum==0)
        SPI_FLASH_A_DISABLE();
    else
        SPI_FLASH_B_DISABLE();
    MsDelay(30);//新的flash必须的延时(至少大于15ms)，否则操作不成功
}

/***************************************************************************
**函数名称：SSTEraseSector
**函数功能：擦除某一扇区的值，擦除后值为0xFF。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            addr--要擦除的地址。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTEraseSector(uint8_t flashnum,uint32_t addr)
{
    uint8_t  dataout[4];
    
    if(addr > MAX_ADDR) {
        return FALSE; 
    }
    
    SpiSimpleCmd(flashnum,SPI_FLASH_WREN);               //发送写使能信号。   

    dataout[0] = SPI_FLASH_SE;                           //擦除4k的存储器阵列。
    dataout[1] = (uint8_t)((addr&0xFFFFFF) >> 16);       //地址1。
    dataout[2] = (uint8_t)((addr&0xFFFF) >>  8);         //地址2。
    dataout[3] = (uint8_t)(addr&0xFF);                   //地址3。

    if(flashnum==0)
        SPI_FLASH_A_ENABLE();
    else
        SPI_FLASH_B_ENABLE();
    
    FlashSpiWrite(dataout, 4);
    
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();
    else
        SPI_FLASH_B_DISABLE();
    
    while ((SSTReadFlashStatus(flashnum) & 0x01) == 0x01);//判断忙标志，0不忙，1忙。
    
    return TRUE; 
}

/***************************************************************************
**函数名称：SSTEraseBlock
**函数功能：擦除某一块的值，每一块64K空间。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            addr--要擦除的地址。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTEraseBlock(uint8_t flashnum,uint32_t addr)
{
    uint8_t  dataout[4];
    
    if(addr > MAX_ADDR) {
        return FALSE; 
    }
    
    SpiSimpleCmd(flashnum,SPI_FLASH_WREN);        //发送写使能信号。              

    dataout[0] = SPI_FLASH_BE;                    //擦除块指令。
    dataout[1] = (uint8_t)((addr&0xFFFFFF) >> 16);//地址1。
    dataout[2] = (uint8_t)((addr&0xFFFF) >>  8);  //地址2。
    dataout[3] = (uint8_t)(addr&0xFF);            //地址3。

    if(flashnum==0)
        SPI_FLASH_A_ENABLE();
    else
        SPI_FLASH_B_ENABLE();
    
    FlashSpiWrite(dataout, 4);                    //发送擦除指令。
    
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();
    else
        SPI_FLASH_B_DISABLE();
    while ((SSTReadFlashStatus(flashnum) & 0x01) == 0x01);   //判断忙标志，0不忙，1忙。
       
    return TRUE;  
}

/***************************************************************************
**函数名称：SSTEraseBlock
**函数功能：擦除某一块的值，每一块64K空间。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            sector--要擦除的块。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void SSTEraseChip(uint8_t flashnum)
{
    SpiSimpleCmd(flashnum,SPI_FLASH_WREN);                 //发送写使能信号。
    SpiSimpleCmd(flashnum,SPI_FLASH_CE);                   //整片擦除。
    while ((SSTReadFlashStatus(flashnum) & 0x01) == 0x01) ;//等待擦除结束。
}

/***************************************************************************
**函数名称：SSTReadNbyte
**函数功能：读取指定地址的数据。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            addr--存储器存储首地址；
            *src--要存入的数据首地址；
            num--存储数据个数。
**返 回 值：bool/1/0
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTReadNbyte(uint8_t flashnum,
                   uint32_t addr, 
                   uint8_t *dst, 
                   uint32_t num, 
                   uint8_t readmode)
{
    uint8_t  dataout[5];

    if ((addr > MAX_ADDR) || ((readmode != SPI_FLASH_READ) &&
                              (readmode != SPI_FLASH_FAST_READ))) 
    {
        return FALSE;
    }

    dataout[1] = (uint8_t)((addr&0xFFFFFF) >> 16);//地址1。
    dataout[2] = (uint8_t)((addr&0xFFFF) >>  8);  //地址2。
    dataout[3] = (uint8_t)(addr&0xFF);            //地址3。
    dataout[4] = 0xff;
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();
    else
        SPI_FLASH_B_ENABLE();
    
    if(readmode == SPI_FLASH_READ) 
    {  
        dataout[0] = SPI_FLASH_READ;     //普通读模式 。
        FlashSpiWrite(dataout, 4);
    } 
    else 
    {
       dataout[0] = SPI_FLASH_FAST_READ; //快速读模式 。
       FlashSpiWrite(dataout, 5);
    }

    while (num-- > 0) {
        *dst++= FlashSpiRead();
    }
    
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();
    else
        SPI_FLASH_B_DISABLE();

    return TRUE;
}

/***************************************************************************
**函数名称：SSTWriteNbyte
**函数功能：页编程，向相应地址写入数据。每页可最大存储256字节数据，如果超过，
            将从目标页的起始地址开始编程。并且所选页编程地址必须处于擦除状态，即FF。
            每次页编程，必须对页首地址进行计算。
**入口参数：flashnum--第一块Flash值为0，第二块Flash值为1；
            addr--存储首地址；
            *src--要存入的数据首地址；
            num--存储数据个数。
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTWriteNbyte( uint8_t  flashnum,
                      uint32_t addr, 
                      uint8_t  *src, 
                      uint32_t num )
{
    uint8_t  dataout[4];
    
    if ((addr > MAX_ADDR) || (num > 256)) {
        return FALSE;
    }
    
    SpiSimpleCmd(flashnum,SPI_FLASH_WREN);   // 发送写使能信号 。            
    
    dataout[0] = SPI_FLASH_PP;               //页编程命令。
    dataout[1] = (uint8_t)((addr&0xFFFFFF) >> 16);//地址1。
    dataout[2] = (uint8_t)((addr&0xFFFF) >>  8);  //地址2。
    dataout[3] = (uint8_t)(addr&0xFF);            //地址3。
  
    if(flashnum==0)
        SPI_FLASH_A_ENABLE();               //Flash1片选使能。
    else
        SPI_FLASH_B_ENABLE();               //Flash2片选使能。

    FlashSpiWrite(dataout, 4);              //发送命令和存储地址。

    FlashSpiWrite(src, num);                //发送存储数据。
 
    if(flashnum==0)
        SPI_FLASH_A_DISABLE();              //片选禁能。
    else
        SPI_FLASH_B_DISABLE();              //片选禁能。   
    while ((SSTReadFlashStatus(flashnum) & 0x01) == 0x01);  //判忙？等待操作结束。
    
    return TRUE;
}

/***************************************************************************
**函数名称：SSTFlashTest
**函数功能：读写Flash数据，验证Flash正误。
**入口参数：flashnum: 0--Flash1，1--Flash2。
**返 回 值：ErrorFlag=0，无误；否则有误。
**创 建 者：杨传坤
**创建日期：2016-4-12

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SSTFlashTest(uint8_t flashnum)
{
    uint8_t i = 0;
    uint8_t ErrorFlag = 0;                     //出错标记。
    //uint8_t flashnum=1;                        //Flash选择，0--Flash1，1--Flash2。
	uint8_t Status;
    uint8_t WriteBuff[100],ReadBuff[100];

    for (i = 0; i < 100; i++) {                 //初始化数据缓冲区  
        WriteBuff[i] = i;
        ReadBuff[i]  = 0;
    }                                              

    WriteSpiFlashStatus(flashnum,0x00);        //写状态。上电BP3，BP2，BP1，BP0将置1，所有块被保护，不能读写。
    Status=SSTReadFlashStatus(flashnum);  	   //读状态。
	if(Status!=0)
	{
		ErrorFlag=1;
		return ErrorFlag;
	}
	
    SSTEraseSector(flashnum,FlashTestSADDR);
    SSTWriteNbyte(flashnum,FlashTestSADDR, WriteBuff, 100);  //存储数据，编程Flash。
//    uart2SendStr(WriteBuff,20);
    MsDelay(5);
  
    SSTReadNbyte(flashnum,FlashTestSADDR, ReadBuff, 100, SPI_FLASH_FAST_READ); //从Flash读出20字节数据。
//    uart2SendStr(ReadBuff,20);
    for(i=0;i<100;i++)
    {
        if(WriteBuff[i]!=ReadBuff[i])
        {
            ErrorFlag=1;
            break;
        }
        else  
        {
            ErrorFlag=0;
        }
    }
	return ErrorFlag;
}

/***************************************************************************
**函数名称：SaveFuelRecord
**函数功能：保存加气流水记录
**入口参数：0-存流水；1-存班流水。
**返 回 值：TRUE:成功，FALSE:失败
**创 建 者：杨朝旭
**创建日期：2016-4-26 14:12:09

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SaveFuelRecord(uint8_t select)
{
    uint8_t         ret=FALSE,*pt,*pt1,*pt2;
    uint32_t addr ,i,num,id;
    if(select==0)
    {
        FuelRecordInfo  tmp1,tmp2;
        num=sizeof(FuelRecordInfo);
        id = sysparas.transnum-1;  //记录流水号从0开始的
        pt=(uint8_t *)(&fuelrecordinfo.transnum);
        pt1=(uint8_t *)(&tmp1.transnum);
        pt2=(uint8_t *)(&tmp2.transnum);
        id = id%FlashRecMaxID;
        addr = FlashRecSADDR + id*FlashRecLen;
    }
    
    if(select==1)
    {
        ShiftRecordInfo tmp1,tmp2;
        num=sizeof(shiftrecordinfo);
        id = sysparas.shiftransnum-1;  //记录班 流水号从0开始的
        pt=(uint8_t *)(&shiftrecordinfo.classid);
        pt1=(uint8_t *)(&tmp1.classid);
        pt2=(uint8_t *)(&tmp2.classid);
        id = id%FlashShiftMaxID;
        addr = FlashShiftSADDR + id*FlashShiftLen;
    }
    
    if(addr%SEC_SIZE==0)//若是一个扇区首地址，则擦除后面的4k字节
	//if((addr&0x0fff)==0)
    {
        WriteSpiFlashStatus(0,0x00);
        SSTEraseSector(0,addr);
        MsDelay(5);
        WriteSpiFlashStatus(1,0x00);
        SSTEraseSector(1,addr);
        MsDelay(5);
    }
    if(SSTWriteNbyte(0,addr,pt,num))//写入主存
    {
        MsDelay(5);
        if(SSTReadNbyte(0, addr,pt1,num, SPI_FLASH_FAST_READ))//从主存中读出
        {
            for(i=0; i<num; i++)
            {
                if(pt1[i]!=pt[i]) break; //读出数据与写入数据不一致
            }
            if(i==num)
            {
                ret = TRUE;
            }
            else if(i<num)
            {
                ret = FALSE;
            }
        }
        else
        {
            ret=FALSE;
        }
    }
    else
    {
        SetBit(syserr,SYSERR_W_FLASH_A);//写主存错。系统错误0x200
        ret=FALSE;
    }
    if(SSTWriteNbyte(1,addr,pt,num))//写入副存
    {
        MsDelay(5);
        if(SSTReadNbyte(1, addr,pt2,num, SPI_FLASH_FAST_READ))//从副存中读出
        {
            for(i=0; i<num; i++)
            {
                if(pt2[i]!=pt[i]) break; //读出数据与写入数据不一致
            }
            if(i==num)
            {
                if(ret!=TRUE) ret=FALSE;//主存写错时
            }
            else if(i<num)
            {
                ret = FALSE;
            }
        }
        else
        {
            ret=FALSE;
        }
    }
    else
    {
        SetBit(syserr,SYSERR_W_FLASH_B);//写副存错。系统错误0x800。
        ret=FALSE;
    }
    return ret;
}

/***************************************************************************
**函数名称：ReadFuelRecord
**函数功能：读加气流水记录
**入口参数：id:流水号
**返 回 值：TRUE:成功，FALSE:失败
**创 建 者：杨朝旭
**创建日期：2016-4-26 15:37:37

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ReadFuelRecord(uint32_t id,uint8_t select)
{
    uint8_t         ret=FALSE,*pt1,*pt2;
    uint16_t        crc;
    uint32_t        addr,num;
    FuelRecordInfo  tmp1,tmp2;
    ShiftRecordInfo tmp3,tmp4;
    if(select==0)
    {
        num=sizeof(FuelRecordInfo);
        pt1=(uint8_t *)(&tmp1.transnum);
        pt2=(uint8_t *)(&tmp2.transnum);
        id = id%FlashRecMaxID;
        addr = FlashRecSADDR + id*FlashRecLen;
    }
    
    if(select==1)
    {
        num=sizeof(shiftrecordinfo);
        pt1=(uint8_t *)(&tmp3.classid);
        pt2=(uint8_t *)(&tmp4.classid);
        id = id%FlashShiftMaxID;
        addr = FlashShiftSADDR + id*FlashShiftLen;
    }
    
    if(SSTReadNbyte(0, addr,pt1,num, SPI_FLASH_FAST_READ))//从主存中读出
    {
        MsDelay(10);
        crc=CRC16Calculate(&pt1[0], num-2);
        if((tmp1.crc==crc)||(tmp3.crc==crc))
        {
            ClrBit(syserr,SYSERR_R_FLASH_A);            
            ret=TRUE;
        }
        else
        {
            if((tmp1.transnum==0xFFFFFFFF)&&(select==0))//若主板更换新flash
                memset(pt1,0,sizeof(FuelRecordInfo));
            else if((tmp3.classid==0xFFFFFFFF)&&(select==1))//若主板更换新flash
                memset(pt1,0,sizeof(ShiftRecordInfo));   
            else
                SetBit(syserr,SYSERR_R_FLASH_A);//读主存错。系统错误0x400。
            ret=FALSE;
        }
        //即使CRC错误也赋值
        if(select==0)
        {
            fuelrecordinfo=tmp1;
            
        }
        else if(select==1)
        {
            shiftrecordinfo=tmp3;            
        }
    }
    else
    {
        SetBit(syserr,SYSERR_R_FLASH_A);//读主存错。系统错误0x400。
        ret=FALSE;
    }
    if(ret==FALSE)//主存如果读错误再读副存
    {
        if(SSTReadNbyte(1, addr,pt2,num, SPI_FLASH_FAST_READ))//从副存中读出
        {
            MsDelay(10);
            crc=CRC16Calculate(pt2, num-2);
            if((tmp2.crc==crc)||(tmp4.crc==crc))
            {
                ClrBit(syserr,SYSERR_R_FLASH_B);
                ret=TRUE;
            }
            else
            {
                if((tmp2.transnum==0xFFFFFFFF)&&(select==0))//若主板更换新flash
                    memset(pt2,0,sizeof(FuelRecordInfo));
                else if((tmp4.classid==0xFFFFFFFF)&&(select==1))//若主板更换新flash
                    memset(pt2,0,sizeof(FuelRecordInfo)); 
                else
                    SetBit(syserr,SYSERR_R_FLASH_B);//读副存错。
                ret=FALSE;
            }
            //即使CRC错误也赋值
            if(select==0)
            {
                fuelrecordinfo=tmp2;
                
            }
            else if(select==1)
            {
                shiftrecordinfo=tmp4;              
            }
        }
        else
        {
            SetBit(syserr,SYSERR_R_FLASH_B);//读副存错。系统错误0x1000。
            ret=FALSE;
        }
    }
    return ret;
}
/***************************************************************************
**函数名称：SaveFlash
**函数功能：存储黑名单，新增-新删黑名单，白名单。
**入口参数：flashnum-flash选择；addr-数据要存储的地址；*src-数据首地址；num-数据个数。
**返 回 值：0
**创 建 者：杨传坤
**创建日期：2016-4-26 15:37:37

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SaveFlash(uint8_t  flashnum,uint32_t addr, uint8_t  *src, uint32_t num )
{
    uint32_t addroffset, writenum;
	while(num > 0)                                             //
	{
		if(addr%SEC_SIZE==0)//若是一个扇区首地址，则擦除后面的4k字节
		{
			if(flashnum == 0)
			{
				WriteSpiFlashStatus(0,0x00);
				SSTEraseSector(0,addr);
				MsDelay(5);
			}
			else
			{
				WriteSpiFlashStatus(1,0x00);
				SSTEraseSector(1,addr);
				MsDelay(5);
			}
		}
		if(addr > MAX_ADDR)                                    //判断是否超过最大地址。
		{
			return 0;
		}

		addroffset = addr % PAGE_SIZE;                         //求页偏移。
		if((num < PAGE_SIZE) && (addroffset + num < PAGE_SIZE))//如果一页能写完数据。
		{
			writenum = num;
		}
		else                                                   //如果一页不能写完数据。
		{
			writenum = PAGE_SIZE - addroffset;
		}

        MsDelay(1);
	
        if(SSTWriteNbyte( flashnum,addr, src, writenum ) == TRUE)
		{   //向Flash写入数据。
    		addr += writenum;
    		num  -= writenum;
            src  += writenum;
		}
		else
			return 0;
	}
    return 1;
}


uint8_t ReadFlash(uint8_t flashnum,uint32_t addr,uint8_t *pt,uint32_t num)
{
    uint8_t ret=FALSE;
    if(flashnum==0)
    {
        ret=SSTReadNbyte(0, addr,pt,num, SPI_FLASH_FAST_READ);
    }
    if(flashnum==1)
    {
        ret=SSTReadNbyte(1, addr,pt,num, SPI_FLASH_FAST_READ);
    }
    return ret;
}
/*********************************************************************************************************
  End File
*********************************************************************************************************/
