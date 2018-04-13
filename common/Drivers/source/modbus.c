#include "kernel.h"


MassFlowModbus  modbus;
RealTimeData    realtime;

uint16_t _MODBUS_G_AUTO_RM_BUF[]= {0/*流速*/,2/*总量*/,1/*温度*/,3/*密度*/,9/*压力*/};
uint16_t _MODBUS_L_AUTO_RM_BUF[]= {0/*流速*/,2/*总量*/,1/*温度*/,3/*密度*/,47/*增益*/};

//9503为增益寄存器，直接读的范围为0-100，通过扫描寄存器读取则范围为:0-0.1
uint16_t _MODBUS_G_AUTO_EH_BUF[]= {2007/*流速*/,2610/*总量*/,2017/*温度*/,2013/*密度*/,2089/*压力*/};
uint16_t _MODBUS_L_AUTO_EH_BUF[]= {2007/*流速*/,2610/*总量*/,2017/*温度*/,2013/*密度*/,9503/*增益*/};


/***************************************************************************
**函数名称： ModbusCrc16
**函数功能：流量计CRC16计算
**入口参数：buf:需要计算CRC的数据，len:需要计算的数据长度
**返回值：计算后CRC
**创建者：杨朝旭
**创建日期：2016-3-30 15:45:03
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint16_t ModbusCrc16(uint8_t *buf, uint16_t Len)
{
    uint16_t IX, IY, CRC;
    CRC = 0xFFFF;
    if(Len <= 0)    return 0;
    for(IX = 0; IX < Len; IX++)
    {
        CRC = CRC ^ (uint16_t)(buf[IX]);
        for(IY = 0; IY < 8; IY++)
        {
            if((CRC &1) != 0)
                CRC = (CRC >> 1) ^ 0xA001;
            else
                CRC = CRC >> 1;
        }
    }
    return CRC;
}

/***************************************************************************
**函数名称： ModbusReadScanReg
**函数功能：读取扫描寄存器的值
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    RegAddr:寄存器地址
                                    regnum:寄存器数量
**返回值：无
**创建者：杨朝旭
**创建日期：2016-3-31 15:47:27
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusReadScanReg(uint8_t modbus_addr,uint16_t RegAddr, uint8_t regnum)
{
	uint16_t  crc;
    uint32_t  timecount=0;
    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc=ModbusCrc16(&modbus.sendbuf[0],6);//FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc &0x00FF);//校验值低位
    modbus.sendbuf[7] = crc >>8;

    OpenUartRcvIRQ(ModbusPort);
    UartSendStr(ModbusPort,&modbus.sendbuf[0],8);
    while(timecount<MODBUS_TIMEOUT)
    {
        timecount++;
        if(modbus.flag==1)
        {
            modbus.flag=0;
			ModbusDataDisposs();
            return TRUE;
        }
    }
    CloseUartRcvIRQ(ModbusPort);
    return FALSE;
}

/***************************************************************************
**函数名称： ModbusReadReg
**函数功能：读取指定寄存器的值
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    RegAddr:寄存器地址
                                    regnum:寄存器数量
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:47:27
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusReadReg(uint8_t modbus_addr,uint16_t RegAddr, uint8_t regnum)
{
    uint16_t  crc;
    uint32_t  timecount=0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = 0x03;             //读寄存器的功能代码
    modbus.sendbuf[2] = RegAddr >> 8;     //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;   //寄存器低位
    modbus.sendbuf[4] = 0;                //number of registers
    modbus.sendbuf[5] = regnum;
    crc=ModbusCrc16(&modbus.sendbuf[0],6);//FLOW_CRC16//进行CRC校验
    modbus.sendbuf[6] = (uint8_t)(crc &0x00FF);//校验值低位
    modbus.sendbuf[7] = crc >>8;

    OpenUartRcvIRQ(ModbusPort);
    UartSendStr(ModbusPort,&modbus.sendbuf[0],8);
    while(timecount<MODBUS_TIMEOUT)
    {
        timecount++;
        if(modbus.flag==1)
        {
            modbus.flag=0;
			globalvar.modbus_g_error=0;
            return TRUE;
        }
    }
    CloseUartRcvIRQ(ModbusPort);
    return FALSE;
}


/***************************************************************************
**函数名称： ModbusWriteReg
**函数功能：读取指定寄存器的值
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    modbus_coil:写寄存器功能码
                                    RegAddr:寄存器地址
                                    v_value:需要写入寄存器的值
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:50:39
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusWriteReg(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t RegAddr, uint16_t v_value)
{
    uint32_t    timecount=0;
    uint16_t    crc = 0;

    modbus.sendbuf[0] = modbus_addr;
    modbus.sendbuf[1] = modbus_coil;
    modbus.sendbuf[2] = RegAddr >> 8;       //寄存器高位
    modbus.sendbuf[3] = RegAddr & 0xFF;     //寄存器低位
    modbus.sendbuf[4] = v_value >> 8;       //值高位
    modbus.sendbuf[5] = v_value &0x00FF;    //值低位

    crc = ModbusCrc16(&modbus.sendbuf[0], 6);   //FLOW_CRC16校验

    modbus.sendbuf[6] = (uint8_t)(crc &0x00FF); //校验值低位
    modbus.sendbuf[7] = crc >>8;

    OpenUartRcvIRQ(ModbusPort);
    UartSendStr(ModbusPort,&modbus.sendbuf[0], 8);
    while(timecount<MODBUS_TIMEOUT)
    {
        timecount++;
        if(modbus.flag==1)
        {
            modbus.flag=0;
            return TRUE;
        }
    }
    CloseUartRcvIRQ(ModbusPort);
    return FALSE;
}


/***************************************************************************
**函数名称： ModbusReadDeviceAddress
**函数功能：读取设备地址，测试MODBUS总线
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    rdaddress:流量计返回的设备地址
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:52:10
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusReadDeviceAddress(uint8_t modbus_addr,uint16_t *rdaddress)
{
    uint8_t readresult;
    UnionU16 rdInt16reg;
    rdInt16reg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult=ModbusReadReg(modbus_addr,312,1);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult=ModbusReadReg(modbus_addr,4909,1);
    }


    if(readresult!= MODBUS_OK)
    {
        return 0;
    }

    rdInt16reg.rdcv[0] =modbus.recbuf[4];
    rdInt16reg.rdcv[1] =modbus.recbuf[3];

    *rdaddress=rdInt16reg.data;

    return MODBUS_OK ;
}

/***************************************************************************
**函数名称： ModbusWriteDeviceAddress
**函数功能：写流量计设备地址
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    rdaddress:流量计的新设备地址
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 16:27:23
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusWriteDeviceAddress(uint8_t modbus_addr,uint16_t rdaddress)
{
    uint8_t readresult;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult=ModbusWriteReg(modbus_addr,6,312,rdaddress);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult=ModbusWriteReg(modbus_addr,6,4909,rdaddress);
    }

    if(readresult!= MODBUS_OK)
    {
        return FALSE;
    }

    return MODBUS_OK ;
}


/***************************************************************************
**函数名称： ModbusReadTotal
**函数功能：读取加气总量
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    total:流量计返回的总累值
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:52:10
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusReadTotal(uint8_t modbus_addr,float *total)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult=ModbusReadReg(modbus_addr,258,2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult=ModbusReadReg(modbus_addr,2609,2);
    }

    if(readresult!= MODBUS_OK)
    {
        return FALSE;
    }

    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data=rdFreg.data+0.005;
    *total=rdFreg.data;
    return MODBUS_OK ;
}

/***************************************************************************
**函数名称： ModbusReadFlow
**函数功能：读流速
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
                                    flow:流量计返回的流速值
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:53:16
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusReadFlow(uint8_t modbus_addr,float *flow)
{
    uint8_t readresult;
    UnionFloat rdFreg;
    rdFreg.data = 0;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        readresult=ModbusReadReg(modbus_addr,246,2);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        readresult=ModbusReadReg(modbus_addr,2006,2);
    }
    if(readresult!= MODBUS_OK)
    {
        return FALSE;
    }
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];

    rdFreg.data=rdFreg.data+0.005;
    *flow=rdFreg.data;
    return MODBUS_OK ;
}


/***************************************************************************
**函数名称： ModbusClearTotal
**函数功能：复位总量寄存器
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:54:31
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusClearTotal(uint8_t modbus_addr)
{
    uint8_t rdresult=FALSE;
    float   cngsum;

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        rdresult=ModbusWriteReg(modbus_addr,5,55,0xFF00);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        rdresult=ModbusWriteReg(modbus_addr,6,2607,1);
    }
    if(rdresult)
    {
        rdresult=FALSE;
        rdresult=ModbusReadTotal(modbus_addr,&cngsum);
        if(cngsum<0.01)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/***************************************************************************
**函数名称： ModbusInitReg
**函数功能：流量计scan buffer寄存器初始化
**入口参数：无
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 15:55:42
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusInitReg(void)
{
    uint8_t n = 0;    
    //uint16_t AUTOSCAN_RM_BUF[]= {246/*流速*/,258/*总量*/,250/*温度*/,248/*密度*/,290/*增益*/}; //罗斯蒙特扫描寄存器
    uint16_t AUTOSCAN_RM_BUF[]= {0/*流速*/,2/*总量*/,1/*温度*/,3/*密度*/,47/*增益*/};
    uint16_t AUTOSCAN_EH_BUF[]= {2007/*流速*/,2610/*总量*/,2017/*温度*/,2013/*密度*/,9503/*增益*/};//E+H扫描寄存器

    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        for(n=0; n<_MODBUS_G_AUTO_BUF_LEN; n++)
        {
            WATCH_DOG();
            if(!ModbusWriteReg(_MODBUS_G_,6, 750+n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation==0)
                {
                    syserr |= 1<<SYSERR_MODBUS_G_AUTO_ERR;//气相流量计通讯错误。系统错误0x04。               
                }                  
                return FALSE;
            }
            MsDelay(200);
        }

        for(n=0; n<_MODBUS_L_AUTO_BUF_LEN; n++)
        {
            WATCH_DOG();
            if(!ModbusWriteReg(_MODBUS_L_,6, 750+n, AUTOSCAN_RM_BUF[n]))
            {
                if(sysparas.simulation==0)
                {
                    syserr |= 1<<SYSERR_MODBUS_L_AUTO_ERR;//液相流量计通讯错误。系统错误0x08。
                }
                return FALSE;
            }
            MsDelay(200);
        }
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        for(n=0; n<_MODBUS_G_AUTO_BUF_LEN; n++)
        {
            WATCH_DOG();
            if(!ModbusWriteReg(_MODBUS_G_,6, 5000+n, AUTOSCAN_EH_BUF[n]))
            {
                if(sysparas.simulation==0)
                {
                    syserr |= 1<<SYSERR_MODBUS_G_AUTO_ERR;//气相流量计通讯错误。系统错误0x04。
                }
                return FALSE;
            }
            MsDelay(200);
        }

        for(n=0; n<_MODBUS_L_AUTO_BUF_LEN; n++)
        {
            WATCH_DOG();
            if(!ModbusWriteReg(_MODBUS_L_,6, 5000+n, AUTOSCAN_EH_BUF[n]))
            {
                if(sysparas.simulation==0)
                {
                    syserr |= 1<<SYSERR_MODBUS_L_AUTO_ERR;//液相流量计通讯错误。系统错误0x08。
                }
                    return FALSE;
            }
            MsDelay(200);
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/***************************************************************************
**函数名称： ModbusDataDisposs
**函数功能：流量计scan buffer数据处理
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016年9月8日09:20:21
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void ModbusDataDisposs()
{
    UnionFloat rdFreg;
    rdFreg.data = 0;
    /*计算流速*/
    rdFreg.rdcv[0] =  modbus.recbuf[4];
    rdFreg.rdcv[1] =  modbus.recbuf[3];
    rdFreg.rdcv[2] =  modbus.recbuf[6];
    rdFreg.rdcv[3] =  modbus.recbuf[5];
    if(modbus.recbuf[0]==_MODBUS_G_)
    {
        realtime.cur_flowrate_g=rdFreg.data;
    }
    else if(modbus.recbuf[0]==_MODBUS_L_)
    {
        realtime.cur_flowrate_l=rdFreg.data;
    }

    /*计算总量*/
    rdFreg.rdcv[0] =  modbus.recbuf[8];
    rdFreg.rdcv[1] =  modbus.recbuf[7];
    rdFreg.rdcv[2] =  modbus.recbuf[10];
    rdFreg.rdcv[3] =  modbus.recbuf[9];
    if(modbus.recbuf[0]==_MODBUS_G_)
    {
        realtime.cur_totalize_g=rdFreg.data;
    }
    else if(modbus.recbuf[0]==_MODBUS_L_)
    {
        realtime.cur_totalize_l=rdFreg.data;
    }

    /*计算温度*/
    rdFreg.rdcv[0] =  modbus.recbuf[12];
    rdFreg.rdcv[1] =  modbus.recbuf[11];
    rdFreg.rdcv[2] =  modbus.recbuf[14];
    rdFreg.rdcv[3] =  modbus.recbuf[13];
    if(modbus.recbuf[0]==_MODBUS_G_)
    {
        realtime.cur_temperature_g=rdFreg.data;
    }
    else if(modbus.recbuf[0]==_MODBUS_L_)
    {
        realtime.cur_temperature_l=rdFreg.data;
    }

    /*计算密度*/
    rdFreg.rdcv[0] =  modbus.recbuf[16];
    rdFreg.rdcv[1] =  modbus.recbuf[15];
    rdFreg.rdcv[2] =  modbus.recbuf[18];
    rdFreg.rdcv[3] =  modbus.recbuf[17];
    if(modbus.recbuf[0]==_MODBUS_G_)
    {
        realtime.cur_density_g=rdFreg.data;
    }
    else if(modbus.recbuf[0]==_MODBUS_L_)
    {
        realtime.cur_density_l=rdFreg.data;
    }

    /*计算增益*/
    rdFreg.rdcv[0] =  modbus.recbuf[20];
    rdFreg.rdcv[1] =  modbus.recbuf[19];
    rdFreg.rdcv[2] =  modbus.recbuf[22];
    rdFreg.rdcv[3] =  modbus.recbuf[21];
    if(modbus.recbuf[0]==_MODBUS_G_)
    {
        if(rdFreg.data<=0.1)
        {
            rdFreg.data=rdFreg.data*1000;
        }
        else if(rdFreg.data>99.9)//增益最大可以为100.45，但是显示屏显示的个数有限
        {
            rdFreg.data=99.9;
        }
        realtime.cur_gain_g=rdFreg.data;
        globalvar.modbus_g_error=0;
        ClrBit(syserr, SYSERR_MODBUS_G_AUTO_ERR);//清除气相流量计通讯错误。
    }
    else if(modbus.recbuf[0]==_MODBUS_L_)
    {
        if(rdFreg.data<=0.1)
        {
            rdFreg.data=rdFreg.data*1000;
        }
        else if(rdFreg.data>99.9)//增益最大可以为100.45，但是显示屏显示的个数有限
        {
            rdFreg.data=99.9;
        }
        realtime.cur_gain_l=rdFreg.data;
        globalvar.modbus_l_error=0;
        ClrBit(syserr, SYSERR_MODBUS_L_AUTO_ERR);//清除液相流量计通讯错误。
    }
}


