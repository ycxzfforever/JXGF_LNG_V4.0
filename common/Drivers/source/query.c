/***************************************************************************
**文本：query.c
**说明：用于查询显示，由于函数较多，且大同小异，
                        所以只对每个函数简单描述
**创建者 ：杨朝旭
**创建日期：2016-4-25 10:46:36

**修改者：
**修改日期：
**修改内容：
***************************************************************************/

#include "kernel.h"
Reply5B  tmp;//用于修改记录查询变量使用。
//总累查询显示
void PrinterSum(void)
{
    PrintSum();
    DispUI=DispSum;
}

void DispSum(void)
{
    Fyd12864DispPrintf(1,1,"流水号%ld",sysparas.transnum);
    Fyd12864DispPrintf(2,1,"已传流水号%ld",sysparas.uptransnum);
    Fyd12864DispPrintf(3,1,"总气量%ld.%02d",sysparas.totvolume/100,sysparas.totvolume%100);
    Fyd12864DispPrintf(4,1,"总金额%ld.%02d",sysparas.totmoney/100,sysparas.totmoney%100);
    TaskAdd(KEY_RET, DispQuery1,NULL); 
	TaskAdd(KEY_OK, PrinterSum,NULL); 
}

//班累数据显示后面根据本记录数据读取查询
void DispShiftSum(void)
{
    Fyd12864DispPrintf(1,1,"班号%ld",sysparas.shiftnum);
    Fyd12864DispPrintf(2,1,"班加气次数%ld",sysparas.shiftfueltimes);
    Fyd12864DispPrintf(3,1,"班气量%ld.%02d",sysparas.shiftotvol/100,sysparas.shiftotvol%100);
    Fyd12864DispPrintf(4,1,"班金额%ld.%02d",sysparas.shiftotmon/100,sysparas.shiftotmon%100);    
    TaskAdd(KEY_RET, DispQuery1,NULL); 
}

//请输入流水号
void DispInputTransNum(void)
{
	if(sysparas.transnum > 0)
	{
		//Fyd12864ClearLine(1);
		Fyd12864DispPrintf(1,1,"          %05ld",sysparas.transnum);
		Fyd12864DispPrintf(2,1,"请输入流水号");
		Fyd12864DispPrintf(3,1,"%s",ScreenInput.array);    
		ScreenInput.inputmaxlen=16;
		ScreenInput.pointnum=0;
		fuelrecordinfo.transnum=atol((char *)ScreenInput.array);
		NumKeyHandler(); 
		TaskAdd(KEY_RET, DispQuery1,NULL);
		if(fuelrecordinfo.transnum<=sysparas.transnum)
		{
			Fyd12864ClearLine(4);
			if(fuelrecordinfo.transnum==0)
			{
				fuelrecordinfo.transnum=sysparas.transnum;
			}
			TaskAdd(KEY_OK, DispRecord,NULL);
		}
		else
		{
			Fyd12864DispPrintf(4,1,"流水号超限");
		}
	}
	else
	{
		Fyd12864ClearLine(1);
		Fyd12864DispPrintf(2,1,"  无流水记录");
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
		TaskMessage(20, DispQuery1);
	}
}

//请输入班流水号
void DispInputShifTransNum(void)
{
    //Fyd12864ClearLine(1);
	Fyd12864DispPrintf(1,1,"          %05ld",sysparas.shiftransnum);
    Fyd12864DispPrintf(2,1,"请输入班流水号");
    Fyd12864DispPrintf(3,1,"%s",ScreenInput.array);    
    ScreenInput.inputmaxlen=16;
    ScreenInput.pointnum=0;
    shiftrecordinfo.classid=atol((char *)ScreenInput.array);
    NumKeyHandler(); 
    TaskAdd(KEY_RET, DispQuery2,NULL);
    if(shiftrecordinfo.classid<=sysparas.shiftransnum)
    {
        Fyd12864ClearLine(4);
        if(shiftrecordinfo.classid==0)
        {
            shiftrecordinfo.classid=sysparas.shiftransnum;
        }
        TaskAdd(KEY_OK, DispshifRecord,NULL);
    }
    else
    {
        Fyd12864DispPrintf(4,1,"流水号超限");
    }
}

