#include "kernel.h"

/***************************************************************************
**函数名称： PumpSwitch
**函数功能：根据加气机的状态打开和关闭相应的阀门和PLC联动信号
**入口参数：state:加气机的状态
**返回值：无
**创建者：杨朝旭
**创建日期：2016-4-1 12:35:28
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void PumpSwitch(unsigned char state)
{
    switch(state)
    {
        case StaClose://停机
            PUMPCTRL_HIGH_CLOSE();  //泵高关
            PUMPCTRL_LOW_CLOSE();   //泵低关
            VCTRL_CLOSEKEY();       //主阀关
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
            break;
        case StaOtherFuel://对方加液
            PUMPCTRL_LOW_OPEN();    //泵低开
            PUMPCTRL_HIGH_CLOSE();  //泵高关
            VCTRL_OPENKEY();        //主阀开
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
            break;
        case StaSPrecool://小循环预冷
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();
            VCTRL_HIGH_CLOSE();
            break;
        case StaBPrecool://大循环预冷
            PUMPCTRL_HIGH_OPEN();
            PUMPCTRL_LOW_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;
        case StaFuel://加液
            PUMPCTRL_LOW_OPEN();    //PLC
            PUMPCTRL_HIGH_OPEN();   //PLC
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();
            VCTRL_HIGH_OPEN();
            break;
        case StaIdle://待机
        default:
            PUMPCTRL_LOW_OPEN();
            PUMPCTRL_HIGH_CLOSE();
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();
            VCTRL_HIGH_CLOSE();
            break;
    }
}

/***************************************************************************
**函数名称： GetIntNum
**函数功能：加气过程中取整
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-12-2 11:10:10
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void GetIntNum()
{
    if(globalvar.KeyValue==KEY_PT)//按小数点取整按定金额处理
    {
        globalvar.fix=2;          //定金额。
        globalvar.fixmoney=((fuelrecordinfo.money+100)/100)*100;        
    }
}


/***************************************************************************
**函数名称： JudgeStopConditions
**函数功能：判断停机条件
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-5-10 13:06:28
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void JudgeStopConditions(void)
{
    double money_tmp;
    double price_tmp;
    double volume_tmp;
	if(globalvar.KeyValue==KEY_STOP)
	{
		return;
	}
    if(globalvar.timeout > (uint32_t)(sysparas.delaystoptime))//停机延时过后才判断停机条件
    {
        if(realtime.cur_flowrate_l<(float)sysparas.lowratestop) //低流速停机
        {
            globalvar.stoptimes++;
            stopreason=8;
        }
        if(realtime.cur_flowrate_l>(float)sysparas.highratestop) //高流速停机
        {
            globalvar.stoptimes++;
            stopreason=9;
        }
        if(realtime.cur_pressure>((float)sysparas.pressurestop)) //超压停机??
        {
            globalvar.stoptimes++;
            stopreason=12;
        }
        if(realtime.cur_density_g>((float)sysparas.backdenstystop/10000.0)) //回气密度停机
        {
            globalvar.stoptimes++;
            stopreason=10;
        }
        if(realtime.cur_gain_g>((float)sysparas.backgainstop)) //回气增益停机
        {
            globalvar.stoptimes++;
            stopreason=11;
        }        
    }
    
    if(globalvar.stoptimes>100)
    {
        globalvar.KeyValue=KEY_STOP;
    }
    else
    {
        stopreason=0;
    }
    
    if(m1card.cardtype==2)//记账卡。
    {
        if((fuelrecordinfo.money+m1card.cardbalance)>=sysparas.cardmaxyue)//余额上限。
        {
            money_tmp=(double)(fuelrecordinfo.money);
            price_tmp=(double)(realtime.price);
            fuelrecordinfo.volume=(uint32_t)(money_tmp/price_tmp*100+0.5);
            stopreason=14;
            globalvar.KeyValue=KEY_STOP;
        }
    }
    if(sysparas.intnumflag==1)//判断是否需要取整
    {
        GetIntNum();        
    }    
    if(globalvar.fix==1)//定气量。
    {
        if(fuelrecordinfo.volume+10>=(globalvar.fixnum))//提前0.1kg关阀
        {
            fuelrecordinfo.volume=globalvar.fixnum;
            volume_tmp = (double)fuelrecordinfo.volume/100.0;
            fuelrecordinfo.money=(uint32_t)(volume_tmp*realtime.price+0.5);
            stopreason=6;
            globalvar.KeyValue=KEY_STOP;
        }
    }
    if(globalvar.fix==2)//定金额。
    {
        if(fuelrecordinfo.money+5>=(globalvar.fixmoney))//提前0.5元关阀
        {
            fuelrecordinfo.money=(globalvar.fixmoney);
            money_tmp=(double)(globalvar.fixmoney);
            price_tmp=(double)(realtime.price);
            fuelrecordinfo.volume=(uint32_t)(money_tmp*100/price_tmp+0.5);
            stopreason=7;
            globalvar.KeyValue=KEY_STOP;
        }
    }

    if((m1card.existflag==0)&&(m1card.cardtype!=0))//有卡加气时，中途拔卡
    {
        globalvar.KeyValue=KEY_STOP;
        stopreason=13;
    }    
}

/***************************************************************************
**函数名称： StartPrecooling
**函数功能：加气前的预冷
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-5-10 10:08:51
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void StartPrecooling(void)
{
    globalvar.startstate=0xff;            //清电子标签命令。    
    ReadModbusAndPressure();
    if(sysparas.simulation)
    {
        realtime.cur_flowrate_l+=10;      //流速
        realtime.cur_temperature_l-=10;   //预冷温度
        realtime.cur_gain_l+=1;          //预冷增益
        if(realtime.cur_gain_l>=sysparas.precoolgain)
        {
			realtime.cur_gain_l--;
		}
        realtime.cur_density_l+=1;       //预冷密度
        
    }
    
    if((DispNextUI==DispFirst)&&(sysparas.precoolmode==1))//只有当按预冷键的时候才会存在大循环，其余情况下均进行小循环 
    {
        PumpSwitch(StaBPrecool);
        globalvar.workstate = StaBPrecool;
    }
    else 
    {
        PumpSwitch(StaSPrecool);
        globalvar.workstate = StaSPrecool;
    }

    if(realtime.cur_totalize_l > 20000)
    {
        timer1off();
        MsDelay(300);
        WATCH_DOG();
        ModbusClearTotal(_MODBUS_L_);
        timer1on();
        memset(&realtime.lcng,0,sizeof(RealTimeData));//清总累后必须重新读取新数据
    }
    if(realtime.cur_totalize_g > 20000)
    {
        timer1off();
        MsDelay(300);
        WATCH_DOG();
        ModbusClearTotal(_MODBUS_G_);
        timer1on();
        memset(&realtime.lcng,0,sizeof(RealTimeData));//清总累后必须重新读取新数据
    }
    
    Fyd12864DispPrintf(1,1,"正在预冷%d",globalvar.timeout);
    Fyd12864DispPrintf(2,1,"流速:%.02f",realtime.cur_flowrate_l);
    Fyd12864DispPrintf(3,1,"温度:%.02f",realtime.cur_temperature_l);
    Fyd12864DispPrintf(4,1,"增益:%.02f",realtime.cur_gain_l);
    
    OsplcShow(0,0,0,0,0);
    if(globalvar.timeout>sysparas.precooltimeout)//预冷超时
    {
        timer1off();
        if(globalvar.otherstate==1)//对方未加液
        {
            PumpSwitch(StaIdle);
        }
        else
        {
            PumpSwitch(StaOtherFuel);
        } 
        DispUI=DispFirst;
    }
    else//达到预冷要求。
    {
        if(((realtime.cur_temperature_l<((float)(0-sysparas.precooltemp)))//预冷温度
           &&(realtime.cur_density_l>((float)(sysparas.precooldensty)/10000.0))//预冷密度
           &&(realtime.cur_gain_l<((float)(sysparas.precoolgain))))//预冷增益
           ||(globalvar.precoolfinished==1))//之前已经完成预冷，用于处理直接按加液键
        {
            globalvar.precoolfinished=1;//预冷完成进来
            if((m1card.existflag==1)&&(m1card.checkresult==0)&&(DispNextUI!=DispFirst))
            {
                DispUI=GreyLock;
            }
            else if(DispNextUI!=NULL)
            {
                DispUI=DispNextUI;
                if(globalvar.otherstate==1)//对方未加液
                {
                    PumpSwitch(StaSPrecool);
                }
                else
                {
                    PumpSwitch(StaOtherFuel);
                }           
            }
            else
            {
                DispUI=DispFirst;
            }
        }
    }
    globalvar.stoppumptime=sysparas.stoppumptime+2;//预冷后也停泵延时。
    if(globalvar.stoptimes>100)//流量计异常和压力传感器异常停机
    {
        globalvar.KeyValue=KEY_STOP;        
    }
    TaskAdd(KEY_STOP,DispFirst, NULL);
}

/***************************************************************************
**函数名称： StartFueling
**函数功能：开始加气,预冷完成后开始加液前
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-4-26 16:17:52
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void StartFueling(void)
{
    uint32_t  availmoney;     //IC卡可用加气金额 
    memset(&fuelrecordinfo,0,sizeof(FuelRecordInfo));                         //清流水结构体。
    PumpSwitch(StaFuel);
    globalvar.fuelstep=1;                                                    //加气步骤1。
    globalvar.workstate=StaFuel;                                              //加气状态。
    globalvar.timeout=0;
    globalvar.stoptimes=0;
    stopreason=0;
    globalvar.displayonce=1;    
    fuelrecordinfo.classnum=sysparas.shiftnum;                                //班号。
    memcpy(&fuelrecordinfo.employeenum[0],&shiftrecordinfo.cardstaffnum[0],8);//员工号。
    memcpy(&fuelrecordinfo.usernum[0],&m1card.cardnum[0],8);              //用户卡号。
    memcpy(&fuelrecordinfo.carnum[0],&m1card.carnum[0],9);                //车牌号。
    fuelrecordinfo.cardtype=m1card.cardtype;                              //卡类型。

    memcpy(&fuelrecordinfo.stationnum[0],&sysparas.stationcode[0],4);         //站编码。
    fuelrecordinfo.starttime[0]=0x20;
    memcpy(&fuelrecordinfo.starttime[1], &time.year, 6);                      //加气开始时间。
    if(m1card.existflag==1)
        fuelrecordinfo.price=sysparas.price[m1card.pricelevel];
    else
        fuelrecordinfo.price=sysparas.price[0];                               //交易价格。
    realtime.price=fuelrecordinfo.price;
	if(realtime.price==0)
	{
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
		Fyd12864DispPrintf(2,1,"  单价错误");
		TaskMessage(20, DispFirst);
		return;
	}
    fuelrecordinfo.beforepressure=realtime.cur_pressure;                      //加气前压力。
    fuelrecordinfo.beforebalance=m1card.cardbalance;                      //加气前余额。
    fuelrecordinfo.afterbalance=m1card.cardbalance;                       //加气后余额。
    fuelrecordinfo.allrechargemoney=m1card.sumchargemoney;                //累计充值金额
    fuelrecordinfo.allrechargetimes=m1card.sumchargetimes;                //累计充值次数。
    fuelrecordinfo.allconsumptiontimes=m1card.sumtimes+1;                 //累计消费次数。

    globalvar.starttotalizeL=realtime.cur_totalize_l;    
    globalvar.pretotalL=realtime.cur_totalize_l;
    if(sysparas.backgascalc==1)                                               //回气计量
    {
        globalvar.starttotalizeG=realtime.cur_totalize_g;
        globalvar.pretotalG=realtime.cur_totalize_g;
    }

    fuelrecordinfo.devicetype=sysparas.devicetype;                                                 //气品代码，1字节。
    if(m1card.cardtype==1)//储蓄卡做定额处理 ycx20161201
    {
        availmoney=m1card.cardbalance-sysparas.cardminyue; 
        if(globalvar.fix==1)//之前按定量加气
        {
            if(((globalvar.fixnum*fuelrecordinfo.price)/100)>availmoney)
            {
                globalvar.fix=2;
                globalvar.fixmoney=availmoney;
            }
        }
        else if(globalvar.fix==2)
        {
            if(globalvar.fixmoney>availmoney)
            {
                globalvar.fixmoney=availmoney; 
            }
        } 
        else
        {
            globalvar.fix=2;
            globalvar.fixmoney=availmoney; 
        }
    }
    realtime.powerdown=0;    
	DispUI=DispStartFueling;
}

//加液显示
double	 lngvolume=0;//进液量
double   cngvolume=0;//回气量
void DispStartFueling(void)
{
    globalvar.workstate = StaFuel;
    globalvar.fuelstep=2;//加气步骤2。    
    if(sysparas.simulation)
    {
        realtime.cur_totalize_l+=0.11;
        realtime.cur_flowrate_l++;
        realtime.cur_pressure+=1;
        if(realtime.cur_pressure>150)
        {
            realtime.cur_pressure=150;
        }
        if(realtime.cur_flowrate_l>400)
        {
            realtime.cur_flowrate_l=400;
        }
    }
    ReadModbusAndPressure();
    if(fabs(realtime.cur_totalize_l-globalvar.pretotalL)>2)//液相数据异常
    {
        globalvar.KeyValue=KEY_STOP;
        stopreason=3;
    }
    else
    {
        lngvolume=realtime.cur_totalize_l-globalvar.starttotalizeL;
        globalvar.pretotalL=realtime.cur_totalize_l;//将此次实时总累赋值，用于下次判断瞬时数据
    }
    if(sysparas.backgascalc==1)//回气计量
    {
        if(globalvar.starttotalizeG<0.1)//防止第一次没有读到气象流量计的起始量
        {
            globalvar.starttotalizeG=realtime.cur_totalize_g;
			globalvar.pretotalG=realtime.cur_totalize_g;
        }
        else
        {
            if(fabs(realtime.cur_totalize_g-globalvar.pretotalG)>1)//气象数据异常
            {
                globalvar.KeyValue=KEY_STOP;
                stopreason=3;
            }
            else
            {
                cngvolume=realtime.cur_totalize_g-globalvar.starttotalizeG;
                globalvar.pretotalG=realtime.cur_totalize_g;//将此次实时总累赋值，用于下次判断瞬时数据
            }
        }
    }
    else
    {
        cngvolume=0;
    }        
    if(sysparas.unit==1)//按标方计量
    {
        lngvolume = lngvolume/((double)sysparas.densty/10000.0);
        cngvolume = cngvolume/((double)sysparas.densty/10000.0);
    }
    lngvolume = (lngvolume-cngvolume)*((double)sysparas.coeff/10000.0);//当前交易气量。
    fuelrecordinfo.backgas=(uint32_t)(cngvolume*100.0);//当前回气量
    fuelrecordinfo.volume=(uint32_t)(lngvolume*100.0); //当前交易气量。

    lngvolume = (double)fuelrecordinfo.volume/100.0;
    fuelrecordinfo.money=(uint32_t)(lngvolume*realtime.price+0.5);//当前交易金额。

    JudgeStopConditions();//判断停机条件。
    realtime.volume=fuelrecordinfo.volume;
    realtime.money=fuelrecordinfo.money;

    Fyd12864DispPrintf(1,1,"状态: 正在加液");
    Fyd12864DispPrintf(2,1,"气量:%ld.%02d",fuelrecordinfo.volume/100,fuelrecordinfo.volume%100);
    Fyd12864DispPrintf(3,1,"金额:%ld.%02d",fuelrecordinfo.money/100,fuelrecordinfo.money%100);
    Fyd12864DispPrintf(4,1,"V:%.02f P:%ld.%02d",realtime.cur_flowrate_l,realtime.cur_pressure/100,realtime.cur_pressure%100);    

    TaskAdd(KEY_STOP,StopFueling, NULL);
    realtime.powerdown=0;          //未掉电标志。
    SaveFm25l04();                 //写铁电。
    OsplcShow(fuelrecordinfo.volume,fuelrecordinfo.money,realtime.price,0,0);
    if(PowerCheckIn!=0)            //判断是否掉电。
    {
        MsDelay(20);
        if(PowerCheckIn!=0)
        {
            realtime.powerdown=1;      //已掉电。
            SaveFm25l04();             //写铁电。
            PowerDown();
        }
    }
}

/***************************************************************************
**函数名称： StopFueling
**函数功能：停止加气
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-4-26 16:17:52
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void StopFueling()
{
    timer1off();    
    globalvar.fuelstep=3;//加气步骤3。
    if(globalvar.displayonce==1)
    {
        if(globalvar.otherstate==1)//对方未加液
        {
            PumpSwitch(StaSPrecool);
        }
        else
        {
            PumpSwitch(StaOtherFuel);
        }
        globalvar.displayonce=0;
        lngvolume=0;                                    //加液结束后气量变量清零。
        globalvar.stoppumptime=sysparas.stoppumptime+2; //加液后停泵延时。                
        sysparas.transnum++;
        sysparas.totvolume += fuelrecordinfo.volume;//枪总气量。
        sysparas.totmoney += fuelrecordinfo.money;  //枪总金额。
        sysparas.shiftotvol+= fuelrecordinfo.volume;//班累计气量。
        sysparas.shiftotmon+= fuelrecordinfo.money; //班累计金额。
        sysparas.shiftfueltimes++;                  //班加气次数。
        fuelrecordinfo.transnum=sysparas.transnum;  //交易流水号。
        fuelrecordinfo.allconsumptionnum=m1card.sumvolme+fuelrecordinfo.volume;//累计消费气量。
        if(m1card.cardtype!=0)
        {
            fuelrecordinfo.transactiontype=0xFF;                        //交易类型。
            fuelrecordinfo.reallymoney=0;                               //本次实际扣款金额。
            fuelrecordinfo.allconsumptionmoney=m1card.summoney;         //累计消费金额。
        }
        else
        {
            fuelrecordinfo.transactiontype=0;                           //交易类型。
            fuelrecordinfo.reallymoney=fuelrecordinfo.money;            //本次实际扣款金额。
            fuelrecordinfo.allconsumptionmoney=fuelrecordinfo.money;    //累计消费金额。
        }
        fuelrecordinfo.endtime[0]=0x20;
        memcpy(&fuelrecordinfo.endtime[1], &time.year, 6);              //加气结束时间。

        fuelrecordinfo.allgunnum=sysparas.totvolume;                    //枪气量累计。
        fuelrecordinfo.allgunmoney=sysparas.totmoney;                   //枪金额累计。
        fuelrecordinfo.afterpressure=realtime.cur_pressure;             //加气后压力。
        fuelrecordinfo.curnum=sysparas.coeff;                           //当量。
        fuelrecordinfo.measuretype=sysparas.unit;                       //计量单位，0-标方，1-Kg。
        fuelrecordinfo.density=realtime.cur_density_l;                  //密度。
        fuelrecordinfo.classid=sysparas.shiftransnum;                   //班流水号。
        realtime.stopreason=stopreason;
        fuelrecordinfo.stopreason=realtime.stopreason;                  //停机原因
        fuelrecordinfo.fuelmode=sysparas.fuelmode;
        fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
        OsplcShow(fuelrecordinfo.volume,fuelrecordinfo.money,realtime.price,0,1);
        SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
        m1card.debittimes=10;                                           //最多申请10次扣款。
    }

    if((m1card.existflag==1)&&(PowerCheckIn==0))  //判断是否用卡加气且未掉电。
    {
        globalvar.workstate=StaPay;
        Fyd12864DispPrintf(1,1,"正在等待扣款..");
        if(m1card.debittimes==0)
        {
            globalvar.displayonce=1;
            DispUI=FuelEndDisplay;
        }
    }
    else if(PowerCheckIn==0)//未掉电。
    {
        globalvar.displayonce=1;
        DispUI=FuelEndDisplay;
    }

    realtime.powerdown=0;          //未掉电标志。
    SaveFm25l04();                 //写铁电。
    if(PowerCheckIn!=0)            //判断是否掉电。
    {
        MsDelay(20);
        if(PowerCheckIn!=0)
        {
            realtime.powerdown=1;  //已掉电。
            SaveFm25l04();         //写铁电。
            PowerDown();
        }
    }
}


/***************************************************************************
**函数名称： FuelEndDisplay
**函数功能：加液完毕后存储流水和打印
**入口参数：无
**返回值：无
**创建者：杨朝旭
**创建日期：2016-4-26 16:17:52
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void FuelEndDisplay(void)
{
    globalvar.workstate=StaIdle;   
    Fyd12864DispPrintf(1,1,"加液结束!");
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        if(SaveFuelRecord(0))
        {
            Fyd12864DispPrintf(4,1,"存储流水成功");
        }
        else
        {
            Fyd12864DispPrintf(4,1,"存储流水失败");
        }
        if(sysparas.printmode==1)  //判断是否自动打印一次。
        {
            Printid();
        }
        memset(&realtime.lcng,0,sizeof(RealTimeData));  //ycx加气后清零实时数据。
        Fm25l04Write(0,(uint8_t *)(&realtime.lcng),sizeof(RealTimeData));
        globalvar.MsgFlag=0;
        globalvar.MsgTime=0;//防止多次调用函数TaskMessage而不起作用
    }    
    Fyd12864DispPrintf(3,1,"%s",stopreasonbuf[stopreason]);
    TaskAdd(KEY_RET, DispFirst, NULL);
    TaskMessage(30, DispFirst);// 3s后返回主界面
}

void DispCardError(void)
{
    Fyd12864DispPrintf(2,1,"读卡错误");
    if(mt318reader.recbuf[6] == 0x33)
    {
        if(mt318reader.recbuf[4]==KEYA)
            Fyd12864DispPrintf(3,1,"A 密验证失败");
        else if(mt318reader.recbuf[4]==KEYB)
            Fyd12864DispPrintf(3,1,"B 密验证失败");
    }
    if(m1card.existflag==0)//拔卡直接返回主界面
        DispUI=DispFirst;
    else
        TaskMessage(50, DispFirst); // 5s后返回主界面
}

/***************************************************************************
**函数名称：RemoveStringChar
**函数功能：删除字符串前面的特殊字符
**入口参数：
                                buf:需要删除特殊字符的字符串数组
                                ch:需要删除的特殊字符
                                len:字符串数组的长度
**返 回 值：无
**创 建 者：杨朝旭
**创建日期：2016-11-30
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void RemoveStringChar(uint8_t *buf,uint8_t ch,uint16_t len)
{
    uint8_t *t1,*t2;
    uint16_t i=0,j=0;
    if((len==0)||(buf==NULL)) return;
    if(buf[0]!=ch)  return; //第一个就不满足，直接返回
    t1=buf;                 //用于变化处理buf
    t2=buf;                 //用于添加结束符
    while(*buf==ch)         //找出与需要删除的字符不一样的第一个字符
    {
        buf++;
        i++;
    }
    len -= i;               //计算buf后面有用数据长度
    j=len;                  //保存此长度，用以添加字符串结束符
    while(len--)
    {
        *t1++ = *buf++;
    }
    t2[j]='\0';
}

/***************************************************************************
**函数名称：GetCardData
**函数功能：如果密钥比对成功，就会得到卡内部信息。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void GetCardData(void)
{
    uint8_t tmp=0;                              //读块出错次数。
    #if HardIcTest
    	uint8_t keystr1[16]={0xCB,0xC4,0xB4,0xA8,0xBD,0xF0,0xD0,0xC7,0xC7,0xE5,0xBD,0xE0,0xC4,0xDC,0xD4,0xB4};
    #endif
	uint8_t keystr[16];
    uint8_t keykk[8];                           //ThreeDES加密后数据。
    uint8_t icnum[8];                           //两次卡序列号。
    Fyd12864ClearLine(1);
    Fyd12864ClearLine(4);
    Fyd12864DispPrintf(2,1,"    读卡中");
    Fyd12864DispPrintf(3,1,"    请稍后...");
    m1card.existflag=1;
    m1card.checkresult=0xFF;                     //卡验证结果标志。
    globalvar.KeyValue=0;                        //按任何按键，皆无效。
    BufReverse2(&mt318reader.recbuf[6],4);       //读取密钥。
    memcpy(&icnum[0], &mt318reader.recbuf[6],4);
    memcpy(&icnum[4], &mt318reader.recbuf[6],4); //得到连续序列号。

    At24C512ReadNByte(LOCKVER_ADDR, keystr, 16);
	#if HardIcTest
    	memcpy(&keystr[0], &keystr1[0],16);
	#endif
    ThreeDES(&keystr[0],&icnum[0],&keykk[0],1);  // 3DES加密运算得到A密和B密。
    memcpy(&m1card.keyA[0], &keykk[0],6);                            //A密。
    memcpy(&m1card.keyB[0], &keykk[2],6);                            //B密。
    memcpy(&m1card.serialnumber[0], &mt318reader.recbuf[6],4);

    if(!Mt318ReadM1Card(KEYB,&m1card.keyB[0],1, &m1card.cardnum[0]))    //读块1。
    {
        tmp++;
    }

    if(tmp==0)
    {
        if(!Mt318ReadM1Card(KEYB,&m1card.keyB[0],2, &m1card.stationcode[0]))    //读块2。
        {
            tmp++;
        }
    }

    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],4, &m1card.password[0]))   //读块4。
        {
            RemoveStringChar(&m1card.customername[0],0,13);
        }
        else tmp++;
    }

    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],5,(uint8_t *)&m1card.balancecrc))   //读块5。
        {
            BufReverse2((uint8_t *)&m1card.balancecrc+8,4);
            BufReverse2((uint8_t *)&m1card.balancecrc+12,4);
        }
        else tmp++;
    }

    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],8,(uint8_t *)&m1card.sumtimes))    //读块8。
        {
            StrReverse((uint8_t *)&m1card.sumtimes,3,4);
        }
        else tmp++;
    }

    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],9,(uint8_t *)&m1card.sumchargetimes))    //读块9。
        {
            StrReverse((uint8_t *)&m1card.sumchargetimes,2,4);
        }
        else tmp++;
    }
    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],10,&m1card.cardstate))  //读块10。
        {
            StrReverse(&m1card.cardstate+2,1,4);
        }
        else tmp++;
    }

    if(tmp==0)
    {
        if(Mt318ReadM1Card(KEYB,&m1card.keyB[0],12,&m1card.cylindersdate[0]))  //读块12。
        {
            RemoveStringChar(&m1card.carnum[0],0,9);
        }
        else tmp++;
    }

    if(tmp==0)                                                 //读卡成功。
    {
        DispUI=IcCardPasswd;
    }
    else                                                       //读卡失败。
    {
        DispUI=DispCardError;
    }
}

/***************************************************************************
**函数名称：IcCardPasswd
**函数功能：读卡成功后，判断卡密码是否为0，不为0则需要输入密码。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void IcCardPasswd(void)
{
    uint8_t str[7];
    uint32_t stationcode1,stationcode2;
    memset(str,0,7);
    str[0]=((m1card.password[0]>>4)+0x30);   //将压缩BCD码转化为字符。
    str[1]=((m1card.password[0]&0x0f)+0x30);
    str[2]=((m1card.password[1]>>4)+0x30);
    str[3]=((m1card.password[1]&0x0f)+0x30);
    str[4]=((m1card.password[2]>>4)+0x30);
    str[5]=((m1card.password[2]&0x0f)+0x30);
    m1card.key=atol((char *)str);               //将字符转化为数字。
    stationcode1=BcdbufToHex(m1card.stationcode, 4);
    stationcode2=BcdbufToHex(sysparas.stationcode, 4);
    if((stationcode2==0)||(stationcode1==stationcode2))//站编号等于0时不判断
    {
       if(sysparas.cardcheck==0)                    //不需要检验钢瓶日期。
        {
            if(m1card.key==0)                       //卡密码为0，直接跳过。
            {
                DispUI=ApplyVerif;
            }
            else                                     //卡密码不为0，则输入密码。
            {
                DispUI=InputIccardPasswd;
            }
        }
        else if(sysparas.cardcheck==1)               //需要检验钢瓶日期。
        {
            //判断钢瓶到期日期。
            if((m1card.cylindersdate[1]>time.year)
               ||((m1card.cylindersdate[1]==time.year)&&(m1card.cylindersdate[2]>time.month))
               ||((m1card.cylindersdate[1]==time.year)&&(m1card.cylindersdate[2]==time.month)&&(m1card.cylindersdate[3]>time.day)))
            {
                if(m1card.key==0)                   //卡密码为0，直接跳过。
                {
                    DispUI=ApplyVerif;
                }
                else                                 //卡密码不为0，则输入密码。
                {
                    DispUI=InputIccardPasswd;
                }
            }
            else
            {
                DispUI=DisplayGP;
            }
        } 
    }
    else
    {
        Fyd12864DispPrintf(2,1,"非本站卡");
        Fyd12864DispPrintf(3,1,"请退卡");
        if(m1card.existflag==0)
        {
            DispUI=DispFirst;
        }
    }
    
}

/***************************************************************************
**函数名称：DisplayGP
**函数功能：显示钢瓶已经过期。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void DisplayGP(void)
{
    Fyd12864ClearLine(4);
    Fyd12864DispPrintf(2,1,"钢瓶已经过期");
    Fyd12864DispPrintf(3,1,"%02X%02X-%02X-%02X",m1card.cylindersdate[0],m1card.cylindersdate[1],
                       m1card.cylindersdate[2],m1card.cylindersdate[3]);
    TaskAdd(KEY_RET,DispFirst, NULL);
    if(Mt318GetCardSerialNum()!=true)
    {
        DispUI=DispFirst;
    }
}

/***************************************************************************
**函数名称：InputIccardPasswd
**函数功能：输入卡密码。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void InputIccardPasswd(void)
{
    DispInputPwd("请输入卡密码",ComparePasswd);
    if(m1card.existflag==0)
    {
        DispUI=DispFirst;
    }
}

/***************************************************************************
**函数名称：ComparePasswd
**函数功能：判断卡密码正误。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void ComparePasswd(void)
{
    if(globalvar.U32Temp!=m1card.key)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,InputIccardPasswd ,NULL);
        TaskAdd(KEY_RET,InputIccardPasswd ,NULL);
    }
    else
    {
        DispUI=ApplyVerif;
    }
}

//电子标签加气时，未收到标签信息而按了启动键
void DispScanRFID(void)
{
    Fyd12864ClearLine(1);
    Fyd12864DispPrintf(2,1,"请扫描电子标签");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    if(globalvar.startstate!=0xff)//电子标签不正常或没有标签命令。
	{							
		DispUI=BQresults;
	}
    TaskAdd(KEY_RET,DispFirst,NULL);    
}


/***************************************************************************
**函数名称：DispCardInfo
**函数功能：读卡成功后，显示卡信息，判断卡用途。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void DispCardInfo(void)
{
#if 0
    WriteICcard();
#endif
    globalvar.timeout=0;
    if((sysparas.offonwork==1)&&(sysparas.yesnolock==0))  //判断加气机是否处在锁定或者下班状态。
    {
        Fyd12864DispPrintf(1,1,"卡号: %02X%02X%02X%02X",m1card.cardnum[4],m1card.cardnum[5],m1card.cardnum[6],m1card.cardnum[7]);
        Fyd12864DispPrintf(2,1,"余额: %ld.%02d",m1card.cardbalance/100,m1card.cardbalance%100);
        Fyd12864DispPrintf(3,1,"姓名: %s",m1card.customername);
        Fyd12864DispPrintf(4,1,"车牌: %s",m1card.carnum);
        TaskAdd(KEY_FIX, FixGas,NULL);    //定量加气。
        if(sysparas.fuelmode==0)          //手动加气。
        {
            TaskAdd(KEY_START, StartPrecooling, NULL);
            TaskAdd(KEY_BACK,StartPrecooling,DispFirst);
        }
        else if(sysparas.fuelmode==2)     //标签启动。
        {
            TaskAdd(KEY_START, DispScanRFID,NULL);
            TaskAdd(KEY_BACK,StartPrecooling,DispFirst);
            if(globalvar.startstate!=0xff)//电子标签不正常或没有标签命令。
            {                
                DispUI=BQresults;
            }
        }
    }
    else
    {
        if(sysparas.yesnolock==1)
        {
            Fyd12864DispPrintf(2,1,"加气机已锁"); 
            Fyd12864ClearLine(3);
        }
        else if(sysparas.offonwork==0)
        {
            Fyd12864DispPrintf(2,1,"    下班中...");            
            if(m1card.shiftflag==1)//是白名单且具有换班功能
                Fyd12864DispPrintf(3,1,"按【停止+7】上班"); 
            else
                Fyd12864ClearLine(3);
        }
    }
    if((m1card.Shiftfunction == 1)&&(m1card.shiftflag==0))//员工卡。
    {
        if(SearchBlackList(4,m1card.cardnum)==1)          //判断是否为白名单卡。
        {   
            m1card.shiftflag=1;            
        }
        else
        {
            Fyd12864DispPrintf(4,1,"非白名单卡");
        }
    }
    if(m1card.shiftflag==1)
    {
        TaskAdd(KEY_SET, DispInputMenuPwd,NULL);      //设置。
        TaskAdd(KEY_COMB_7KEY,OnWork,NULL);           //停止+7上班
        TaskAdd(KEY_COMB_8KEY,OffWork, NULL);         //停止+8下班
    }
    if(m1card.existflag==0)//卡被拔出
    {
        DispUI=DispFirst;
    }
}


/***************************************************************************
**函数名称：ApplyVerif
**函数功能：向后台查询此卡是否可用。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void ApplyVerif(void)
{
    globalvar.startstate=0xff;
    if(m1card.cardstate == 2)
    {
        m1card.greycard=1;
        Fyd12864DispPrintf(2,1,"申请解灰中...");
    }
    else
    {
        if(m1card.cardtype==2)//记账卡。
        {
            if(m1card.cardbalance<sysparas.cardmaxyue)//判断余额是否高于上限。
            {
                m1card.checkflag=1;
                DispUI=VerificationCard;
                globalvar.displayonce=1;
            }
            else
            {
                Fyd12864ClearLine(1);
                Fyd12864DispPrintf(2,1,"余额: %ld.%02d",m1card.cardbalance/100,m1card.cardbalance%100);
                Fyd12864DispPrintf(3,1,"余额超限，请付账");
                Fyd12864ClearLine(4);
            }
        }
        else if(m1card.cardtype==1)//储蓄卡。
        {
            if(m1card.cardbalance>sysparas.cardminyue)//判断余额是否低于余额下限。
            {
                m1card.checkflag=1;
                DispUI=VerificationCard;
                globalvar.displayonce=1;
            }
            else
            {
                Fyd12864ClearLine(1);      
                Fyd12864DispPrintf(2,1,"余额: %ld.%02d",m1card.cardbalance/100,m1card.cardbalance%100);
                Fyd12864DispPrintf(3,1,"余额不足，请充值");
                Fyd12864ClearLine(4);
            }
        }
    }
    if(m1card.existflag==0)//卡被拔出
    {
        DispUI=DispFirst;
    }
}


/***************************************************************************
**函数名称：VerificationCard
**函数功能：等待后台返回查询此卡是否可用的结果。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void VerificationCard(void)
{
    if(globalvar.displayonce==1)
    {
        Fyd12864ClearLine(1);
        Fyd12864ClearLine(4);
        globalvar.displayonce=0;
    }
    #if HardIcTest
		m1card.checkresult=0;
	#endif
    if(m1card.checkresult==0)  //正常卡。
    {
        DispUI=DispCardInfo;
        globalvar.displayonce=1;
    }
    else if(m1card.checkresult==0xFF)
    {

        Fyd12864DispPrintf(2,1,"正在等待验证");
        Fyd12864DispPrintf(3,1,"请稍后...");
    }
    else
    {
        Fyd12864DispPrintf(2,1,"验证失败");
        if(m1card.checkresult==1)
        {
            Fyd12864DispPrintf(3,1,"请求卡号不存在");
        }
        else if(m1card.checkresult==2)
        {
            Fyd12864DispPrintf(3,1,"黑名单号");
        }
        else if(m1card.checkresult==3)
        {
            Fyd12864DispPrintf(3,1,"余额异常");
        }
        else if(m1card.checkresult==4)
        {
            Fyd12864DispPrintf(3,1,"最后交易时间异常");
        }
    }
    if(m1card.existflag==0) DispUI=DispFirst; //卡被拔出
}

/***************************************************************************
**函数名称：GreyLock
**函数功能：如果此卡可用，则将卡置灰。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void GreyLock(void)
{
    uint8_t tmpbuffer[16];
	if(sysparas.price[m1card.pricelevel]==0)
	{
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
		Fyd12864DispPrintf(2,1,"  单价错误");
		TaskMessage(20, DispFirst);
		return;
	}
    m1card.cardstate=2;//置灰。
    m1card.gunnum=sysparas.gunnum;
    m1card.transnum=sysparas.transnum+1;
    memcpy(&m1card.laststationcode[0],&sysparas.stationcode[0],4);
    memcpy(&tmpbuffer[0],&m1card.cardstate,16);
    StrReverse(&tmpbuffer[2],1,4);

    if(Mt318WriteM1Card(KEYB,&m1card.keyB[0],10,&tmpbuffer[0]))//写卡
    {
        DispUI=StartFueling;
    }
    else
    {
        Fyd12864DispPrintf(2,1,"置灰失败！");
        TaskMessage(10, DispFirst);// 1s后返回主界面
    }
}

/***************************************************************************
**函数名称：CheckBlackorWhite
**函数功能：查询此卡是否为黑白卡。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void CheckBlackorWhite(void)
{
    Fyd12864ClearLine(1);
    Fyd12864ClearLine(4);
    Fyd12864DispPrintf(2,1,"2.查询是否黑卡");
    Fyd12864DispPrintf(3,1,"3.查询是否白卡");
    TaskAdd(KEY_2, GetBlack, NULL);
    TaskAdd(KEY_3, GetWhite, NULL);
    globalvar.displayonce=1;
}

/***************************************************************************
**函数名称：GetBlack
**函数功能：查询此卡是否为黑卡。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void GetBlack(void)
{
    if(globalvar.displayonce==1)
    {
        Fyd12864ClearLine(2);
        Fyd12864ClearLine(3);
        Fyd12864DispPrintf(2,1,"正在等待查询..");
        globalvar.displayonce=0;
        UpLoad56H(0);
    }

    if(m1card.blackorwhite==1)
    {
        m1card.blackorwhite=0;
        Fyd12864DispPrintf(2,1,"此卡不是黑卡");
    }
    else if(m1card.blackorwhite==2)
    {
        m1card.blackorwhite=0;
        Fyd12864DispPrintf(2,1,"此卡正是黑卡");
    }
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**函数名称：GetWhite
**函数功能：查询此卡是否为白卡。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void GetWhite(void)
{
    if(globalvar.displayonce==1)
    {
        Fyd12864ClearLine(2);
        Fyd12864ClearLine(3);
        globalvar.displayonce=0;
        Fyd12864DispPrintf(2,1,"正在等待查询..");
        UpLoad56H(1);
    }
    if(m1card.blackorwhite==1)
    {
        m1card.blackorwhite=0;
        Fyd12864DispPrintf(2,1,"此卡不是白卡");
    }
    else if(m1card.blackorwhite==2)
    {
        m1card.blackorwhite=0;
        Fyd12864DispPrintf(2,1,"此卡正是白卡");
    }
    TaskAdd(KEY_RET, DispFirst, NULL);
}

/***************************************************************************
**函数名称：PowerDown
**函数功能：掉电显示函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void PowerDown(void)
{
    PumpSwitch(StaClose);                                               //停机。
    Fyd12864DispPrintf(1,1,"系统已经掉电...");
    Fyd12864DispPrintf(4,1,"数据已保存");
    DispUI=PowerDown;
    TaskAdd(KEY_RET, SoftReset,NULL);
}

/***************************************************************************
**函数名称：SaveFm25l04
**函数功能：掉电保存数据函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void SaveFm25l04(void)
{
    realtime.lcng = sysparas.devicetype;                                                  //气品代码。
    if(globalvar.fuelstep==3)
    {
        realtime.transnum=sysparas.transnum;                            //当前流水号。
    }
    else
    {
        realtime.transnum=sysparas.transnum+1;                          //当前流水号。
    }
    realtime.price=fuelrecordinfo.price;                                //单价。
    realtime.cardtype=m1card.cardtype;                                  //卡类型。
    realtime.shiftnum=sysparas.shiftnum;                                //班号。
    memcpy(&realtime.employeenum[0],&shiftrecordinfo.cardstaffnum[0],8);//员工号。
    memcpy(&realtime.cardnum[0],&m1card.cardnum[0],8);                  //用户卡号。
    memcpy(&realtime.carnum[0],&m1card.carnum[0],9);                    //车牌号。
    memcpy(&realtime.starttime[0], &fuelrecordinfo.starttime[0], 7);    //加气开始时间。
    realtime.endtime[0]=0x20;
    memcpy(&realtime.endtime[1], &time.year, 6);                        //加气结束时间。
    realtime.beforepressure=fuelrecordinfo.beforepressure;              //加气前压力。
    realtime.afterpressure=realtime.cur_pressure;                       //加气后压力。
    realtime.beforebalance=fuelrecordinfo.beforebalance;                //加气前余额。
    realtime.afterbalance=fuelrecordinfo.afterbalance;                  //加气后余额。
    realtime.allrechargetimes=fuelrecordinfo.allrechargetimes;          //累计充值次数。
    realtime.allrechargemoney=fuelrecordinfo.allrechargemoney;          //累计充值金额。
    realtime.allconsumptiontimes=fuelrecordinfo.allconsumptiontimes;    //累计消费次数。
    realtime.backgas=fuelrecordinfo.backgas;                            //回气量
    realtime.volume=fuelrecordinfo.volume;                              //气量。
    realtime.money=fuelrecordinfo.money;                                //金额。
    realtime.allconsumptionnum=fuelrecordinfo.volume+m1card.sumvolme;   //累计消费气量。
    realtime.allconsumptionmoney=m1card.summoney;  //累计消费金额。由于后台解灰计算时已经累了一次，所以加气机下面不需要再累

    if(m1card.existflag==1)
    {
        if((globalvar.fuelstep==1)||(globalvar.fuelstep==2))         //如果在加气步骤1或者2。
        {
            realtime.reallymoney=0;                                  //实扣金额。
            realtime.transactiontype=0xFF;                           //交易类型。
        }
        else if(globalvar.fuelstep==3)                               //如果在加气步骤3.
        {
            realtime.reallymoney=fuelrecordinfo.reallymoney;           //实扣金额。
            realtime.transactiontype=fuelrecordinfo.transactiontype;   //交易类型。
        }
    }
    else
    {
        realtime.reallymoney=fuelrecordinfo.money;                      //实扣金额。
        realtime.transactiontype=0x0;                                   //交易类型。
    }
    realtime.curnum=sysparas.coeff;                                     //当量。
    realtime.measuretype=sysparas.unit;                                 //计量单位。
    realtime.stopreason=5;                                              //停机原因。
    realtime.jhid=0;                                                    //解灰相关流水号。
    realtime.classid=sysparas.shiftransnum;                             //班流水号。
    realtime.fuelmode=sysparas.fuelmode;
    Fm25l04Write(0,(uint8_t *)(&realtime.lcng),sizeof(RealTimeData));   //存储铁电。

}

/***************************************************************************
**函数名称：PowerDownRealDate
**函数功能：掉电数据恢复函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void PowerDownRealDate(void)
{
    if(realtime.workstatus==StaFuel)
    {
        sysparas.transnum=realtime.transnum;                            //流水号。
        sysparas.totvolume+=realtime.volume;                            //枪累气量。
        sysparas.totmoney+=realtime.money;                              //枪累金额。
        sysparas.shiftotvol+=realtime.volume;                           //班累气量。
        sysparas.shiftotmon+=realtime.money;                            //班累气量。
        sysparas.shiftfueltimes++;                                      //班加气次数。
        SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
    }

    fuelrecordinfo.transnum=realtime.transnum;                          //流水号。
    fuelrecordinfo.classnum=sysparas.shiftnum;                          //班号。;
    memcpy(&fuelrecordinfo.employeenum[0],&realtime.employeenum[0],8);  //员工号。
    fuelrecordinfo.transactiontype=realtime.transactiontype;            //逃卡交易。
    memcpy(&fuelrecordinfo.usernum[0],&realtime.cardnum[0],8);          //用户卡号。
    fuelrecordinfo.cardtype=realtime.cardtype;                          //卡类型。
    memcpy(&fuelrecordinfo.carnum[0],&realtime.carnum[0],9);            //车牌号。
    memcpy(&fuelrecordinfo.stationnum[0],&sysparas.stationcode[0],4);   //站编码。
    memcpy(&fuelrecordinfo.starttime[0],&realtime.starttime[0],7);      //加气开始时间。
    memcpy(&fuelrecordinfo.endtime[0],&realtime.endtime[0],7);          //加气结束时间。

    fuelrecordinfo.price=realtime.price;                                //单价。
    fuelrecordinfo.volume=realtime.volume;                              //气量。
    fuelrecordinfo.money=realtime.money;                                //金额。
    fuelrecordinfo.backgas=realtime.backgas;                            //回气量
    fuelrecordinfo.reallymoney=realtime.reallymoney;                    //实际扣款金额。
    fuelrecordinfo.beforebalance=realtime.beforebalance;                //加气前余额。
    fuelrecordinfo.afterbalance=realtime.afterbalance;                  //加气后余额。
    fuelrecordinfo.allrechargetimes=realtime.allrechargetimes;          //累计充值次数。


    fuelrecordinfo.allrechargemoney=realtime.allrechargemoney;          //累计充值金额。

    fuelrecordinfo.allconsumptiontimes=realtime.allconsumptiontimes;    //累计消费次数。
    fuelrecordinfo.allconsumptionnum=realtime.allconsumptionnum;        //累计消费气量。
    fuelrecordinfo.allconsumptionmoney=realtime.allconsumptionmoney;    //累计消费金额。
    fuelrecordinfo.allgunnum=sysparas.totvolume;                        //枪累气量。
    fuelrecordinfo.allgunmoney=sysparas.totmoney;                       //枪累金额。
    fuelrecordinfo.beforepressure=realtime.beforepressure;              //加气前压力。
    fuelrecordinfo.afterpressure=realtime.afterpressure;                //加气后压力。
    fuelrecordinfo.density=(uint32_t)(realtime.cur_density_g*10000+0.5);//密度。
    fuelrecordinfo.curnum=realtime.curnum;                              //当量。
    fuelrecordinfo.devicetype=realtime.lcng;                               //气品代码。
    fuelrecordinfo.measuretype=realtime.measuretype;                    //计量单位。
    fuelrecordinfo.stopreason=realtime.stopreason;                      //停机原因。
    fuelrecordinfo.jhid=realtime.jhid;                                  //解灰相关流水号。
    fuelrecordinfo.classid=realtime.classid;                            //班流水号。
    fuelrecordinfo.fuelmode=realtime.fuelmode;
    fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
   
    MsDelay(100);
    if(SaveFuelRecord(0))
    {
        Fyd12864DispPrintf(4,1,"存储流水成功");
    }
    else
    {
        Fyd12864DispPrintf(4,1,"存储流水失败");
    }
}

/***************************************************************************
**函数名称：FixGas
**函数功能：定量选择函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void FixGas(void)
{
    Fyd12864ClearLine(1);
    Fyd12864ClearLine(4);
    Fyd12864DispPrintf(2,1,"  1.定气量");
    Fyd12864DispPrintf(3,1,"  2.定金额");

    TaskAdd(KEY_RET, DispFirst, NULL);
    TaskAdd(KEY_1, FixNum, NULL);
    TaskAdd(KEY_2, FixMoney, NULL);
    if((m1card.cardtype!=0)&&(m1card.existflag==0))//卡被拔出
    {
        DispUI=DispFirst;
    }
    globalvar.fixnum=0;
    globalvar.fixmoney=0;
    globalvar.displayonce=1;
    InputInit();
}

/***************************************************************************
**函数名称：FixNum
**函数功能：定气量函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void FixNum(void)
{
    char buf[16]= {0};
    ScreenInput.inputmaxlen=5;
    NumKeyHandler();
    globalvar.fix=1;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    globalvar.fixnum=atol(buf);
    Fyd12864ClearLine(3);
    Fyd12864DispPrintf(2,1,"定气量:%ld.%02d",globalvar.fixnum/100,globalvar.fixnum%100);
    TaskAdd(KEY_RET, DispFirst, NULL);
    if((m1card.existflag==1)&&(m1card.checkresult==0))
    {
        if(sysparas.fuelmode==0)                        //手动加气。
        {
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
        if(sysparas.fuelmode==2)                        //标签启动。
        {
            TaskAdd(KEY_OK, WaitBQresults, NULL);
        }

    }
    else
    {
        if((sysparas.fuelmode==0)&&(sysparas.usecard==0)) //手动加气。
        {
            TaskAdd(KEY_OK, StartPrecooling,StartFueling);
        }
        if(sysparas.fuelmode==2)                        //标签启动。
        {
            TaskAdd(KEY_OK, WaitBQresults, NULL);
        }

    }

}

/***************************************************************************
**函数名称：FixMoney
**函数功能：定金额函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void FixMoney(void)
{
    char buf[16]= {0};
    ScreenInput.inputmaxlen=5;
    NumKeyHandler();
    globalvar.fix=2;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    globalvar.fixmoney=atol(buf);
    Fyd12864ClearLine(3);
    Fyd12864DispPrintf(2,1,"定金额:%ld.%02d",globalvar.fixmoney/100,globalvar.fixmoney%100);
    TaskAdd(KEY_RET, DispFirst, NULL);
    if((m1card.existflag==1)&&(m1card.checkresult==0))
    {
        if(sysparas.fuelmode==0)                          //手动加气。
        {
            TaskAdd(KEY_OK, StartPrecooling, NULL);
        }
        if(sysparas.fuelmode==2)                          //标签启动。
        {
            TaskAdd(KEY_OK, WaitBQresults, NULL);
        }

    }
    else
    {
        if((sysparas.fuelmode==0)&&(sysparas.usecard==0)) //手动加气。
        {
            TaskAdd(KEY_OK, StartPrecooling,StartFueling);
        }
        if(sysparas.fuelmode==2)                          //标签启动。
        {
            TaskAdd(KEY_OK, WaitBQresults, NULL);
        }
    }
}

/***************************************************************************
**函数名称：DisPlayLock
**函数功能：加密板显示函数。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-7-25
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void DisPlayLock(void)
{
    Fyd12864ClearLine(1);
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    if(SI_INDATE==0)//设置状态。
    {
        Fyd12864DispPrintf(2,1,"非正常状态");
    }
    else
    {
        DispUI=DispFirst;
    }
}

/***************************************************************************
**函数名称：SearchBlackList
**函数功能：从Flash中寻找是否有相应的黑白名单卡号。
**入口参数：type:1-新删黑名单
                                            2-新增黑名单
                                            3-基础黑名单
                                            4-白名单
                                   cardNo:需要查找的卡号
**返 回 值：1-有；0-无。
**创 建 者：杨朝旭
**创建日期：2016-11-30 16:03:19
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
uint8_t SearchBlackList(uint8_t type,uint8_t *cardNo)
{
    uint32_t  addr,value;
    uint8_t   readAarray[8];
    int       low,mid,high,result;
    if(type==0x01)     //新删黑名单
    {
        addr=FlashDelBlkSADDR;
        value=sysparas.blackdelnum;
    }
    else if(type==0x02)//新增黑名单
    {
        addr=FlashAddBlkSADDR;
        value=sysparas.blackaddnum;
    }
    else if(type==0x03)//基础黑名单
    {
        addr=FlashBlkSADDR;
        value=sysparas.blacknum;
    }
    else if(type==0x04)//白名单
    {
        addr=FlashWhiteSADDR;
        value=sysparas.whitenum;
    }
    low=0;
    mid=0;
    high=value-1;
    while(low<=high)
    {
        mid=(low+high)/2;
        memset(&readAarray[0],0,sizeof(readAarray));
        ReadFlash(0,addr+mid*8, &readAarray[0], 8);
        result=memcmp(&cardNo[0],&readAarray[0],8);
        if(result>0)
        {
            low=mid+1;
        }
        else if(result<0)
        {
            high=mid-1;
        }
        else
        {
            return 1;
        }
        if(high<0)
        {
            return 0;
        }
    }
    return 0;
}


/***************************************************************************
**函数名称：BcdToStr
**函数功能：将BCD码转化为字符串。
**入口参数：无
**返 回 值：无
**创 建 者：杨传坤
**创建日期：2016-8-18
**
**修改者：
**修改日期：
**修改内容：
***************************************************************************/
void BcdToStr(uint8_t bcd[],uint8_t str[])
{
    str[0]=((bcd[0]>>4)+0x30);
    str[1]=((bcd[0]&0x0f)+0x30);
    str[2]=((bcd[1]>>4)+0x30);
    str[3]=((bcd[1]&0x0f)+0x30);
    str[4]=((bcd[2]>>4)+0x30);
    str[5]=((bcd[2]&0x0f)+0x30);
    str[6]=((bcd[3]>>4)+0x30);
    str[7]=((bcd[3]&0x0f)+0x30);
}