/***************************************************************************
**函数名称： ModbusReadScanBuffer
**函数功能：读取流量计扫描寄存器中的值
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
**返回值：无
**创建者：杨朝旭
**创建日期：2016-3-31 15:56:30
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void ModbusReadScanBuffer(uint8_t modbus_addr)
{
    if(sysparas.modbustype == MODBUS_TYPE_RM)
    {
        ModbusReadScanReg(modbus_addr,782,10);
    }
    else if(sysparas.modbustype == MODBUS_TYPE_EH)
    {
        ModbusReadScanReg(modbus_addr,5050,10);
    }
}

/***************************************************************************
**函数名称： ModbusComTest
**函数功能：读取流量计的设备地址，用于流量计通讯测试
**入口参数：modbus_addr:流量计设备地址，1-气象，2-液相
**返回值：TRUE:成功，FALSE:失败
**创建者：杨朝旭
**创建日期：2016-3-31 16:07:31
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t ModbusComTest(uint8_t modbus_addr)
{
    uint16_t deviceaddr;
    if(ModbusReadDeviceAddress(modbus_addr,&deviceaddr))
    {
        if(deviceaddr==modbus_addr)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/***************************************************************************
   **函数名称： ModbusRecDataCheck
   **函数功能：流量计接收数据处理
   **入口参数：无
   **返回值：    无
   **创建者：    杨朝旭
   **创建日期：2016-3-31 09:57:21

   **修改者：
   **修改日期：
   **修改内容：
   ***************************************************************************/