//班流水记录查询显示
void DispshifRecord(void)
{
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        ReadFuelRecord(shiftrecordinfo.classid-1,1);
    }
    Fyd12864DispPrintf(1,1,"班流水号: %ld",  shiftrecordinfo.classid);
    if(shiftrecordinfo.transactiontype==0)
        Fyd12864DispPrintf(2,1,"上班卡号%02x%02x%02x%02x",shiftrecordinfo.cardstaffnum[4],shiftrecordinfo.cardstaffnum[5],
                                            shiftrecordinfo.cardstaffnum[6],shiftrecordinfo.cardstaffnum[7]);
    else if(shiftrecordinfo.transactiontype==1)
        Fyd12864DispPrintf(2,1,"下班卡号%02x%02x%02x%02x",shiftrecordinfo.cardstaffnum[4],shiftrecordinfo.cardstaffnum[5],
                                            shiftrecordinfo.cardstaffnum[6],shiftrecordinfo.cardstaffnum[7]);
    Fyd12864DispPrintf(3,1,"班气量:%ld.%02d",shiftrecordinfo.classgasnum/100,shiftrecordinfo.classgasnum%100);
    Fyd12864DispPrintf(4,1,"班金额:%ld.%02d",shiftrecordinfo.classgasmoney/100,shiftrecordinfo.classgasmoney%100);  
    TaskAdd(KEY_UP,RecordShifUp,NULL);
    TaskAdd(KEY_DOWN,RecordShifDown,NULL);
    TaskAdd(KEY_RET, DispQuery2,NULL); 
    TaskAdd(KEY_OK, Printershifid,NULL); 
}
void Printershifid(void)
{
    PrintShifid();
    DispUI=DispshifRecord;
}
//班流水号下键查询
void RecordShifUp(void)
{
     if(shiftrecordinfo.classid>1)
    {
        shiftrecordinfo.classid--;
        globalvar.displayonce=1;
    }
    DispUI=DispshifRecord;
}

//班流水号上键查询
void RecordShifDown(void)
{
    if(shiftrecordinfo.classid<sysparas.shiftransnum)
    {
        shiftrecordinfo.classid++;
        globalvar.displayonce=1;
    }
    DispUI=DispshifRecord;
}

//流水号上键查询
void RecordUp(void)
{
    if(fuelrecordinfo.transnum>1)
    {
        fuelrecordinfo.transnum--;
        globalvar.displayonce=1;
    }
    DispUI=DispRecord;
}

//流水号下键查询
void RecordDown(void)
{
    if(fuelrecordinfo.transnum<sysparas.transnum)
    {
        fuelrecordinfo.transnum++;
        globalvar.displayonce=1;
    }
    DispUI=DispRecord;
}

//流水记录查询显示
void DispRecord(void)
{
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        
        ReadFuelRecord(fuelrecordinfo.transnum-1,0);
    }
    Fyd12864DispPrintf(1,1,"流水号:%ld",fuelrecordinfo.transnum);
    Fyd12864DispPrintf(2,1,"气量:%ld.%02d",fuelrecordinfo.volume/100,fuelrecordinfo.volume%100);
    Fyd12864DispPrintf(3,1,"单价:%d.%02d",fuelrecordinfo.price/100,fuelrecordinfo.price%100);    
    Fyd12864DispPrintf(4,1,"金额:%ld.%02d",fuelrecordinfo.money/100,fuelrecordinfo.money%100);
    TaskAdd(KEY_UP,RecordUp,NULL);
    TaskAdd(KEY_DOWN,RecordDown,NULL);
    TaskAdd(KEY_RET, DispQuery1,NULL); 
    TaskAdd(KEY_OK, Printerid,NULL);
}

void Printerid(void)//打印流水号
{
    Printid();
    DispUI=DispRecord;
}

