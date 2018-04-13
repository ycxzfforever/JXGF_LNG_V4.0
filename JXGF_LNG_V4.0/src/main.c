#include "kernel.h"
GlobalVar        globalvar;                                     //全局变量
ShiftRecordInfo  shiftrecordinfo;                               //上下班记录信息
ShiftRecordInfo  shiftrecordinfo_tmp;
FuelRecordInfo   fuelrecordinfo;                                //加气流水记录信息
FuelRecordInfo   fuelrecordinfo_tmp;
Sysparas         sysparas;                                      //系统参数
Sysparas         tmp_sysparas;                                  //用户修改系统参数时，临时存储需要修改的参数
M1carddata       m1card;       		                            //卡结构数据。

uint32_t         syserr;                                        //系统异常
uint32_t         stopreason;                                    //停机原因

//必须用卡加气时，未插卡而扫描了电子标签
void DispInserCard(void)
{   
    Fyd12864ClearLine(1);
    if(sysparas.offlinemode==1)
    {
        Fyd12864DispPrintf(2,1,"脱机模式不可用卡"); 
        Fyd12864ClearLine(3);
    }
    else
    {        
        Fyd12864DispPrintf(2,1,"请插入IC卡后，再");
        if((sysparas.fuelmode==2)&&(globalvar.startstate==0))
        {
            Fyd12864DispPrintf(3,1,"扫描电子标签");
        }
        else
        {
            Fyd12864DispPrintf(3,1,"启动加气机");
        }
    }
    Fyd12864ClearLine(4);
    TaskAdd(KEY_RET,DispFirst,NULL);    
}

void DispDateTime(void)
{

    if(globalvar.U32Temp<5)
    {
        globalvar.U32Temp++;
        Fyd12864DispPrintf(4,1,"20%02X-%02X-%02X %02X:%02X", 
            time.year,time.month,time.day, time.hour,time.minute);        
    }
    else if(globalvar.U32Temp<10)
    {
        globalvar.U32Temp++;
        Fyd12864DispPrintf(4,1,"20%02X-%02X-%02X %02X %02X", 
            time.year,time.month,time.day, time.hour,time.minute); 
    }
    else globalvar.U32Temp=0;
}

char ErrorInfo[][16]=
{
    "主FLASH 检测失败",
    "副FLASH 检测失败",
    "写主存错",
    "读主存错",
    "写副存错",// 4
    "读副存错",
    "写内部EEPROM错误",
    "读内部EEPROM错误",
    "写外部EEPROM错误",//8
    "读外部EEPROM错误",
    "内外EE数据不一致",
    "铁电测试错误",
    "气相流量计通讯错",
    "液相流量计通讯错",// 13
    "压力传感器异常",
};

void DispErrorInfo()
{   
    uint8_t row=0,num=0,i=4;
    uint32_t tmpsyserror=syserr;
    globalvar.workstate=StaFault; //加气机故障。
    for(row=0;row<4;)
    {
        num=GetOnePosition(tmpsyserror);//返回的位置比实际位置大1
        if((num==13)||(num==14)||(num==15)) ReadModbusAndPressure();
        if(num>0)
        {
            ClrBit(tmpsyserror, num-1);//找到1的位置后将其清除
            row++;
            Fyd12864DispPrintf(row,1,ErrorInfo[num-1]);
        }
        else break;
    }
    for(i=4;i>row;i--)
    {
        Fyd12864ClearLine(i);
    }          
    if(sysparas.lcdtype==1) //主要用于大屏显示报警
    {        
        OsplcShowN(fuelrecordinfo.volume,fuelrecordinfo.money,fuelrecordinfo.price);
    } 
}


