/***************************************************************************
**文本：DispControl.c
**说明：用于CNG或者LNG中的"控制参数"菜单显示
**创建者 ：杨朝旭
**创建日期：2016-4-21 17:17:20

**修改者：
**修改日期：
**修改内容：
***************************************************************************/

#include "kernel.h"
//控制参数显示CNG和LNG需要分开编写此内容

//预冷超时时间设置
void DispSetPrecoolingTimeout(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=4;
    tmp_sysparas.precooltimeout=atol((char *)ScreenInput.array);    
    globalvar.paraindex=206;
    DispSet(0,sysparas.precooltimeout,"预冷超时时间");
    TaskAdd(KEY_RET, DispSetControl1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl1);
}

//预冷临界温度设置
void DispSetPrecoolingTemp(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.precooltemp=atol((char *)ScreenInput.array);  
    globalvar.paraindex=203;
    DispSet(7,sysparas.precooltemp,"预冷临界温度设置");
    TaskAdd(KEY_RET, DispSetControl1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl1);
}

//预冷临界增益设置
void DispSetPrecoolingGain(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.precoolgain=atol((char *)ScreenInput.array); 
    globalvar.paraindex=204;
    DispSet(0,sysparas.precoolgain,"预冷临界增益设置");
    TaskAdd(KEY_RET, DispSetControl1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl1);
}

//预冷临界密度设置
void DispSetPrecoolingDensty(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*10000);
    tmp_sysparas.precooldensty=atol(buf);   
    globalvar.paraindex=205;
    DispSet(4,sysparas.precooldensty,"预冷临界密度设置");
    TaskAdd(KEY_RET, DispSetControl1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl1);
}

//LNG设置控制参数1
void DispSetControl1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.预冷超时时间");
    Fyd12864DispPrintf(2,1,"2.预冷临界温度");
    Fyd12864DispPrintf(3,1,"3.预冷临界增益");
    Fyd12864DispPrintf(4,1,"4.预冷临界密度");
    TaskAdd(KEY_1, DispSetPrecoolingTimeout,NULL);
    TaskAdd(KEY_2, DispSetPrecoolingTemp,NULL);
    TaskAdd(KEY_3, DispSetPrecoolingGain,NULL);
    TaskAdd(KEY_4, DispSetPrecoolingDensty,NULL);
    TaskAdd(KEY_DOWN, DispSetControl2,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

//停机延时时间设置
void DispSetDelayStopTime(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.delaystoptime=atol((char *)ScreenInput.array);   
    globalvar.paraindex=207;
    DispSet(0,sysparas.delaystoptime,"停机延时时间");
    TaskAdd(KEY_RET, DispSetControl2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl2);
}

//超压停机设置
void DispSetPressureStop(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=4;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.pressurestop=atol(buf);   
    globalvar.paraindex=212;
    DispSet(2,sysparas.pressurestop,"超压停机设置");
    TaskAdd(KEY_RET, DispSetControl2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl2);
}

//低停机流速设置
void DispSetLowRateStop(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.lowratestop=atol((char *)ScreenInput.array);  
    globalvar.paraindex=208;
    DispSet(0,sysparas.lowratestop,"低停机流速设置");
    TaskAdd(KEY_RET, DispSetControl2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl2);
}

//高停机流速设置
void DispSetHighRateStop(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=4;
    tmp_sysparas.highratestop=atol((char *)ScreenInput.array);   
    globalvar.paraindex=209;
    DispSet(0,sysparas.highratestop,"高停机流速设置");
    TaskAdd(KEY_RET, DispSetControl2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl2);
}

//LNG设置控制参数2
void DispSetControl2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.停机延时");
    Fyd12864DispPrintf(2,1,"2.超压停机");
    Fyd12864DispPrintf(3,1,"3.低流速停机");
    Fyd12864DispPrintf(4,1,"4.高流速停机");
    TaskAdd(KEY_1, DispSetDelayStopTime,NULL);
    TaskAdd(KEY_2, DispSetPressureStop,NULL);
    TaskAdd(KEY_3, DispSetLowRateStop,NULL);
    TaskAdd(KEY_4, DispSetHighRateStop,NULL);
    TaskAdd(KEY_UP, DispSetControl1,NULL);
    TaskAdd(KEY_DOWN, DispSetControl3,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

//回气密度停机设置
void DispSetBackDenstyStop(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*10000);
    tmp_sysparas.backdenstystop=atol(buf);   
    globalvar.paraindex=210;
    DispSet(4,sysparas.backdenstystop,"回气密度停机");
    TaskAdd(KEY_RET, DispSetControl3,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl3);
}

//回气增益停机设置
void DispSetBackGainStop(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.backgainstop=atol((char *)ScreenInput.array);    
    globalvar.paraindex=211;
    DispSet(0,sysparas.backgainstop,"回气增益停机");
    TaskAdd(KEY_RET, DispSetControl3,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl3);
}

//是否回气计量
void DispSetBackGasCalc(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.backgascalc=tmp;
        globalvar.paraindex=201;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.backgascalc,"是否回气计量");
    Fyd12864DispPrintf(2,1,"0.不计量");
    Fyd12864DispPrintf(3,1,"1.计量");
    TaskAdd(KEY_RET, DispSetControl3,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl3);
}

