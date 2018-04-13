#include "kernel.h"

void ValveTestLow(void);
void ValveTestMid(void);
void ValveTestHigh(void);
void ValveTestAll(void);
void MassmeterTest(void);

/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键0测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest0(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:0");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键1测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest1(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:1");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键2测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest2(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:2");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键3测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest3(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:3");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键4测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest4(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:4");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键5测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest5(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:5");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键6测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest6(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:6");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键7测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest7(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:7");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键8测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest8(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:8");
}
/********************************************************
**函数名称:KeyBoardTest0
**函数功能:按键9测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest9(void)
{
    Fyd12864DispPrintf(3,1,"数字按键:9");
}
/********************************************************
**函数名称:KeyBoardTestPoint
**函数功能:小数点测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestPoint(void)
{
    Fyd12864DispPrintf(3,1,"数字按键: 小数点");
}
/********************************************************
**函数名称:KeyBoardTestQuery
**函数功能:查询按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestQuery(void)
{
    Fyd12864DispPrintf(3,1,"功能按键: 查询");
}
/********************************************************
**函数名称:KeyBoardTestMenu
**函数功能:菜单按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestMenu(void)
{
    Fyd12864DispPrintf(3,1,"功能按键: 菜单");
}
/********************************************************
**函数名称:KeyBoardTestRation
**函数功能:定量按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestRation(void)
{
    Fyd12864DispPrintf(3,1,"功能按键: 定量");
}
/********************************************************
**函数名称:KeyBoardTestClear
**函数功能:清除按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestClear(void)
{
    Fyd12864DispPrintf(3,1,"功能按键：清除");
}
/********************************************************
**函数名称:KeyBoardTestStop
**函数功能:停止按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestStop(void)
{
    Fyd12864DispPrintf(3,1,"功能按键：停止");
}
/********************************************************
**函数名称:KeyBoardTestReplay
**函数功能:回显按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestReplay(void)
{
    Fyd12864DispPrintf(3,1,"功能按键: 回显");
}
/********************************************************
**函数名称:KeyBoardTestEnter
**函数功能:确认按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestEnter(void)
{
    Fyd12864DispPrintf(3,1,"功能按键：确认");
}
/********************************************************
**函数名称:KeyBoardTestStart
**函数功能:加气按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestStart(void)
{
    Fyd12864DispPrintf(3,1,"功能按键：加气");
}
/********************************************************
**函数名称:KeyBoardTestSub
**函数功能:按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTestSub(void)
{
    Fyd12864DispPrintf(1,1,"    按键测试");
    Fyd12864DispPrintf(2,1,"    请按按键");
    Fyd12864DispPrintf(4,1,"【返回】停止测试");
    TaskAdd(KEY_RET,DeviceTestMenu,NULL);
    switch(globalvar.KeyValue)
    {
        case KEY_0:
            KeyBoardTest0();
            break;
        case KEY_1:
            KeyBoardTest1();
            break;
        case KEY_2:
            KeyBoardTest2();
            break;
        case KEY_3:
            KeyBoardTest3();
            break;
        case KEY_4:
            KeyBoardTest4();
            break;
        case KEY_5:
            KeyBoardTest5();
            break;
        case KEY_6:
            KeyBoardTest6();
            break;
        case KEY_7:
            KeyBoardTest7();
            break;
        case KEY_8:
            KeyBoardTest8();
            break;
        case KEY_9:
            KeyBoardTest9();
            break;
        case KEY_PT:
            KeyBoardTestPoint();
            break;
        case KEY_QRY:
            KeyBoardTestQuery();
            break;
        case KEY_SET:
            KeyBoardTestMenu();
            break;
        case KEY_FIX:
            KeyBoardTestRation();
            break;
        case KEY_BACK:
            KeyBoardTestReplay();
            break;
        case KEY_CLR:
            KeyBoardTestClear();
            break;
        case KEY_START:
            KeyBoardTestStart();
            break;
        case KEY_STOP:
            KeyBoardTestStop();
            break;
        case KEY_OK:
            KeyBoardTestEnter();
            break;
        default:
            break;
    }
}
/********************************************************
**函数名称:KeyBoardTest
**函数功能:按键测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void KeyBoardTest(void)
{
    Fyd12864ClearScreen();
    DispUI=KeyBoardTestSub;
}

/********************************************************
**函数名称:ValveTestStop
**函数功能:阀全部关闭
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTestStop(void)
{
    VCTRL_MID_CLOSE();
    UsDelay(10);
    VCTRL_HIGH_CLOSE();
    UsDelay(10);
    VCTRL_LOW_CLOSE();
    UsDelay(10);
    VCTRL_CLOSEKEY();
    UsDelay(10);
    DispUI=DeviceTestMenu;
}

/********************************************************
**函数名称:ValveTestLow
**函数功能:低压阀打开
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTestLow(void)
{
    Fyd12864DispPrintf(2,1,"低  中  高  全");
    Fyd12864DispPrintf(3,1,"●  ○  ○  ○");
    TaskAdd(KEY_2,ValveTestMid,NULL);
    TaskAdd(KEY_3,ValveTestHigh,NULL);
    TaskAdd(KEY_4,ValveTestAll,NULL);
    TaskAdd(KEY_STOP,ValveTestStop,NULL);
    VCTRL_OPENKEY();
    UsDelay(10);
    VCTRL_MID_CLOSE();
    UsDelay(10);
    VCTRL_HIGH_CLOSE();
    UsDelay(10);
    VCTRL_LOW_OPEN();
    UsDelay(10);
}
/********************************************************
**函数名称:ValveTestMid
**函数功能:中压阀打开
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTestMid(void)
{
    Fyd12864DispPrintf(2,1,"低  中  高  全");
    Fyd12864DispPrintf(3,1,"○  ●  ○  ○");
    TaskAdd(KEY_1,ValveTestLow,NULL);
    TaskAdd(KEY_3,ValveTestHigh,NULL);
    TaskAdd(KEY_4,ValveTestAll,NULL);
    TaskAdd(KEY_STOP,ValveTestStop,NULL);
    VCTRL_OPENKEY();
    UsDelay(10);
    VCTRL_LOW_CLOSE();
    UsDelay(10);
    VCTRL_HIGH_CLOSE();
    UsDelay(10);
    VCTRL_MID_OPEN();
    UsDelay(10);
}
/********************************************************
**函数名称:ValveTestHigh
**函数功能:高压阀打开
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTestHigh(void)
{
    Fyd12864DispPrintf(2,1,"低  中  高  全");
    Fyd12864DispPrintf(3,1,"○  ○  ●  ○");
    TaskAdd(KEY_1,ValveTestLow,NULL);
    TaskAdd(KEY_2,ValveTestMid,NULL);
    TaskAdd(KEY_4,ValveTestAll,NULL);
    TaskAdd(KEY_STOP,ValveTestStop,NULL);
    VCTRL_OPENKEY();
    UsDelay(10);
    VCTRL_MID_CLOSE();
    UsDelay(10);
    VCTRL_HIGH_OPEN();
    UsDelay(10);
    VCTRL_LOW_CLOSE();
    UsDelay(10);
}
/********************************************************
**函数名称:ValveTestAll
**函数功能:阀全部打开
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTestAll(void)
{
    Fyd12864DispPrintf(2,1,"低  中  高  全");
    Fyd12864DispPrintf(3,1,"●  ●  ●  ●");
    TaskAdd(KEY_1,ValveTestLow,NULL);
    TaskAdd(KEY_2,ValveTestMid,NULL);
    TaskAdd(KEY_3,ValveTestHigh,NULL);
    TaskAdd(KEY_STOP,ValveTestStop,NULL);
    VCTRL_OPENKEY();
    UsDelay(10);
    VCTRL_LOW_OPEN();
    UsDelay(10);
    VCTRL_MID_OPEN();
    UsDelay(10);
    VCTRL_HIGH_OPEN();
    UsDelay(10);
}

/********************************************************
**函数名称:ValveTest
**函数功能:电磁阀测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void ValveTest(void)
{
    globalvar.devicetest=1;
    Fyd12864DispPrintf(1,1,"  电磁阀测试");
    Fyd12864DispPrintf(2,1,"低  中  高  全");
    Fyd12864DispPrintf(3,1,"○  ○  ○  ○ ");
    Fyd12864DispPrintf(4,1,"【停止】停止测试");
    TaskAdd(KEY_1,ValveTestLow,NULL);
    TaskAdd(KEY_2,ValveTestMid,NULL);
    TaskAdd(KEY_3,ValveTestHigh,NULL);
    TaskAdd(KEY_4,ValveTestAll,NULL);
    TaskAdd(KEY_STOP,ValveTestStop,NULL);
}
/********************************************************
**函数名称:PressureWork
**函数功能:压力测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void PressureWork(void)
{
    uint16_t pzero;
    uint16_t pratio;
    uint32_t curad;
    pzero=sysparas.pzero;
    pratio=sysparas.pcoeff;
    if(pzero==0)
    {
        Fyd12864DispPrintf(2,1,"压力零点错");
        Fyd12864DispPrintf(4,1,"请按【返回】返回");
    }
    else
    {
        Fyd12864DispPrintf(2,1,"零点:%d",pzero);
        curad=PlADCGetValue();
        Fyd12864DispPrintf(3,1,"当前AD值:%d",curad);
        curad=curad;
        pzero=pzero;
        if(curad<=pzero)
        {
            curad=0;
        }
        else
        {
            curad=curad-pzero;
        }
        Fyd12864DispPrintf(4,1,"当前压力:%.02f",(double)curad/pratio);
    }
    TaskAdd(KEY_RET,DeviceTestMenu,NULL);
}
/********************************************************
**函数名称:PressureTest
**函数功能:压力测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void PressureTest(void)
{
    Fyd12864ClearScreen();
    Fyd12864DispPrintf(1,1,"    压力测试");
    DispUI=PressureWork;
}
/********************************************************
**函数名称:GasMassmeterTest
**函数功能:气相流量计测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void GasMassmeterTest(void)
{
    uint8_t  result;
    uint16_t deviceadd;
    Fyd12864DispPrintf(1,1,"  气相流量计");
    Fyd12864ClearLine(2);
    result=ModbusReadDeviceAddress(_MODBUS_G_,&deviceadd);
    if(result==TRUE)
    {
        if(deviceadd==_MODBUS_G_)
        {
            Fyd12864DispPrintf(3,1,"  流量计连接正常");
            ModbusClearTotal(_MODBUS_G_);
        }
        else
        {
            Fyd12864DispPrintf(3,1,"  流量计连接异常");
        }
    }
    else
    {
        Fyd12864DispPrintf(3,1,"  读流量计失败");
    }
    Fyd12864DispPrintf(4,1,"请按【返回】返回");
    TaskAdd(KEY_RET, MassmeterTest,NULL);
}
/********************************************************
**函数名称:LiquidMassmeterTest
**函数功能:液相流量计测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void LiquidMassmeterTest(void)
{
    uint8_t  result;
    uint16_t deviceadd;
    Fyd12864DispPrintf(1,1,"  液相流量计");
    Fyd12864ClearLine(2);
    result=ModbusReadDeviceAddress(_MODBUS_L_,&deviceadd);
    MsDelay(100);
    if(result==TRUE)
    {
        if(deviceadd==_MODBUS_L_)
        {
            Fyd12864DispPrintf(3,1,"  流量计连接正常");
            ModbusClearTotal(_MODBUS_L_);
        }
        else
        {
            Fyd12864DispPrintf(3,1,"  流量计连接异常");
        }
    }
    else
    {
        Fyd12864DispPrintf(3,1,"  读流量计失败");
    }
    Fyd12864DispPrintf(4,1,"请按【返回】返回");
    TaskAdd(KEY_RET, MassmeterTest,NULL);
}
/********************************************************
**函数名称:MassmeterTest
**函数功能:流量计测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void MassmeterTest(void)
{
    Fyd12864DispPrintf(1,1,"  流量计测试");
    Fyd12864DispPrintf(2,1,"1.气相流量计");
    Fyd12864DispPrintf(3,1,"2.液相流量计");
    Fyd12864DispPrintf(4,1,"请按【返回】返回");
    TaskAdd(KEY_RET, DeviceTestMenu,NULL);
    TaskAdd(KEY_1,GasMassmeterTest,NULL);
    TaskAdd(KEY_2,LiquidMassmeterTest,NULL);
}
/********************************************************
**函数名称:PrinterTest
**函数功能:打印机测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void PrinterTest(void)
{
    SELECT_PRINT();
    WHPrinterHead();
    WHPrintf("       打印机测试\r\n");
    WHPrinterFreeRow(1);
    WHPrintf("---------------------------\r\n");
    WHPrinterNormalCmd();
    WHPrintf("   谢谢使用金星产品\r\n");
    WHPrinterFreeRow(6);
    SELECT_CARD();
    DispUI=DeviceTestMenu;
}

//软件重启
void SoftReset(void)
{
    (*(void(*)(void))0x00)();
}

//执行外部eeprom拷到内部
void CopyExToIn(void)
{
    uint32_t    addr=0,num,i;
    uint8_t     tmp1[100],tmp2[100],ret=FALSE;
    num=sizeof(tmp1);
    if(globalvar.displayonce==1)
    {
        while(1)
        {   
            WATCH_DOG();
            if(At24C512ReadNByte(addr, tmp1, num))
            {
                if(EepromInterWriteNByte(addr, tmp1, num))
                {
                    if(EepromInterReadNByte(addr, tmp2, num))
                    {
                        for(i=0;i<num;i++)
                        {
                            if(tmp1[i]!=tmp2[i]) break;
                        }
                        if(i==num) ret=TRUE;
                        else ret=FALSE;
                    }
                    else
                    {
                        ret=FALSE;
                    }                    
                }
                else
                {
                    ret=FALSE;
                }
            }
            else
            {
                ret=FALSE;
            }
            addr+=num;
            if(addr>=4000) break;
            if(ret==FALSE) break;
        }
        globalvar.displayonce=0;
        if(ret==TRUE)   SetSuccess();  
        else    SetFault();
    }    
    TaskMessage(10,DispEepromCopy);
}

//外部拷到内部询问
void DispCopyExToIn(void)
{
    Fyd12864DispPrintf(1,1,"是否拷贝  外->内");
    Fyd12864DispPrintf(2,1,"【确认】  继续");
    Fyd12864DispPrintf(3,1,"【返回】  取消");
    Fyd12864ClearLine(4); 
    TaskAdd(KEY_OK,CopyExToIn,NULL);
    TaskAdd(KEY_RET,DispEepromCopy ,NULL);
}


//执行内部eeprom拷到外部
void CopyInToEx(void)
{
    uint32_t    addr=0,num,i;
    uint8_t     tmp1[100],tmp2[100],ret=FALSE;
    num=sizeof(tmp1);
    if(globalvar.displayonce==1)
    {
        while(1)
        {         
            WATCH_DOG();
            if(EepromInterReadNByte(addr, tmp1, num))
            {
                if(At24C512WriteNByte(addr, tmp1, num))
                {
                    if(At24C512ReadNByte(addr, tmp2, num))
                    {
                        for(i=0;i<num;i++)
                        {
                            if(tmp1[i]!=tmp2[i]) break;
                        }
                        if(i==num) ret=TRUE;
                        else ret=FALSE;
                    }
                    else
                    {
                        ret=FALSE;
                    }                    
                }
                else
                {
                    ret=FALSE;
                }
            }
            else
            {
                ret=FALSE;
            }
            addr+=num;
            if(addr>=4000) break;
            if(ret==FALSE) break;
        }
        globalvar.displayonce=0;
		if(ret==TRUE)   SetSuccess();  
		else    SetFault();
    }    
    TaskMessage(10,DispEepromCopy);
}

//内部拷到外部询问
void DispCopyInToEx(void)
{
    Fyd12864DispPrintf(1,1,"是否拷贝  内->外");
    Fyd12864DispPrintf(2,1,"【确认】  继续");
    Fyd12864DispPrintf(3,1,"【返回】  取消");
    Fyd12864ClearLine(4); 
    TaskAdd(KEY_OK,CopyInToEx,NULL);
    TaskAdd(KEY_RET,DispEepromCopy ,NULL);
}

//eeprom 拷贝
void DispEepromCopy(void)
{
    globalvar.displayonce=1;
    Fyd12864DispPrintf(1,1,"    EEPROM拷贝  ");
    Fyd12864DispPrintf(2,1,"1.将内部拷到外部");
    Fyd12864DispPrintf(3,1,"2.将外部拷到内部");
    Fyd12864ClearLine(4);    
    TaskAdd(KEY_1,DispCopyInToEx,NULL);
    TaskAdd(KEY_2,DispCopyExToIn,NULL);
    TaskAdd(KEY_RET, DeviceTestMenu,NULL);
}

//客显测试
void DispLCDTest(void)
{
    Fyd12864ClearLine(1);
    Fyd12864DispPrintf(2,1,"    客显测试  ");
    Fyd12864DispPrintf(3,1,"停止请按【返回】");
    Fyd12864DispPrintf(4,1,"键不放，直到退出"); 
    OsplcHardwarechk();
    TaskAdd(KEY_RET, DeviceTestMenu,NULL);
}

/********************************************************
**函数名称:DeviceTestMenu
**函数功能:设备硬件测试
**输入参数:无
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-25

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void DeviceTestMenu(void)
{
    Fyd12864DispPrintf(1,1,"  硬件维护测试");
    Fyd12864DispPrintf(2,1,"1.键盘  2.电磁阀");
    Fyd12864DispPrintf(3,1,"3.压力  4.流量计");
    Fyd12864DispPrintf(4,1,"5.客显  6.打印机");
    TaskAdd(KEY_RET, DispFirst,NULL);
    TaskAdd(KEY_1,KeyBoardTest,NULL);
    TaskAdd(KEY_2,ValveTest,NULL);
    TaskAdd(KEY_3,PressureTest,NULL);
    TaskAdd(KEY_4,MassmeterTest,NULL);
    TaskAdd(KEY_5,DispLCDTest,NULL);
    TaskAdd(KEY_6,PrinterTest,NULL);
    TaskAdd(KEY_8,DispEepromCopy,NULL);
    TaskAdd(KEY_COMB_SFIX, SoftReset, NULL);//软件重启
}