//首界面显示
void DispFirst(void)
{
    timer1off();
    InputInit();
    globalvar.timeout=0;           								//超时时间清零。    
    globalvar.modbus_g_error=0;    								//读气相流量计错次数清零。
    globalvar.modbus_l_error=0;    								//读液相流量计错次数清零。    
    globalvar.fuelstep=0;         								//加气步骤。
    globalvar.fix=0;               								//定量标志。
    globalvar.fixnum=0;
    globalvar.fixmoney=0;
    globalvar.U8Temp=1;
    globalvar.displayonce=1;
    globalvar.devicetest=0;
    globalvar.stoptimes=0;    
    memset(&m1card,0,sizeof(M1carddata));   					//卡数据结构体清零。    
    if(syserr)
    {
        DispErrorInfo();
    }
    else
    {
        if((sysparas.offonwork==1)&&(sysparas.yesnolock==0))    //判断加气机是否处在锁定或者下班状态。
        {
			globalvar.workstate=StaIdle;                        //加气机空闲。
            if(sysparas.usepwdboard==1)                         //如果使用加密板。
            {
                if(SI_INDATE==0)                                //如果加密板是SET状态。
                {
                    Fyd12864DispPrintf(1,1,"加气机是设置状态");
                }
                else
                {
                    Fyd12864DispPrintf(1,1,"枪号:%02d 班号:%02d",sysparas.gunnum,sysparas.shiftnum);
                }
            }
            else
            {
                Fyd12864DispPrintf(1,1,"枪号:%02d 班号:%02d",sysparas.gunnum,sysparas.shiftnum);
            }
            if(globalvar.eepromflag==1)
            {
                Fyd12864DispPrintf(2,1,"EEPROM次数超限");
                Fyd12864DispPrintf(3,1,"请及时更换主板");
            }
            else
            {
                Fyd12864DispPrintf(2,1,"气量:%ld.%02d",fuelrecordinfo.volume/100,fuelrecordinfo.volume%100);
                Fyd12864DispPrintf(3,1,"金额:%ld.%02d",fuelrecordinfo.money/100,fuelrecordinfo.money%100);            
            }
            if((globalvar.comfailcount>10)&&(sysparas.offlinemode==0)) //判断主板与后台是否断开连接。          
            {
                Fyd12864DispPrintf(4,1,"加气机已掉线");
            }
            else
            {            
                DispDateTime();
            }
            TaskAdd(KEY_BACK, StartPrecooling,DispFirst);       //只是预冷
			if((sysparas.fuelmode==0)&&(sysparas.usecard==0))   //手动加气。
			{
				TaskAdd(KEY_START, StartPrecooling,StartFueling);//预冷后加液。
			}
            else if(sysparas.fuelmode==2)                       //标签启动。
			{
			    if(sysparas.usecard==0)
    			{
    			    TaskAdd(KEY_START, DispScanRFID,NULL);
    				if(globalvar.startstate!=0xff)//电子标签不正常或没有标签命令。
    				{											
    					DispUI=BQresults;
    				}
                }
                else if(globalvar.startstate==0)  //未插卡收到标签信息
                {
                    globalvar.startstate=0xFF;
                    DispUI=DispInserCard;
                }
			}
            if(sysparas.usecard==0)
    		{
                TaskAdd(KEY_FIX, FixGas,NULL);                  //定量加气。
            }
            else
            {
                TaskAdd(KEY_START, DispInserCard,NULL);
                TaskAdd(KEY_FIX, DispInserCard,NULL);
            }
        }
        else
        {
            Fyd12864ClearLine(1);             
            if(sysparas.yesnolock==1)
            {
				globalvar.workstate=StaClok;
                Fyd12864DispPrintf(2,1,"加气机已锁");
                Fyd12864ClearLine(3);
            }
            else if(sysparas.offonwork==0)
            {
				globalvar.workstate=StaOffDuty;                 //加气机下班。
                Fyd12864DispPrintf(2,1,"    下班中..");
                if(sysparas.offlinemode==1)//脱机
                    Fyd12864DispPrintf(3,1,"按【停止+7】上班");
                else
                    Fyd12864ClearLine(3);
            }
            Fyd12864ClearLine(4);
        }       
    }
    if(sysparas.offlinemode==1)									//脱机模式
	{
	    if(sysparas.offonwork==0)
		    TaskAdd(KEY_COMB_7KEY,OnWorkWithPwd,NULL);          //停止+7上班
        else if(sysparas.offonwork==1)
            TaskAdd(KEY_COMB_8KEY,OffWorkWithPwd, NULL);        //停止+8下班
	}       
    TaskAdd(KEY_QRY, DispQuery1,NULL);                          //查询。
    TaskAdd(KEY_SET, DispInputMenuPwd,NULL);                    //设置。
    TaskAdd(KEY_COMB_SBACK,DispInputQcPwd,NULL);                //停止+回显 修改质检参数    (还需根据质检模式修改)   
    TaskAdd(KEY_COMB_SQRY,DispInputTestPwd,NULL);               //停止+查询 硬件测试。   
    TaskAdd(KEY_COMB_SSET,DispInputResetPwd, NULL);             //停止+菜单 计量单位，质检模式，恢复出厂设置。   
    
}