void ModbusRecDataCheck(void)
{
    uint32_t    len,i;
    uint16_t    CRC;
    //数据处理
    if(((UART1Buffer[0]==_MODBUS_G_)||(UART1Buffer[0]==_MODBUS_L_))&&
       ((UART1Buffer[1]==0x03)||(UART1Buffer[1]==0x06)||(UART1Buffer[1]==0x05)))   //数据有效性判断
    {
        if((UART1Buffer[1]==0x06)||((UART1Buffer[1]==0x05)))//写寄存器
        {
            len=8;
        }
        else
        {
            len=UART1Buffer[2]+5;   //数据长度
        }
        if(len<40)
        {
            if(UART1Count>=len)
            {
                CloseUartRcvIRQ(ModbusPort);
                CRC=ModbusCrc16((uint8_t *)UART1Buffer,len-2);
                if((UART1Buffer[len-2]==((uint8_t)(CRC &0x00FF)))&&
                   (UART1Buffer[len-1]==((uint8_t)(CRC>>8))))
                {                    
                    for(i=0; i<len; i++)
                    {
                        modbus.recbuf[i]=UART1Buffer[i] ;
                    }
                    modbus.flag = 1;
                }
                UART1Count=0;
                ClearBuf(UART1Buffer,0,40);
            }
        }
        else
        {
            OpenUartRcvIRQ(ModbusPort);
            UART1Count=0;
            ClearBuf(UART1Buffer,0,40);
        }
    }
    else
    {
        OpenUartRcvIRQ(ModbusPort);
        UART1Count=0;
        ClearBuf(UART1Buffer,0,40);
    }
}




