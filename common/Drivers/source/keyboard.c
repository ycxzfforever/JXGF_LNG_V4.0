#include "kernel.h"

Screeninput ScreenInput;

/***************************************************************************
**函数名称： KeyPinInit
**函数功能：键盘IO 口初始化
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-3-25 14:26:32
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void KeyPinInit(void)
{
    LPC_GPIO3->DIR &= 0xFFF00000;     //设置P3.0~P3.19为输入
}

/***************************************************************************
**函数名称： GetKeyValue
**函数功能：获取键盘按键值
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-3-25 14:27:52
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void KeyGetValue(void)
{
    uint32_t rdkey,rdkey1;
    rdkey = (LPC_GPIO3->PIN & 0x000FFFFF);
    if(globalvar.KeyPreValue==rdkey)//按下未释放
    {
        return;
    }    
    if(rdkey!=0x000FFFFF)//键被按下
    {
        MsDelay(5);
        rdkey1=(LPC_GPIO3->PIN & 0x000FFFFF);
        if(rdkey==rdkey1)
        {
            globalvar.KeyValue=rdkey;
            BeepOut(1);
        }
    } 
    globalvar.KeyPreValue=rdkey;
}




/***************************************************************************
**函数名称： NumKeyHandler
**函数功能：数字按键处理
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-3-28 16:51:49
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void NumKeyHandler(void)
{
    if(ScreenInput.inputlen<ScreenInput.inputmaxlen)
    {
        switch(globalvar.KeyValue)
        {
            case KEY_0:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x30;
            break;
            case KEY_1:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x31;
            break;
            case KEY_2:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x32;
            break;
            case KEY_3:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x33;
            break;
            case KEY_4:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x34;
            break;
            case KEY_5:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x35;
            break;
            case KEY_6:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x36;
            break;
            case KEY_7:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x37;
            break;
            case KEY_8:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x38;
            break;
            case KEY_9:
                globalvar.KeyValue=0;
                ScreenInput.array[ScreenInput.inputlen++]=0x39;
            break;
            case KEY_PT:
                if(ScreenInput.pointnum==0)
                {
                    ScreenInput.pointnum=1;
                    globalvar.KeyValue=0;
                    ScreenInput.array[ScreenInput.inputlen++]=0x2E;
                }
            break;
            case KEY_CLR:
                globalvar.KeyValue=0;
                if(ScreenInput.inputlen>0)  ScreenInput.inputlen--;
                if(ScreenInput.array[ScreenInput.inputlen]==0x2E) ScreenInput.pointnum=0;//0x2E小数点。
    			ScreenInput.array[ScreenInput.inputlen]=0x20;                            //0x20空。
            break;
        }        
    }
    else
    {
        switch(globalvar.KeyValue)
        {
//            case KEY_RET:
            case KEY_CLR:
                globalvar.KeyValue=0;                
                if(ScreenInput.inputlen>0)  ScreenInput.inputlen--;
                if(ScreenInput.array[ScreenInput.inputlen]==0x2E) ScreenInput.pointnum=0;
    			ScreenInput.array[ScreenInput.inputlen]=0x20;
            break;
        }     
    }
}