//循环加气测试时，按定量处理
void CirculateFueling(void)
{
	globalvar.fixnum=100+time.day+time.hour+time.minute+time.second;
	globalvar.fix=1;										//定量
	DispUI=StartPrecooling;
    if(sysparas.offlinemode==1)//脱机
        DispNextUI=StartFueling;
    else
        DispNextUI=GreyLock;
    globalvar.displayonce=1; 
}


/*************************************************************************
** 函数名称：Task100msProcess
** 函数功能：100ms任务处理
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task100msProcess(void)
{
    timer0.flag_100ms=0;
    if((globalvar.BeepCount--)==0){    BEEP_OFF();}
    if(globalvar.MsgTime>0)  globalvar.MsgTime--;   
    RecBlckDataCheck();
    if(globalvar.comreceiveflag==1)          				 //后代有命令来临标志。
    {
        BGCommunication(RecBack485Buf[4]);  				 //后台命令解析。
        globalvar.comreceiveflag=0;
    }
    DispUI();
    globalvar.KeyValue=0; //界面刷新后，需将键值清零
}

/*************************************************************************
** 函数名称：Task500msProcess
** 函数功能：500ms任务处理
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task500msProcess(void)
{
    timer0.flag_500ms=0;  
    Ds3232ReadTime();                 //更新时间。
    RunLed1();                        //led1 500ms闪烁一次			    
    if((SI1STATUS!=0)&&(SI2STATUS!=0))//一泵双机加液时，判断状态
    {
        globalvar.otherstate=1;
    }
    else
    {
        globalvar.otherstate=0;
    }    
}

/*************************************************************************
** 函数名称：Task1sProcess
** 函数功能：1s任务处理
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨朝旭
** 创建时间：2016-12-7 13:21:02

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void Task1sProcess(void)
{
    timer0.flag_1s=0;
    FEEDDOG();                      //喂狗。
    if((globalvar.devicetest==0)&&((globalvar.workstate==StaIdle)||
                                    (globalvar.workstate==StaOffDuty)||
                                    (globalvar.workstate==StaFault))) //下班，空闲，故障下实时判断，电磁阀测试时，不判断
    {
        if(globalvar.otherstate==0)//对方加液
        {
            VCTRL_OPENKEY();
            VCTRL_MID_CLOSE();      //中关
            VCTRL_HIGH_CLOSE();     //高关
        }
        else if(globalvar.otherstate==1)//对方待机
        {
            VCTRL_OPENKEY();
            VCTRL_MID_OPEN();      //中开
            VCTRL_HIGH_CLOSE();    //高关
        }
    }
    if(globalvar.stoppumptime>0) 
    {
        globalvar.stoppumptime--;//延时停泵倒计时
        if((globalvar.stoppumptime==0)&&(globalvar.workstate!=StaFuel))//加液后延时停泵
        {
            if(globalvar.otherstate==1) //对方未加液
            {
                PumpSwitch(StaIdle);
            }
            else                        //对方加液
            {
                PumpSwitch(StaOtherFuel);
            }
        }	
    }
    if((sysparas.offlinemode==0)&&(m1card.existflag==0)&&
        ((globalvar.workstate==StaIdle)||(globalvar.workstate==StaOffDuty)))			 //无卡状态且在空闲下班时寻卡。
    {
        if(Mt318GetCardSerialNum())  						 //寻卡。
        {                    
            DispUI=GetCardData;      						 //读卡。
        }
    }
    if(m1card.existflag==1)									 //ycx20161125若有卡，实时检测卡状态
    {
        if(Mt318GetCardStatus()==FALSE)
        {
            if(Mt318GetCardStatus()==FALSE)					 //两次判断都没有卡，证明卡被拔出
            {
                m1card.existflag=0;
                m1card.checkresult=0xFF;
            }
        }
    }
    if((globalvar.circulatefue == 1)&&(globalvar.workstate==StaIdle)
        &&(((sysparas.offlinemode==1)&&(globalvar.displayonce==1))||((m1card.existflag==1)&&(m1card.checkresult==0)))) //是否循环加气。
	{
	    globalvar.displayonce=0;
    	TaskMessage(20, CirculateFueling);
	} 
}



/*********************************************************************************************************
** 函数名称：main
** 函数描述：主函数,LPC1788模板
** 输入参数：无
** 返回值  ：系统返回
*********************************************************************************************************/
int main(void)
{
    uint8_t count=0;
    MsDelay(800);                                                    //防止硬件电压还没准备好，导致初始化不成功
	SystemInit();                   
    KeyPinInit();                                                    //按键板初始化
	GPIOInit();                                                      //GPIO初始化
	WATCH_DOG();
    PumpSwitch(StaIdle);        									 //上电后将加气机设置为空闲状态。
	Fyd12864Init();                                                  //主显屏初始化   
    I2C0Init(400000);                                                //i2c0初始化 最大400K
    I2C1Init(400000);                                                //i2c1初始化 最大400K
    EEPROMInit();                                                    //内部eeprom初始化
    At24c512Init();                                                  //外部eeprom初始化
    Fm25L04AndFlashSpiInit();	                                     //初始化spi0    	        	
    timer0Init();                                                    //定时器0初始化
    timer1Init();                                                    //定时器1初始化
    Fm25l04Test();     
    LockPortInit();                                                  //加密板set方向设置。
    PowerCheck();                                                    //掉电方向设置为输入。 
    DispUI=DispFirst;
    globalvar.comreceiveflag=0;                                      //后台有数据来临标志。
    globalvar.startstate=0xff;                                       //清电子标签命令。
    if(SSTFlashTest(0)) SetBit(syserr, SYSERR_FLASH_A_CHK);
    if(SSTFlashTest(1)) SetBit(syserr, SYSERR_FLASH_B_CHK); 
    MsDelay(100);
    WATCH_DOG();
    ReadSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
    for(count=0;count<4;count++)
    {
        if(sysparas.writeE2num>=EEPROM_W_TIMES)
        {
            SYSPARA_ADDR +=sizeof(Sysparas);
            ReadSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
        }
        else break;
    }
    if(count==4) globalvar.eepromflag=1;
    else    globalvar.eepromflag=0;    
    MsDelay(100);
    UARTInit(0, 9600);      	                                     //读卡器、打印机
	UARTInit(1, sysparas.modbusbaud);     	                         //流量计
	UARTInit(2, 9600);                                               //串口屏、DB9              
	UARTInit(3, sysparas.pcbaud);                                    //后台
	UARTInit(4, 9600);      	                                     //电子标签	
    CloseUartRcvIRQ(3);                                              //关后台中断。        
    Fm25l04Read(0,(uint8_t *)(&realtime.lcng),sizeof(RealTimeData)); //读铁电上次掉电保存数据。
    MsDelay(100);
    WATCH_DOG();
    if((realtime.workstatus==StaFuel)||(realtime.workstatus==StaPay))//如果上次加气机正在加气或者扣费过程中掉电。
    {
        PowerDownRealDate();
        MsDelay(1000);
    }
    else
    {
        if(sysparas.transnum>0) ReadFuelRecord(sysparas.transnum-1,0);
    }
    memset(&realtime.lcng,0,sizeof(RealTimeData));                   //清零实时数据。
    Fm25l04Write(0,(uint8_t *)(&realtime.lcng),sizeof(RealTimeData));//清铁电。   
    OsplcInit();                                                     //客显屏初始化                                                                                       
    if(sysparas.shiftransnum>0) ReadFuelRecord(sysparas.shiftransnum-1,1);
    OpenUartRcvIRQ(PCPort);                                          //开后台串口中断。        
    WATCH_DOG();
    MsDelay(800);
    if(sysparas.simulation==0)   ModbusInitReg();             
    
    while(1)
    {           
        realtime.workstatus=globalvar.workstate;
        KeyGetValue();  
#if 0
        if(modbus.flag==1)
        {
            modbus.flag=0;
            ModbusDataDisposs();
        }        
#endif
        if(timer0.flag_100ms==1)
        {
            Task100msProcess();
        }
        if(timer0.flag_500ms==1)
        {	            
            Task500msProcess();
		}          
        if(timer0.flag_1s==1)
        {
            Task1sProcess();
        }               
    }
}


/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