void PrinterSysInfo(void)//打印系统参数
{
    PrintSysInfo();
    DispUI=DispSysParas1;
}

//系统信息显示1
void DispSysParas1(void)
{
    Fyd12864DispPrintf(1,1,"枪号%02d",sysparas.gunnum);
    Fyd12864DispPrintf(2,1,"单价%d.%02d",sysparas.price[0]/100,sysparas.price[0]%100);
    Fyd12864DispPrintf(3,1,"修改流水号%ld",sysparas.modnum);
    Fyd12864DispPrintf(4,1,"已传修改号%ld",sysparas.upmodnum);
    TaskAdd(KEY_UP,DispSysParas9 ,NULL); 
    TaskAdd(KEY_DOWN,DispSysParas2 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示2
void DispSysParas2(void)
{
    Fyd12864DispPrintf(1,1,"总气量%ld.%02d",sysparas.totvolume/100,sysparas.totvolume%100);
    Fyd12864DispPrintf(2,1,"总金额%ld.%02d",sysparas.totmoney/100,sysparas.totmoney%100);
    Fyd12864DispPrintf(3,1,"班号%d",sysparas.shiftnum);
    Fyd12864DispPrintf(4,1,"最大班号%d",sysparas.shiftmaxnum);

    TaskAdd(KEY_UP,DispSysParas1 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas3 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示3
void DispSysParas3(void)
{
    Fyd12864DispPrintf(1,1,"流量计类型%d",sysparas.modbustype);
    Fyd12864DispPrintf(2,1,"计量单位%d",sysparas.unit);
    Fyd12864DispPrintf(3,1,"质检模式%d",sysparas.qcmode);
    Fyd12864DispPrintf(4,1,"EE写次数%ld",(SYSPARA_ADDR/sizeof(Sysparas))*EEPROM_W_TIMES+sysparas.writeE2num);

    TaskAdd(KEY_UP,DispSysParas2 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas4 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示4
void DispSysParas4(void)
{
    Fyd12864DispPrintf(1,1,"密度%d.%04d",sysparas.densty/10000,sysparas.densty%10000);
    Fyd12864DispPrintf(2,1,"修正系数%d.%04d",sysparas.coeff/10000,sysparas.coeff%10000);
    Fyd12864DispPrintf(3,1,"压力零点%d",sysparas.pzero);
    Fyd12864DispPrintf(4,1,"压力系数%d",sysparas.pcoeff);

    TaskAdd(KEY_UP,DispSysParas3 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas5 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}


//系统信息显示5
void DispSysParas5(void)
{
    Fyd12864DispPrintf(1,1,"卡上限%ld.%02d",sysparas.cardmaxyue/100,sysparas.cardmaxyue%100);
    Fyd12864DispPrintf(2,1,"卡下限%ld.%02d",sysparas.cardminyue/100,sysparas.cardminyue%100);
    Fyd12864DispPrintf(3,1,"PSAM卡座%d",sysparas.cardpsamnum);
    Fyd12864DispPrintf(4,1,"间隔天数%d",sysparas.cardmaxday);

    TaskAdd(KEY_UP,DispSysParas4 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas6 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示6
void DispSysParas6(void)
{
    Fyd12864DispPrintf(1,1,"是否检验钢瓶%d",sysparas.cardcheck);
    Fyd12864DispPrintf(2,1,"流量计波特%ld",sysparas.modbusbaud);
    Fyd12864DispPrintf(3,1,"后台波特率%ld",sysparas.pcbaud);
    Fyd12864DispPrintf(4,1,"加气模式%d",sysparas.fuelmode);

    TaskAdd(KEY_UP,DispSysParas5 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas7 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示7
void DispSysParas7(void)
{
    Fyd12864DispPrintf(1,1,"打印方式%d",sysparas.printmode);
    Fyd12864DispPrintf(2,1,"单价版号%d",sysparas.pricever);
    Fyd12864DispPrintf(3,1,"黑名单版号%d",sysparas.blackver);
    Fyd12864DispPrintf(4,1,"白名单版号%d",sysparas.whitever);

    TaskAdd(KEY_UP,DispSysParas6 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas8 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//系统信息显示8
void DispSysParas8(void)
{
    Fyd12864DispPrintf(1,1,"新增黑版号%d",sysparas.addbkver);
    Fyd12864DispPrintf(2,1,"新删黑版号%d",sysparas.delbkver);
    Fyd12864DispPrintf(3,1,"通用信息版号%d",sysparas.comminfover);
    Fyd12864DispPrintf(4,1,"密钥版号%d",sysparas.lockver);
   

    TaskAdd(KEY_UP,DispSysParas7 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas9 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL);
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);	
}

//系统信息显示9
void DispSysParas9(void)
{
    Fyd12864ClearLine(2);
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    Fyd12864DispPrintf(1,1,"是否使用加密板%d",sysparas.usepwdboard);
    TaskAdd(KEY_UP,DispSysParas8 ,NULL);
    TaskAdd(KEY_DOWN,DispSysParas1 ,NULL); 
    TaskAdd(KEY_RET, DispQuery2,NULL);
	TaskAdd(KEY_OK, PrinterSysInfo,NULL);
}

//查询系统版本号
void DispSystemVer()
{
    Fyd12864ClearLine(1);
    Fyd12864DispPrintf(2,1,"系统版本号");
    Fyd12864DispPrintf(3,1,"%s",VERSION);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_RET, DispQuery1,NULL);
}

//查询菜单显示1
void DispQuery1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.总累数据查询");
    Fyd12864DispPrintf(2,1,"2.班累数据查询");
    Fyd12864DispPrintf(3,1,"3.流水记录查询");
    Fyd12864DispPrintf(4,1,"4.控制参数查询");
    TaskAdd(KEY_1,DispSum ,NULL);
    TaskAdd(KEY_2,DispShiftSum ,NULL);
    TaskAdd(KEY_3,DispInputTransNum ,NULL);
    TaskAdd(KEY_4,DispCtrlParas1,NULL);
    TaskAdd(KEY_5,DispSystemVer,NULL);
    TaskAdd(KEY_DOWN,DispQuery2, NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

//查询菜单显示2
void DispQuery2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.系统信息查询");
    Fyd12864DispPrintf(2,1,"2.班流水号查询");
    Fyd12864DispPrintf(3,1,"3.修改流水查询");
	Fyd12864DispPrintf(4,1,"4.黑白名单查询");
    
    TaskAdd(KEY_1,DispSysParas1 ,NULL);
    TaskAdd(KEY_2,DispInputShifTransNum ,NULL);
    TaskAdd(KEY_3,DispInputXGNum ,NULL);
	TaskAdd(KEY_4,DispHeiBai ,NULL);
    TaskAdd(KEY_UP,DispQuery1, NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

void DispHeiBai(void)
{
	Fyd12864DispPrintf(1,1,"1.黑名单条数查询");
    Fyd12864DispPrintf(2,1,"2.新增黑条数查询");
	Fyd12864DispPrintf(3,1,"3.新删黑条数查询");
	Fyd12864DispPrintf(4,1,"4.白名单条数查询");
	TaskAdd(KEY_1,DispHei ,NULL);
    TaskAdd(KEY_2,DispHeiAdd ,NULL);
	TaskAdd(KEY_3,DispHeiDel ,NULL);
	TaskAdd(KEY_4,DispBai ,NULL);
	TaskAdd(KEY_RET, DispQuery2,NULL);
}

void DispHei(void)
{
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
	Fyd12864DispPrintf(1,1,"黑名单条数：");
	Fyd12864DispPrintf(2,1,"%ld",sysparas.blacknum);
    //DebugHeiBai(FlashBlkSADDR, sysparas.blacknum,"黑名单");
    TaskAdd(KEY_RET, DispHeiBai,NULL);
}
void DispHeiAdd(void)
{
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
	Fyd12864DispPrintf(1,1,"新增黑条数：");
	Fyd12864DispPrintf(2,1,"%ld",sysparas.blackaddnum);
    //DebugHeiBai(FlashAddBlkSADDR, sysparas.blackaddnum,"新增黑名单");
    TaskAdd(KEY_RET, DispHeiBai,NULL);
}
void DispHeiDel(void)
{
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
	Fyd12864DispPrintf(1,1,"新删黑条数：");
	Fyd12864DispPrintf(2,1,"%ld",sysparas.blackdelnum);
    //DebugHeiBai(FlashDelBlkSADDR, sysparas.blackdelnum,"新删黑名单");
    TaskAdd(KEY_RET, DispHeiBai,NULL);
}
void DispBai(void)
{
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
	Fyd12864DispPrintf(1,1,"白名单条数：");
	Fyd12864DispPrintf(2,1,"%ld",sysparas.whitenum);
    //DebugHeiBai(FlashWhiteSADDR, sysparas.whitenum,"白名单");
    TaskAdd(KEY_RET, DispHeiBai,NULL);
}
//请输入修改流水号
void DispInputXGNum(void)
{
	if(sysparas.modnum > 0)
	{
		//Fyd12864ClearLine(1);
		Fyd12864DispPrintf(1,1,"          %05ld",sysparas.modnum);
		Fyd12864DispPrintf(2,1,"请输入修改流水号");
		Fyd12864DispPrintf(3,1,"%s",ScreenInput.array);    
		ScreenInput.inputmaxlen=16;
		ScreenInput.pointnum=0;
		globalvar.xgtransnum=atol((char *)ScreenInput.array);
		NumKeyHandler(); 
		TaskAdd(KEY_RET, DispQuery2,NULL);
		if(globalvar.xgtransnum<=sysparas.modnum)
		{
			Fyd12864ClearLine(4);
			if(globalvar.xgtransnum==0)
			{
				globalvar.xgtransnum=1;
			}
			TaskAdd(KEY_OK, DispXGRecord,NULL);
		}
		else
		{
			Fyd12864DispPrintf(4,1,"流水号超限");
		}
	}
	else
	{
		Fyd12864ClearLine(1);
		Fyd12864DispPrintf(2,1,"  无修改记录");
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
		TaskMessage(20, DispQuery2);
	}
}
//修改流水记录查询显示
void DispXGRecord(void)
{
	uint32_t addr,id;
    uint8_t *pt;
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        id=globalvar.xgtransnum%FlashModMaxID;
        addr=FlashModSADDR+(id-1)*FlashModLen;
        pt=(uint8_t *)(&tmp.modifyid);
        ReadFlash(1,addr,pt,sizeof(Reply5B));
    }
	if((tmp.paraindex >=73) && (tmp.paraindex <= 77))
	{
		Fyd12864DispPrintf(1,1,"修改流水号%ld",  tmp.modifyid);
        Fyd12864DispPrintf(2,1,"参数索引号%d",  tmp.paraindex);
        Fyd12864DispPrintf(3,1,"旧值：******");
        Fyd12864DispPrintf(4,1,"新值：******"); 
	}
	else if(tmp.paraindex == 30 || tmp.paraindex == 31)
	{
		if(tmp.paraindex==30)
		{
			Fyd12864DispPrintf(1,1,"密度  流水号%ld",  tmp.modifyid);
		}
		else if(tmp.paraindex==31)
		{
			Fyd12864DispPrintf(1,1,"比例  流水号%ld",  tmp.modifyid);
		}
		Fyd12864DispPrintf(2,1,"旧值：%ld.%02d",  tmp.beforeparameter/10000,tmp.beforeparameter%10000);
		Fyd12864DispPrintf(3,1,"新值：%ld.%02d",  tmp.afterparameter/10000,tmp.afterparameter%10000); 
		Fyd12864DispPrintf(4,1,"%02x-%02x-%02x%02x:%02x:%02x", tmp.modifytime[1],tmp.modifytime[2],tmp.modifytime[3],
															tmp.modifytime[4],tmp.modifytime[5],tmp.modifytime[6]);
	}
	else
	{
		if(tmp.dexnum==2)
		{
			Fyd12864DispPrintf(1,1,"修改流水号%ld",  tmp.modifyid);		//tmp是一个Reply5B结构体
			Fyd12864DispPrintf(2,1,"参数索引号%d",  tmp.paraindex);
			Fyd12864DispPrintf(3,1,"旧值：%ld.%02d",  tmp.beforeparameter/100,tmp.beforeparameter%100);
			Fyd12864DispPrintf(4,1,"新值：%ld.%02d",  tmp.afterparameter/100,tmp.afterparameter%100);                  
		}else if(tmp.dexnum==4)
		{
			Fyd12864DispPrintf(1,1,"修改流水号%ld",  tmp.modifyid);
			Fyd12864DispPrintf(2,1,"参数索引号%d",  tmp.paraindex);
			Fyd12864DispPrintf(3,1,"旧值：%ld.%02d",  tmp.beforeparameter/10000,tmp.beforeparameter%10000);
			Fyd12864DispPrintf(4,1,"新值：%ld.%02d",  tmp.afterparameter/10000,tmp.afterparameter%10000); 
		}else if(tmp.dexnum==7)
		{
			Fyd12864DispPrintf(1,1,"修改流水号%ld",  tmp.modifyid);
			Fyd12864DispPrintf(2,1,"参数索引号%d",  tmp.paraindex);
			Fyd12864DispPrintf(3,1,"旧值：-%ld",  tmp.beforeparameter,tmp.beforeparameter);
			Fyd12864DispPrintf(4,1,"新值：-%ld",  tmp.afterparameter,tmp.afterparameter); 
		}else
		{
			Fyd12864DispPrintf(1,1,"修改流水号%ld",  tmp.modifyid);
			Fyd12864DispPrintf(2,1,"参数索引号%d",  tmp.paraindex);
			Fyd12864DispPrintf(3,1,"旧值：%ld",  tmp.beforeparameter,tmp.beforeparameter);
			Fyd12864DispPrintf(4,1,"新值：%ld",  tmp.afterparameter,tmp.afterparameter); 
		}
	}
	TaskAdd(KEY_RET, DispQuery2,NULL); 
	TaskAdd(KEY_UP,RecordXGUp,NULL);
    TaskAdd(KEY_DOWN,RecordXGDown,NULL);    TaskAdd(KEY_RET, DispQuery2,NULL); 
}
//修改流水号下键查询
void RecordXGUp(void)
{
     if(globalvar.xgtransnum>1)
    {
        globalvar.xgtransnum--;
        globalvar.displayonce=1;
    }
    DispUI=DispXGRecord;
}

//修改流水号上键查询
void RecordXGDown(void)
{
    if(globalvar.xgtransnum<sysparas.modnum)
    {
        globalvar.xgtransnum++;
        globalvar.displayonce=1;
    }
    DispUI=DispXGRecord;
}

void DebugHeiBai(uint32_t addr, uint32_t num,uint8_t *ptr)
{
	uint8_t pt[200], i;

#if 0
	uint8_t	blacktest[24]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};		
	SaveFlash(0 , FlashBlkSADDR, blacktest, 24 );
	SaveFlash(1 , FlashBlkSADDR, blacktest, 24 );
			
#endif
#if 1 
	if(globalvar.displayonce==1)
	{
		ReadFlash(1,addr, pt, num*8);//sysparas.blacknum *8);
		SELECT_PRINT();
		WHPrinterHead();
		WHPrintf("        %s\r\n",ptr);
		WHPrinterNormalCmd();
		for(i=0;i < num;i++)//sysparas.blacknum;i++)
		{
			WHPrintf("%d:%02x%02x%02x%02x%02x%02x%02x%02x\r\n", i, pt[i*8],pt[i*8+1],pt[i*8+2],pt[i*8+3],pt[i*8+4],pt[i*8+5],pt[i*8+6],pt[i*8+7]);
		}
		globalvar.displayonce=0;
		SELECT_CARD();
	}
#endif
}