/*********************************************************************************************************
** 函数名称：UART1_IRQHandler
** 函数描述：UART1中断处理函数
** 输入参数：无
** 返回值  ：无
*********************************************************************************************************/
void UART1_IRQHandler(void)
{
    uint8_t uiIIRValue, uiLSRValue;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART1->IIR;

    uiIIRValue >>= 1;                     /* 中断标志寄存器值右移1位*/
    uiIIRValue &= 0x07;

    if(uiIIRValue == IIR_RLS)             /* 接收线状态中断 */
    {
        uiLSRValue = LPC_UART1->LSR;
        /*
                         * 溢出中断、奇偶中断、帧中断、UART RBR中包含错误数据和帧间隔中断处理
                         */
        if(uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI))
        {
            UART1Status = uiLSRValue;
            uiDummy = LPC_UART1->RBR;
            return;
        }
        if(uiLSRValue & LSR_RDR)          /* 接收FIFO不为空中断*/
        {
            UART1Buffer[UART1Count] = LPC_UART1->RBR;
            UART1Count++;
            if(UART1Count == BUFSIZE)
            {
                UART1Count = 0;
            }
            if(UART1Count>=7)
            {
                ModbusRecDataCheck();
            }
			if((UART1Buffer[0] != _MODBUS_G_) &&(UART1Buffer[0] != _MODBUS_L_))
			{
				UART1Count = 0;	
			}				
        }
    }
    else if(uiIIRValue == IIR_RDA)       /* 接收数据可用中断*/
    {
        UART1Buffer[UART1Count] = LPC_UART1->RBR;
        UART1Count++;
        if(UART1Count == BUFSIZE)
        {
            UART1Count = 0;
        }
        if(UART1Count>=7)
        {
            ModbusRecDataCheck();
        }
		if((UART1Buffer[0] != _MODBUS_G_) &&(UART1Buffer[0] != _MODBUS_L_))
		{
			UART1Count = 0;	
		}
    }
    else if(uiIIRValue == IIR_CTI)       /* 字符超时中断*/
    {
        UART1Status |= 0x100;
    }
    else if(uiIIRValue == IIR_THRE)      /* 发送保持寄存器空中断*/
    {
        uiLSRValue = LPC_UART1->LSR;
        if(uiLSRValue & LSR_THRE)
        {
            UART1TxEmpty = 1;
        }
        else
        {
            UART1TxEmpty = 0;
        }
    }
}