//预冷方式选择
void DispSetPrecoolMode(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.precoolmode=tmp;
        globalvar.paraindex=202;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.precoolmode,"是否回气计量");
    Fyd12864DispPrintf(2,1,"0.小循环预冷");
    Fyd12864DispPrintf(3,1,"1.大循环预冷");
    TaskAdd(KEY_RET, DispSetControl3,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl3);
}

//LNG设置控制参数3
void DispSetControl3(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.回气密度停机");
    Fyd12864DispPrintf(2,1,"2.回气增益停机");
    Fyd12864DispPrintf(3,1,"3.是否回气计量");
    Fyd12864DispPrintf(4,1,"4.预冷方式选择");
    TaskAdd(KEY_1, DispSetBackDenstyStop,NULL);
    TaskAdd(KEY_2, DispSetBackGainStop,NULL);
    TaskAdd(KEY_3, DispSetBackGasCalc,NULL);
    TaskAdd(KEY_4, DispSetPrecoolMode,NULL);
    TaskAdd(KEY_UP, DispSetControl2,NULL);
    TaskAdd(KEY_DOWN, DispSetControl4,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

//加液后停泵延时时间
void DispSetStopPumpTime(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.stoppumptime=atol((char *)ScreenInput.array);    
    globalvar.paraindex=213;
    DispSet(0,sysparas.stoppumptime,"加液后停泵延时");
    TaskAdd(KEY_RET, DispSetControl4,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl4);
}

//预冷完成后起泵超时时间
void DispSetPreStartPumpTime(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.precoolfinishedtime=atol((char *)ScreenInput.array);    
    globalvar.paraindex=214;
    DispSet(0,sysparas.precoolfinishedtime,"重新预冷延时");
    TaskAdd(KEY_RET, DispSetControl4,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetControl4);
}


//LNG设置控制参数4
void DispSetControl4(void)
{
    InputInit();    
    Fyd12864DispPrintf(1,1,"1.停泵延时%d",sysparas.stoppumptime);
    Fyd12864DispPrintf(2,1,"2.重新预冷延时");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1, DispSetStopPumpTime,NULL);
    TaskAdd(KEY_2, DispSetPreStartPumpTime,NULL);
    TaskAdd(KEY_UP, DispSetControl3,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

//打印LNG控制参数
void PrintContInfo(void)
{
    SELECT_PRINT();

    WHPrinterHead();
    WHPrintf("         LNG控制参数\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintf("预冷超时时间:%d\r\n",sysparas.precooltimeout);  
    WHPrintf("预冷临界温度:-%d\r\n",sysparas.precooltemp); 
    WHPrintf("预冷临界增益:%d\r\n",sysparas.precoolgain);  
    WHPrintf("预冷临界密度:%d.%04d\r\n",sysparas.precooldensty/10000,sysparas.precooldensty%10000); 

    WHPrintf("停机延时:%d\r\n",sysparas.delaystoptime);  
    WHPrintf("超压停机:%d.%02d\r\n",sysparas.pressurestop/100,sysparas.pressurestop%100); 
    WHPrintf("低流速停机:%d\r\n",sysparas.lowratestop);  
    WHPrintf("高流速停机:%d\r\n",sysparas.highratestop); 

    WHPrintf("回气密度停机:%d.%04d\r\n",sysparas.backdenstystop/10000,sysparas.backdenstystop%10000);  
    WHPrintf("回气增益停机:%d\r\n",sysparas.backgainstop); 
    WHPrintf("是否回气计量:%d\r\n",sysparas.backgascalc);  
    WHPrintf("预冷方式选择:%d\r\n",sysparas.precoolmode); 

    WHPrintf("停泵延时:%d\r\n",sysparas.stoppumptime); 
    WHPrintf("重新预冷延时:%d\r\n",sysparas.precoolfinishedtime); 
    WHPrinterFreeRow(1);
    WHPrintf("     <请妥善保存此凭条>\r\n");//脚注 
    WHPrinterFreeRow(6); 
    SELECT_CARD();
}

void PrinterContInfo(void)//打印LNG控制参数
{
    PrintContInfo();
    DispUI=DispCtrlParas1;
}


//控制参数1查询
void DispCtrlParas1(void)
{
    Fyd12864DispPrintf(1,1,"预冷超时时间%d",sysparas.precooltimeout);
    Fyd12864DispPrintf(2,1,"预冷临界温度-%d",sysparas.precooltemp);
    Fyd12864DispPrintf(3,1,"预冷临界增益%d",sysparas.precoolgain);
    Fyd12864DispPrintf(4,1,"预冷临界密度%d.%04d",sysparas.precooldensty/10000,sysparas.precooldensty%10000);
    TaskAdd(KEY_DOWN, DispCtrlParas2,NULL);
    TaskAdd(KEY_RET, DispQuery1,NULL);
    TaskAdd(KEY_OK, PrinterContInfo,NULL);
}

//控制参数2查询
void DispCtrlParas2(void)
{
    Fyd12864DispPrintf(1,1,"停机延时%d",sysparas.delaystoptime);
    Fyd12864DispPrintf(2,1,"超压停机%d.%02d",sysparas.pressurestop/100,sysparas.pressurestop%100);
    Fyd12864DispPrintf(3,1,"低流速停机%d",sysparas.lowratestop);
    Fyd12864DispPrintf(4,1,"高流速停机%d",sysparas.highratestop);
    TaskAdd(KEY_UP, DispCtrlParas1,NULL);
    TaskAdd(KEY_DOWN, DispCtrlParas3,NULL);
    TaskAdd(KEY_RET, DispQuery1,NULL);
    TaskAdd(KEY_OK, PrinterContInfo,NULL);
}

//控制参数3查询
void DispCtrlParas3(void)
{
    Fyd12864DispPrintf(1,1,"回气密度停机%d.%04d",sysparas.backdenstystop/10000,sysparas.backdenstystop%10000);
    Fyd12864DispPrintf(2,1,"回气增益停机%d",sysparas.backgainstop);
    Fyd12864DispPrintf(3,1,"是否回气计量%d",sysparas.backgascalc);
    Fyd12864DispPrintf(4,1,"预冷方式选择%d",sysparas.precoolmode);
    TaskAdd(KEY_UP, DispCtrlParas2,NULL);
    TaskAdd(KEY_DOWN, DispCtrlParas4,NULL);
    TaskAdd(KEY_RET, DispQuery1,NULL);
    TaskAdd(KEY_OK, PrinterContInfo,NULL);
}

//控制参数4查询
void DispCtrlParas4(void)
{
    Fyd12864DispPrintf(1,1,"停泵延时%d",sysparas.stoppumptime);
    Fyd12864ClearLine(2);
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_UP, DispCtrlParas3,NULL);
    TaskAdd(KEY_RET, DispQuery1,NULL);
    TaskAdd(KEY_OK, PrinterContInfo,NULL);
}


//恢复出厂设置时，各个参数重置
void FactoryReset(void)
{
    uint8_t i;
    tmp_sysparas.head=0xAA;            //头
    tmp_sysparas.writeE2num=0;         //写eeprom次数
    
    tmp_sysparas.gunnum=1;             //枪号
    tmp_sysparas.transnum=0;           //交易流水号     
    tmp_sysparas.uptransnum=0;         //已传交易流水号     
    
    tmp_sysparas.totvolume=0;          //枪总气量    2位小数
    tmp_sysparas.totmoney=0;           //枪总金额    2位小数

    for(i=0;i<20;i++) tmp_sysparas.price[i]=450;      //20个单价，单价0为默认单价，根据单价等级选择相应单价
    
    //班组信息
    tmp_sysparas.shiftnum=1;           //班号
    tmp_sysparas.shiftmaxnum=3;        //最大班号
    tmp_sysparas.shiftransnum=0;       //班流水号     
    tmp_sysparas.shiftuptransnum=0;    //已传班流水号     
    tmp_sysparas.shiftfueltimes=0;     //班加气次数
    tmp_sysparas.shiftstartvol=0;      //班开始枪累    2位小数
    tmp_sysparas.shiftstartmon=0;      //班开始金额    2位小数
    tmp_sysparas.shiftotvol=0;         //班累计气量    2位小数
    tmp_sysparas.shiftotmon=0;         //班累计金额    2位小数

    tmp_sysparas.shiftstarttime[0]=time.year;    //班开始时间    BCD
    tmp_sysparas.shiftstarttime[1]=time.month;
    tmp_sysparas.shiftstarttime[2]=time.day;
    tmp_sysparas.shiftstarttime[3]=time.hour;    
    tmp_sysparas.shiftstarttime[4]=time.minute;
    tmp_sysparas.shiftstarttime[5]=time.second;

    for(i=0;i<6;i++) tmp_sysparas.shiftendtime[i]=0;    //班结束时间    BCD
    
    //通用信息
    for(i=0;i<2;i++) tmp_sysparas.shengcode[i]=0;       //省编码
    for(i=0;i<4;i++) tmp_sysparas.stationcode[i]=0;     //站编码
    for(i=0;i<2;i++) tmp_sysparas.citycode[i]=0;        //地市代码
    for(i=0;i<4;i++) tmp_sysparas.parentcode[i]=0;      //上级单位代码

    //卡信息
    tmp_sysparas.cardmaxyue=100000000;   //卡余额上限    2位小数 默认100万
    tmp_sysparas.cardminyue=100;         //卡余额下限    2位小数 
    tmp_sysparas.cardpsamnum=1;          //psam卡卡座选择
    tmp_sysparas.cardmaxday=90;          //卡未使用最大间隔天数
    tmp_sysparas.cardcheck=0;            //是否检验钢瓶
    tmp_sysparas.usecard=0;              //默认不用卡

    //波特率设置
    tmp_sysparas.modbusbaud=19200;       //流量计波特率
    tmp_sysparas.pcbaud=38400;           //后台波特率

    tmp_sysparas.fuelmode=0;             //加气模式
    tmp_sysparas.printmode=0;            //小票打印方式

    //版本编号设置
    tmp_sysparas.pricever=0;             //单价版本号
    tmp_sysparas.blackver=0;             //黑名单版本号
    tmp_sysparas.whitever=0;             //白名单版本号
    tmp_sysparas.addbkver=0;             //新增黑名单版本号
    tmp_sysparas.delbkver=0;             //新删黑名单版本号
    tmp_sysparas.comminfover=0;          //通用信息版本号

    //密码设置
    tmp_sysparas.menupwd=0;              //菜单进入密码
    tmp_sysparas.qcpwd=0;                //质检密码
    tmp_sysparas.testpwd=0;              //测试密码，如按键、电磁阀等测试
    tmp_sysparas.resetpwd=0;             //恢复出厂密码
    for(i=0;i<6;i++) tmp_sysparas.pwdboard[i]=0;        //加密板数据
    tmp_sysparas.usepwdboard=0;          //0-不使用加密板，1-使用加密板
    
    
    tmp_sysparas.modbustype=1;           //流量计类型
    tmp_sysparas.unit=0;                 //计量单位
    tmp_sysparas.qcmode=0;               //质检模式设置

    tmp_sysparas.backgascalc=0;          //0-回气不计量，1-回气计量
    tmp_sysparas.precoolmode=0;          //预冷方式，0-小循环，1-大循环
    tmp_sysparas.precooltemp=120;        //预冷临界温度
    tmp_sysparas.precoolgain=10;         //预冷临界增益
    tmp_sysparas.precooldensty=3000;     //预冷临界密度，4位小数，0-1.0000
    tmp_sysparas.precooltimeout=900;     //预冷超时时间
    tmp_sysparas.delaystoptime=30;       //刚开始加液时,不判断停机条件的延时时间
    tmp_sysparas.lowratestop=5;          //低流速停机
    tmp_sysparas.highratestop=80;        //高流速停机
    tmp_sysparas.backdenstystop=4000;    //回气密度停机，4位小数，0-1.0000
    tmp_sysparas.backgainstop=30;        //回气增益停机
    tmp_sysparas.pressurestop=180;       //停机压力,超压停机，两位小数
    tmp_sysparas.stoppumptime=10;        //加液后延时停泵
    tmp_sysparas.precoolfinishedtime=10; //重新预冷延时

    tmp_sysparas.densty=6804;            //密度              4位小数
    tmp_sysparas.coeff=10000;            //修正系数  4位小数
    tmp_sysparas.pzero=820;              //压力零点  
    tmp_sysparas.pcoeff=105;             //压力系数  2位小数       
    
    tmp_sysparas.modnum=0;               //修改记录流水号     
    tmp_sysparas.upmodnum=0;             //已传修改记录流水号           
    tmp_sysparas.simulation=0;           //默认正常加气。
    tmp_sysparas.intnumflag=0;           //默认不取整
    tmp_sysparas.devicetype=0;           //设备类型
    tmp_sysparas.lcdtype=1;              //LNG默认大屏

}

