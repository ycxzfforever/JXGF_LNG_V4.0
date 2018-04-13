/***************************************************************************
**文本：menu.c
**说明：用于菜单显示，由于函数较多，且大同小异，
                        所以只对每个函数简单描述
**创建者 ：杨朝旭
**创建日期：2016-4-15 09:08:28

**修改者：
**修改日期：
**修改内容：
***************************************************************************/
#include "kernel.h"

fun         DispUI;         //界面刷新处理函数
fun         DispNextUI;     //用于消息显示后的下一个界面

//设置参数时显示 numpoint:小数点后面位数,0,2,4有效，等于5时为选择输入
void DispSet(uint8_t numpoint,uint32_t data,char*buf)
{
    char buff[16],tmpbuf[6]={0},i;
    NumKeyHandler();   
    globalvar.pointnum=0;
    globalvar.beforedex=data;
    if(numpoint<5)//有小数点输入(小数点个数为0,2,4)
    {
        Fyd12864DispPrintf(1,1,buf);
        if(numpoint==0)
        {
			if(ScreenInput.array[ScreenInput.inputlen-1] == 0x2E)
			{
				ScreenInput.array[ScreenInput.inputlen-1]=0x20;
				ScreenInput.inputlen--;
			}
            globalvar.pointnum=numpoint;
            globalvar.afterdex=atol((char *)ScreenInput.array);
            Fyd12864DispPrintf(2,1,"旧值:%ld",data);  
        }            
        else if(numpoint==2)
        {
            globalvar.pointnum=numpoint;
            sprintf(buff,"%f",atof((char *)ScreenInput.array)*100);
            globalvar.afterdex=atol(buff);
            Fyd12864DispPrintf(2,1,"旧值:%d.%02d",data/100,data%100);
        }
        else if(numpoint==4)
        {
            globalvar.pointnum=numpoint;
            sprintf(buff,"%f",atof((char *)ScreenInput.array)*10000);
            globalvar.afterdex=atol(buff);
            Fyd12864DispPrintf(2,1,"旧值:%d.%04d",data/10000,data%10000);
        }
        Fyd12864DispPrintf(3,1,"新值:%s",ScreenInput.array);                
        Fyd12864ClearLine(4); 
    }
    else  if(numpoint==5)   //选择输入
    {
		if(ScreenInput.array[ScreenInput.inputlen-1] == 0x2E)
		{
			ScreenInput.array[ScreenInput.inputlen-1]=0x20;
			ScreenInput.inputlen--;
		}
        globalvar.afterdex=atol((char *)ScreenInput.array);
        Fyd12864DispPrintf(1,1,"%s %d",buf,data);
        Fyd12864DispPrintf(4,1,"新值:%s",ScreenInput.array);
    } 
    else  if(numpoint==6)   //密码输入
    {
        globalvar.afterdex=atol((char *)ScreenInput.array);
        Fyd12864DispPrintf(1,1,"%s",buf);
        for(i=0;i<ScreenInput.inputlen;i++) tmpbuf[i]='*';
        Fyd12864DispPrintf(2,1,"新密码:%s",tmpbuf);
        Fyd12864ClearLine(3); 
        Fyd12864ClearLine(4); 
    }
    else  if(numpoint==7)   //负数显示
    {
        globalvar.afterdex=atol((char *)ScreenInput.array);
        Fyd12864DispPrintf(1,1,buf);
        Fyd12864DispPrintf(2,1,"旧值:%ld",data);    
        Fyd12864DispPrintf(3,1,"新值:%s",ScreenInput.array);                
        Fyd12864ClearLine(4);
    }
}

//设置成功显示
void SetSuccess(void)
{
    Fyd12864DispPrintf(4,1,"设置成功");
}

//设置失败显示
void SetFault(void)
{
    Fyd12864DispPrintf(4,1,"设置失败");
}

//确认处理
void DispOK(void)
{      
    uint32_t   addr,id;
	if(ScreenInput.inputlen>0)
	{
		if(globalvar.displayonce==1)
		{
			if(globalvar.paraindex!=34)  tmp_sysparas.modnum++;
			UpLoad5BData.modifyid=tmp_sysparas.modnum;  //修改流水号。
			UpLoad5BData.classnum=tmp_sysparas.shiftnum;//班号。
			memcpy(&UpLoad5BData.cardstaffnum[0],&m1card.cardnum[0],8);//卡号。
			UpLoad5BData.paraindex=globalvar.paraindex;      //参数索引号。
			UpLoad5BData.dexnum=globalvar.pointnum;          //小数位。
			UpLoad5BData.beforeparameter=globalvar.beforedex;//修改前的值。
			UpLoad5BData.afterparameter=globalvar.afterdex;  //修改后的值。
			UpLoad5BData.modifytime[0]=0x20;
			memcpy(&UpLoad5BData.modifytime[1],&time.year,6);//修改时间。  
			sysparas=tmp_sysparas;        
			if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
			{
				id=sysparas.modnum%FlashModMaxID;
				if(id != 0)
				{
					addr=FlashModSADDR+FlashModLen*(id-1);
					SaveFlash(0,addr, (uint8_t *)&UpLoad5BData.modifyid, sizeof(Reply5B));
					SaveFlash(1,addr, (uint8_t *)&UpLoad5BData.modifyid, sizeof(Reply5B));
				}
				SetSuccess();   
			}
			else
			{
				SetFault();   
			}
			globalvar.displayonce=0;
		} 
		if(DispNextUI!=NULL)
		{
			TaskMessage(10, DispNextUI);
		}
	}
	else
	{
		SetFault();
		TaskMessage(10, DispNextUI);
	}
}


//按键处理，实时更新按键后的界面，
//funname1:按键后需要执行的函数,funname2:用于消息显示后的下一个界面
void TaskAdd(uint32_t keyvalue,fun funname1,fun funname2)
{    
    if(globalvar.KeyValue==keyvalue)
    {
		globalvar.KeyValue=0;
        DispUI=funname1;
        if(funname2!=NULL)  DispNextUI=funname2;        
        else    DispNextUI=NULL;
    }
  
}

//消息显示，ms_100:为显示时间，100ms为单位，funname:时间到了过后需要执行的函数
void TaskMessage(uint32_t ms_100,fun funname)
{    
    if((globalvar.MsgTime==0)&&(globalvar.MsgFlag==0))//第一次添加msg任务
    {
        globalvar.MsgFlag=1;
        globalvar.MsgTime=ms_100;
    }
    if((globalvar.MsgTime==0)&&(globalvar.MsgFlag==1))//时间到了过后处理msg任务
    {
        globalvar.MsgFlag=0;
        DispUI=funname;
    }
}

//初始化输入数据buf
void InputInit(void)
{
    memset(&ScreenInput,0,sizeof(Screeninput));
    globalvar.displayonce=1;
}

//计量单位设置
void DispSetUnit(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==1)||(tmp==0))
    {
        tmp_sysparas.unit=tmp;
        globalvar.paraindex=28;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
   
    DispSet(5,sysparas.unit,"计量单位设置");
    Fyd12864DispPrintf(2,1,"0.公斤");
    Fyd12864DispPrintf(3,1,"1.标方");
    TaskAdd(KEY_RET, DispUnitQcReset,NULL);
    TaskAdd(KEY_OK, DispOK,DispUnitQcReset);
}


//默认单价设置
void DispSetDefPrice(void)
{   
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=5;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.price[0]=atol(buf);
    globalvar.paraindex=7;
	if(tmp_sysparas.price[0] > 10000)
	{
		Fyd12864DispPrintf(4,1,"单价超限");
		NumKeyHandler(); 
        return;
	} 
    DispSet(2,sysparas.price[0],"默认单价设置");   
    TaskAdd(KEY_RET, DispSetPrice,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetPrice);
}

//优惠单价1设置
void DispSetPrice1(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=5;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.price[1]=atol(buf);
    globalvar.paraindex=8;
	if(tmp_sysparas.price[1] > 10000)
	{
		Fyd12864DispPrintf(4,1,"单价超限");
		NumKeyHandler(); 
        return;
	}
    DispSet(2,sysparas.price[1],"单价1");   
    TaskAdd(KEY_RET, DispSetOthPrice1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetOthPrice1);
}

//优惠单价2设置
void DispSetPrice2(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=5;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.price[2]=atol(buf);
    globalvar.paraindex=9;
	if(tmp_sysparas.price[2] > 10000)
	{
		Fyd12864DispPrintf(4,1,"单价超限");
		NumKeyHandler(); 
        return;
	}
    DispSet(2,sysparas.price[2],"单价2");   
    TaskAdd(KEY_RET, DispSetOthPrice1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetOthPrice1);
}

//优惠单价3设置
void DispSetPrice3(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=5;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.price[3]=atol(buf);
    globalvar.paraindex=10;
	if(tmp_sysparas.price[3] > 10000)
	{
		Fyd12864DispPrintf(4,1,"单价超限");
		NumKeyHandler(); 
        return;
	}
    DispSet(2,sysparas.price[3],"单价3");   
    TaskAdd(KEY_RET, DispSetOthPrice1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetOthPrice1);
}

//优惠单价4设置
void DispSetPrice4(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=5;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.price[4]=atol(buf);
	if(tmp_sysparas.price[4] > 10000)
	{
		Fyd12864DispPrintf(4,1,"单价超限");
		NumKeyHandler(); 
        return;
	}
    DispSet(2,sysparas.price[4],"单价4"); 
    globalvar.paraindex=11;
    TaskAdd(KEY_RET, DispSetOthPrice1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetOthPrice1);
}

//优惠单价设置1
void DispSetOthPrice1(void)
{   
    InputInit();
    Fyd12864DispPrintf(1,1,"1.单价1 %d.%02d",sysparas.price[1]/100,sysparas.price[1]%100);
    Fyd12864DispPrintf(2,1,"2.单价2 %d.%02d",sysparas.price[2]/100,sysparas.price[2]%100);
    Fyd12864DispPrintf(3,1,"3.单价3 %d.%02d",sysparas.price[3]/100,sysparas.price[3]%100);
    Fyd12864DispPrintf(4,1,"4.单价4 %d.%02d",sysparas.price[4]/100,sysparas.price[4]%100);

    TaskAdd(KEY_1, DispSetPrice1,NULL);
    TaskAdd(KEY_2, DispSetPrice2,NULL);
    TaskAdd(KEY_3, DispSetPrice3,NULL);
    TaskAdd(KEY_4, DispSetPrice4,NULL);  
    TaskAdd(KEY_RET, DispSetPrice,NULL);
}

//质检模式设置
void DispSetQCMode(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1)||(tmp==2)||(tmp==3))
    {
        tmp_sysparas.qcmode=tmp;
        globalvar.paraindex=29;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.qcmode,"质检模式设置");
    Fyd12864DispPrintf(2,1,"0.密码  1.手持机");
    Fyd12864DispPrintf(3,1,"2.IC卡  3.加密版");
    TaskAdd(KEY_RET, DispUnitQcReset,NULL);
    TaskAdd(KEY_OK, DispOK,DispUnitQcReset);
}


//单价设置
void DispSetPrice(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.默认单价设置");
    Fyd12864DispPrintf(2,1,"2.优惠单价设置");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);    
    TaskAdd(KEY_1, DispSetDefPrice,NULL);
    TaskAdd(KEY_2, DispSetOthPrice1,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//枪号设置
void DispSetGunNum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=2;
    tmp_sysparas.gunnum=atol((char *)ScreenInput.array);  
	if(tmp_sysparas.gunnum > 0)
	{
		TaskAdd(KEY_OK, DispOK,DispSysSet1);
	}
    DispSet(0,sysparas.gunnum,"枪号设置");
    globalvar.paraindex=2;
    TaskAdd(KEY_RET, DispSysSet1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet1);
}

//总气量设置
void DispSetTotvolume(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.totvolume=atol(buf);   
    globalvar.paraindex=5;
    DispSet(2,sysparas.totvolume,"总气量设置");
    TaskAdd(KEY_RET, DispSysSet1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet1);
}

//总金额设置
void DispSetTotmoney(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.totmoney=atol(buf);  
    globalvar.paraindex=6;
    DispSet(2,sysparas.totmoney,"总金额设置");
    TaskAdd(KEY_RET, DispSysSet1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet1);
}

//流量计类型设置
void DispSetModbusType(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==1)||(tmp==2))
    {
        tmp_sysparas.modbustype=tmp;
        globalvar.paraindex=27;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.modbustype,"流量计类型设置");
    Fyd12864DispPrintf(2,1,"1.罗斯蒙特");
    Fyd12864DispPrintf(3,1,"2.E+H");
    TaskAdd(KEY_RET, DispSysSet1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet1);
}

//系统参数设置1
void DispSysSet1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.枪号");    
    Fyd12864DispPrintf(2,1,"2.总气量");
    Fyd12864DispPrintf(3,1,"3.总金额");
    Fyd12864DispPrintf(4,1,"4.流量计类型");
    TaskAdd(KEY_1,DispSetGunNum,NULL);
    TaskAdd(KEY_2,DispSetTotvolume,NULL);
    TaskAdd(KEY_3,DispSetTotmoney,NULL);    
    TaskAdd(KEY_4,DispSetModbusType,NULL); 
    //TaskAdd(KEY_UP, DispSysSet2,NULL);
    TaskAdd(KEY_DOWN, DispSysSet2,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//流水号设置
void DispSetTransnum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    ScreenInput.pointnum=0;
    tmp_sysparas.transnum=atol((char *)ScreenInput.array);   
    globalvar.paraindex=3;
    DispSet(0,sysparas.transnum,"流水号设置");
    TaskAdd(KEY_RET, DispSysSet2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet2);
}

//已传流水号设置
void DispSetUpTransnum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    ScreenInput.pointnum=0;
    tmp_sysparas.uptransnum=atol((char *)ScreenInput.array);   
    globalvar.paraindex=4;
    DispSet(0,sysparas.uptransnum,"已传流水号设置");
    TaskAdd(KEY_RET, DispSysSet2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet2);
}

//修改记录流水号设置
void DispSetModnum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    ScreenInput.pointnum=0;
    tmp_sysparas.modnum=atol((char *)ScreenInput.array);   
    globalvar.paraindex=34;
    DispSet(0,sysparas.modnum,"修改流水号设置");
    TaskAdd(KEY_RET, DispSysSet2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet2);
}

//已传参数修改流水号设置
void DispSetUpModnum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    ScreenInput.pointnum=0;
    tmp_sysparas.upmodnum=atol((char *)ScreenInput.array);   
    globalvar.paraindex=35;
    DispSet(0,sysparas.upmodnum,"已传修改号设置");
    TaskAdd(KEY_RET, DispSysSet2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSysSet2);
}


//系统参数设置2
void DispSysSet2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.流水号");
    Fyd12864DispPrintf(2,1,"2.已传流水号");
    Fyd12864DispPrintf(3,1,"3.修改记录流水号");
    Fyd12864DispPrintf(4,1,"4.已传修改流水号");

    TaskAdd(KEY_1,DispSetTransnum,NULL);
    TaskAdd(KEY_2,DispSetUpTransnum,NULL);
    TaskAdd(KEY_3,DispSetModnum,NULL);    
    TaskAdd(KEY_4,DispSetUpModnum,NULL);
    TaskAdd(KEY_UP, DispSysSet1,NULL);
    //TaskAdd(KEY_DOWN, DispSysSet1,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//设置班号
void DispSetShiftNum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.shiftnum=atol((char *)ScreenInput.array); 
	if((tmp_sysparas.shiftnum>0)&&(tmp_sysparas.shiftnum<=tmp_sysparas.shiftmaxnum))
	{
		TaskAdd(KEY_OK, DispOK,DispSetShiftInfo1);;
	}
	else if(tmp_sysparas.shiftnum > tmp_sysparas.shiftmaxnum)
	{
		Fyd12864DispPrintf(4,1,"班号超限");
		NumKeyHandler(); 
		return;
	}
    globalvar.paraindex=36;
    DispSet(0,sysparas.shiftnum,"班号设置");
    TaskAdd(KEY_RET,DispSetShiftInfo1 ,NULL);
}

//设置最大班号
void DispSetShiftMaxNum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=2;
    tmp_sysparas.shiftmaxnum=atol((char *)ScreenInput.array);  
	if(tmp_sysparas.shiftmaxnum>0)
	{
		TaskAdd(KEY_OK, DispOK,DispSetShiftInfo1);
	}
    globalvar.paraindex=37;
    DispSet(0,sysparas.shiftmaxnum,"最大班号设置");
    TaskAdd(KEY_RET,DispSetShiftInfo1 ,NULL);
}

//设置班流水号
void DispSetShiftTransNum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    tmp_sysparas.shiftransnum=atol((char *)ScreenInput.array); 
    globalvar.paraindex=38;
    DispSet(0,sysparas.shiftransnum,"班流水号设置");
    TaskAdd(KEY_RET,DispSetShiftInfo1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo1);
}

//设置已传班流水号
void DispSetShiftUpTransNum(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    tmp_sysparas.shiftuptransnum=atol((char *)ScreenInput.array);  
    globalvar.paraindex=39;
    DispSet(0,sysparas.shiftuptransnum,"已传班流水号设置");
    TaskAdd(KEY_RET,DispSetShiftInfo1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo1);
}


//班组信息设置1
void DispSetShiftInfo1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.班号");
    Fyd12864DispPrintf(2,1,"2.最大班号");
    Fyd12864DispPrintf(3,1,"3.班流水号");
    Fyd12864DispPrintf(4,1,"4.已传班流水号");
    TaskAdd(KEY_1,DispSetShiftNum,NULL);
    TaskAdd(KEY_2,DispSetShiftMaxNum,NULL);
    TaskAdd(KEY_3,DispSetShiftTransNum,NULL);    
    TaskAdd(KEY_4,DispSetShiftUpTransNum,NULL); 
    //TaskAdd(KEY_UP, DispSetShiftInfo3,NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo2,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//班开始枪累设置
void DispSetShiftStartvol(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.shiftstartvol=atol(buf);  
    globalvar.paraindex=41;
    DispSet(2,sysparas.shiftstartvol,"班开始枪累设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo2);
}

//班开始金额设置
void DispSetShiftStartmon(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.shiftstartmon=atol(buf);   
    globalvar.paraindex=42;
    DispSet(2,sysparas.shiftstartmon,"班开始金额设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo2);
}

//班累计气量设置
void DispSetShiftTotvol(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.shiftotvol=atol(buf);  
    globalvar.paraindex=43;
    DispSet(2,sysparas.shiftotvol,"班累计气量设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo2);
}


//班累计金额设置
void DispSetShiftTotmon(void)
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.shiftotmon=atol(buf); 
    globalvar.paraindex=44;
    DispSet(2,sysparas.shiftotmon,"班累计金额设置");
    TaskAdd(KEY_RET, DispSetShiftInfo2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo2);
}

//班组信息设置2
void DispSetShiftInfo2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.班开始枪累");
    Fyd12864DispPrintf(2,1,"2.班开始金额");
    Fyd12864DispPrintf(3,1,"3.班累计气量");
    Fyd12864DispPrintf(4,1,"4.班累计金额");
    TaskAdd(KEY_1,DispSetShiftStartvol,NULL);
    TaskAdd(KEY_2,DispSetShiftStartmon,NULL);
    TaskAdd(KEY_3,DispSetShiftTotvol,NULL);    
    TaskAdd(KEY_4,DispSetShiftTotmon,NULL); 
    TaskAdd(KEY_UP, DispSetShiftInfo1,NULL);
    TaskAdd(KEY_DOWN, DispSetShiftInfo3,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//设置班加气次数
void DispSetShiftFuelTimes(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    tmp_sysparas.shiftfueltimes=atol((char *)ScreenInput.array);  
    globalvar.paraindex=40;
    DispSet(0,sysparas.shiftfueltimes,"班加气次数设置");
    TaskAdd(KEY_RET,DispSetShiftInfo3 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetShiftInfo3);
}

//设置班开始日期
void DispSetShiftStartDate(void)
{
    uint8_t *tmp,i,ret=TRUE;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=6;
		tmp=(uint8_t*)&tmp_sysparas.shiftstarttime;
	  
		for(i=0;i<(ScreenInput.inputlen/2+1);i++)
		{
			*(tmp+i)=AscToBcd(&ScreenInput.array[2*i]);
		}
		
		Fyd12864DispPrintf(1,1,"班开始日期设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X年%02X月%02X日",
								sysparas.shiftstarttime[0],
								sysparas.shiftstarttime[1],
								sysparas.shiftstarttime[2]);
								
		Fyd12864DispPrintf(3,1,"新值: %02X年%02X月%02X日",
								tmp_sysparas.shiftstarttime[0],
								tmp_sysparas.shiftstarttime[1],
								tmp_sysparas.shiftstarttime[2]);   
		if(ScreenInput.inputlen>5) ret=CheckDate(tmp);
		if(ret==FALSE)
		{
			Fyd12864DispPrintf(4,1,"输入格式错误");
			return;
		}
		else
		{
			Fyd12864ClearLine(4);               
		}
		TaskAdd(KEY_RET,DispSetShiftInfo3 ,NULL);
		if(ScreenInput.inputlen==6)//输入完毕后才能按确认键
		{
			globalvar.paraindex=45;
			TaskAdd(KEY_OK,DispOK,DispSetShiftInfo3);
		}
	}
}

//设置班开始时间
void DispSetShiftStartTime(void)
{
    uint8_t *tmp,i,ret=TRUE;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=6;
		tmp=(uint8_t*)&tmp_sysparas.shiftstarttime[3];
	  
		for(i=0;i<(ScreenInput.inputlen/2+1);i++)
		{
			*(tmp+i)=AscToBcd(&ScreenInput.array[2*i]);
		}
		
		Fyd12864DispPrintf(1,1,"班开始时间设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X:%02X:%02X",
								sysparas.shiftstarttime[3],
								sysparas.shiftstarttime[4],
								sysparas.shiftstarttime[5]);
								
		Fyd12864DispPrintf(3,1,"新值: %02X:%02X:%02X",
								tmp_sysparas.shiftstarttime[3],
								tmp_sysparas.shiftstarttime[4],
								tmp_sysparas.shiftstarttime[5]);   
		if(ScreenInput.inputlen>5) ret=CheckTime(tmp);
		if(ret==FALSE)
		{
			Fyd12864DispPrintf(4,1,"输入格式错误");
			return;
		}
		else
		{
			Fyd12864ClearLine(4);               
		}
		TaskAdd(KEY_RET,DispSetShiftInfo3 ,NULL);
		if(ScreenInput.inputlen==6)//输入完毕后才能按确认键
		{
			globalvar.paraindex=45;
			TaskAdd(KEY_OK,DispOK,DispSetShiftInfo3);
		}
	}
}


//班组信息设置3
void DispSetShiftInfo3(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.班加气次数");
    Fyd12864DispPrintf(2,1,"2.班开始日期");
    Fyd12864DispPrintf(3,1,"3.班开始时间");
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1,DispSetShiftFuelTimes,NULL);
    TaskAdd(KEY_2,DispSetShiftStartDate,NULL);
    TaskAdd(KEY_3,DispSetShiftStartTime,NULL); 
    TaskAdd(KEY_UP, DispSetShiftInfo2,NULL);
    //TaskAdd(KEY_DOWN, DispSetShiftInfo1,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//卡额上限设置
void DispSetCardMaxYuE()
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.cardmaxyue=atol(buf);  
    if(tmp_sysparas.cardmaxyue<=100000000)
	{
		TaskAdd(KEY_OK, DispOK,DispSetICInfo1);
	}
	else
	{
		Fyd12864DispPrintf(4,1,"卡额超限");
        NumKeyHandler(); 
        return;
	}
    globalvar.paraindex=51;
    DispSet(2,sysparas.cardmaxyue,"卡额上限设置");
    TaskAdd(KEY_RET, DispSetICInfo1,NULL);
    
}

//卡额下限设置
void DispSetCardMinYuE()
{
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=16;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
    tmp_sysparas.cardminyue=atol(buf); 
    globalvar.paraindex=52;
    DispSet(2,sysparas.cardminyue,"卡额下限设置");
    TaskAdd(KEY_RET, DispSetICInfo1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetICInfo1);
}

//PSAM卡座选择
void DispSetPsamNum(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==1)||(tmp==2)||(tmp==3)||(tmp==4))
    {
        tmp_sysparas.cardpsamnum=tmp;
        globalvar.paraindex=54;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(0,sysparas.cardpsamnum,"PSAM卡座选择 1-4");
    TaskAdd(KEY_RET, DispSetICInfo1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetICInfo1);
}

//设置IC卡未使用最大间隔时间
void DispSetCardMaxDay(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.cardmaxday=atol((char *)ScreenInput.array); 
    globalvar.paraindex=55;
    DispSet(0,sysparas.cardmaxday,"最大间隔天数");
    TaskAdd(KEY_RET,DispSetICInfo2 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetICInfo2);
}

//是否用卡设置
void DispSetUseCard()
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1)||(tmp==2))
    {
        tmp_sysparas.usecard=tmp;
        globalvar.paraindex=57;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.usecard,"用卡选择:");
    Fyd12864DispPrintf(2,1,"0.可不使用卡");
    Fyd12864DispPrintf(3,1,"1.用卡  2.不用卡");
    TaskAdd(KEY_RET, DispSetICInfo1,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetICInfo1);
}

//IC卡信息设置1
void DispSetICInfo1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.卡额上限");
    Fyd12864DispPrintf(2,1,"2.卡额下限");
    Fyd12864DispPrintf(3,1,"3.用卡选择");
    Fyd12864DispPrintf(4,1,"4.PSAM卡座选择");
    TaskAdd(KEY_1,DispSetCardMaxYuE,NULL);
    TaskAdd(KEY_2,DispSetCardMinYuE,NULL);
    TaskAdd(KEY_3,DispSetUseCard,NULL); //ycx20161202
    TaskAdd(KEY_4,DispSetPsamNum,NULL); 
    TaskAdd(KEY_DOWN, DispSetICInfo2,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}

//是否检验钢瓶设置
void DispSetCardCheck(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.cardcheck=tmp;
        globalvar.paraindex=56;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.cardcheck,"是否检验钢瓶");
    Fyd12864DispPrintf(2,1,"0.不检验");
    Fyd12864DispPrintf(3,1,"1.检验");
    TaskAdd(KEY_RET, DispSetICInfo2,NULL);
    TaskAdd(KEY_OK, DispOK,DispSetICInfo2);
}


//IC卡信息设置2
void DispSetICInfo2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.是否检验钢瓶");
    Fyd12864DispPrintf(2,1,"2.最大间隔天数");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1,DispSetCardCheck,NULL);
    TaskAdd(KEY_2,DispSetCardMaxDay,NULL);
    TaskAdd(KEY_UP, DispSetICInfo1,NULL);
    TaskAdd(KEY_RET, DispMenu_1,NULL);
}


//菜单界面1显示
void DispMenu_1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.单价设置");
    Fyd12864DispPrintf(2,1,"2.系统参数设置");
    Fyd12864DispPrintf(3,1,"3.班组信息设置");
    Fyd12864DispPrintf(4,1,"4.IC卡参数设置");
    TaskAdd(KEY_1, DispSetPrice,NULL);
    TaskAdd(KEY_2, DispSysSet1,NULL);
    TaskAdd(KEY_3, DispSetShiftInfo1,NULL);
    TaskAdd(KEY_4, DispSetICInfo1,NULL);
    //TaskAdd(KEY_UP, DispMenu_3,NULL);
    TaskAdd(KEY_DOWN, DispMenu_2,NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

//省编码设置
void DispSetShengCode(void)
{
    uint32_t    tmp;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=4;
		tmp=atol((char *)ScreenInput.array);   
        HexToBcdbuf(tmp, (char *)tmp_sysparas.shengcode, 2);
		globalvar.paraindex=47;
		Fyd12864DispPrintf(1,1,"省编码设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X%02X",
								sysparas.shengcode[0],
								sysparas.shengcode[1]);
		globalvar.beforedex=BcdbufToHex(sysparas.shengcode, 2);						
		Fyd12864DispPrintf(3,1,"新值: %02X%02X",
								tmp_sysparas.shengcode[0],
								tmp_sysparas.shengcode[1]);
	    globalvar.afterdex=BcdbufToHex(tmp_sysparas.shengcode, 2);
        globalvar.pointnum=0;
		Fyd12864ClearLine(4);               
		TaskAdd(KEY_RET,DispGeneralInfo ,NULL);
		TaskAdd(KEY_OK,DispOK,DispGeneralInfo);
	}
}


//站编码设置
void DispSetStationCode(void)
{
    uint32_t    tmp;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=8;
        tmp=atol((char *)ScreenInput.array);   
        HexToBcdbuf(tmp, (char *)tmp_sysparas.stationcode, 4);
		globalvar.paraindex=48;
		Fyd12864DispPrintf(1,1,"站编码设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X%02X%02X%02X",
								sysparas.stationcode[0],
								sysparas.stationcode[1],
								sysparas.stationcode[2],
								sysparas.stationcode[3]);
        globalvar.beforedex=BcdbufToHex(sysparas.stationcode, 4);								
		Fyd12864DispPrintf(3,1,"新值: %02X%02X%02X%02X",
								tmp_sysparas.stationcode[0],
								tmp_sysparas.stationcode[1],
								tmp_sysparas.stationcode[2],
								tmp_sysparas.stationcode[3]);
        globalvar.afterdex=BcdbufToHex(tmp_sysparas.stationcode, 4);
        globalvar.pointnum=0;
	  
		Fyd12864ClearLine(4);               
		TaskAdd(KEY_RET,DispGeneralInfo ,NULL);
		TaskAdd(KEY_OK,DispOK,DispGeneralInfo);
	}
}

//地市编码设置
void DispSetCityCode(void)
{
    uint32_t    tmp;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=4;
		tmp=atol((char *)ScreenInput.array);   
        HexToBcdbuf(tmp, (char *)tmp_sysparas.citycode, 2);
		globalvar.paraindex=49;
		Fyd12864DispPrintf(1,1,"地市编码设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X%02X",
								sysparas.citycode[0],
								sysparas.citycode[1]);
        globalvar.beforedex=BcdbufToHex(sysparas.citycode, 2);								
		Fyd12864DispPrintf(3,1,"新值: %02X%02X",
								tmp_sysparas.citycode[0],
								tmp_sysparas.citycode[1]);
        globalvar.afterdex=BcdbufToHex(tmp_sysparas.citycode, 2);
        globalvar.pointnum=0;	  
		Fyd12864ClearLine(4);               
		TaskAdd(KEY_RET,DispGeneralInfo ,NULL);
		TaskAdd(KEY_OK,DispOK,DispGeneralInfo);
	}
}

//上级单位编码设置
void DispSetParentCode(void)
{
    uint32_t    tmp;
	if(globalvar.KeyValue != KEY_PT)
	{
		tmp_sysparas=sysparas;
		NumKeyHandler();
		ScreenInput.inputmaxlen=8;
		tmp=atol((char *)ScreenInput.array);   
        HexToBcdbuf(tmp, (char *)tmp_sysparas.parentcode, 4);
		globalvar.paraindex=50;		
		Fyd12864DispPrintf(1,1,"上级单位编码设置");  
		Fyd12864DispPrintf(2,1,"旧值: %02X%02X%02X%02X",
								sysparas.parentcode[0],
								sysparas.parentcode[1],
								sysparas.parentcode[2],
								sysparas.parentcode[3]);
        globalvar.beforedex=BcdbufToHex(sysparas.parentcode, 4);									
		Fyd12864DispPrintf(3,1,"新值: %02X%02X%02X%02X",
								tmp_sysparas.parentcode[0],
								tmp_sysparas.parentcode[1],
								tmp_sysparas.parentcode[2],
								tmp_sysparas.parentcode[3]);
        globalvar.afterdex=BcdbufToHex(tmp_sysparas.parentcode, 4);
        globalvar.pointnum=0;		  
		Fyd12864ClearLine(4);               
		TaskAdd(KEY_RET,DispGeneralInfo ,NULL);
		TaskAdd(KEY_OK,DispOK,DispGeneralInfo);
	}
}


//通用信息设置
void DispGeneralInfo(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.省编码");
    Fyd12864DispPrintf(2,1,"2.站编码");
    Fyd12864DispPrintf(3,1,"3.地市代码");
    Fyd12864DispPrintf(4,1,"4.上级单位代码");
    TaskAdd(KEY_1,DispSetShengCode ,NULL);
    TaskAdd(KEY_2,DispSetStationCode ,NULL);
    TaskAdd(KEY_3,DispSetCityCode ,NULL);
    TaskAdd(KEY_4,DispSetParentCode ,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}

void SystemReStart(void)
{
	Fyd12864ClearLine(1);
	Fyd12864ClearLine(3);
	Fyd12864ClearLine(4);
	Fyd12864DispPrintf(2,1,"系统重启中");
	MsDelay(500);
//	Fyd12864DispPrintf(2,1,"系统重启中.");
//	MsDelay(500);
//	Fyd12864DispPrintf(2,1,"系统重启中..");
//	MsDelay(500);
//	Fyd12864DispPrintf(2,1,"系统重启中...");
//	MsDelay(500);

//	LPC_WDT->TC = 0xFF;
//	LPC_WDT->MOD |= 1;
//	LPC_WDT->MOD |= 2;
//	LPC_WDT->FEED = 0xAA;
//    LPC_WDT->FEED = 0x55;
	while(1);
}

//流量计波特率设置
void DispSetModbusBaud(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==1)||(tmp==2)||(tmp==3)||(tmp==4))
    {
        switch(tmp)
        {
            case 1: tmp_sysparas.modbusbaud=9600;   break;
            case 2: tmp_sysparas.modbusbaud=19200;  break;
            case 3: tmp_sysparas.modbusbaud=38400;  break;
            case 4: tmp_sysparas.modbusbaud=115200; break;
        }
        globalvar.paraindex=58;        
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.modbusbaud,"流量计波特率");
    Fyd12864DispPrintf(2,1,"1.9600  2.19200");
    Fyd12864DispPrintf(3,1,"3.38400 4.115200");
    TaskAdd(KEY_RET,DispCommPara ,NULL);
	if(tmp != 0)
	{
		TaskAdd(KEY_OK, DispOK,SystemReStart);
	}
}



//后台波特率设置
void DispSetPCBaud(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==1)||(tmp==2)||(tmp==3)||(tmp==4))
    {
        switch(tmp)
        {
            case 1: tmp_sysparas.pcbaud=9600;   break;
            case 2: tmp_sysparas.pcbaud=19200;  break;
            case 3: tmp_sysparas.pcbaud=38400;  break;
            case 4: tmp_sysparas.pcbaud=115200; break;
        }
        globalvar.paraindex=59;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.pcbaud,"后台波特率");
    Fyd12864DispPrintf(2,1,"1.9600  2.19200");
    Fyd12864DispPrintf(3,1,"3.38400 4.115200");
    TaskAdd(KEY_RET,DispCommPara ,NULL);
	if(tmp != 0)
	{
		TaskAdd(KEY_OK, DispOK,SystemReStart);
	}
}


//通讯参数设置
void DispCommPara(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.流量计波特率");
    Fyd12864DispPrintf(2,1,"2.后台波特率");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1,DispSetModbusBaud ,NULL);
    TaskAdd(KEY_2,DispSetPCBaud ,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}


//压力调校
void SetPressureRatioEnter(void)
{
	uint32_t  pressurevalue,temp;
	char buf[10];
	sprintf(buf,"%f",atof((char *)ScreenInput.array)*100);
	pressurevalue=atol(buf);
	temp=globalvar.U32Temp-tmp_sysparas.pzero;
	temp*=100;
	tmp_sysparas.pcoeff=(uint16_t)(temp/pressurevalue);
    //globalvar.paraindex=33;
	Fyd12864ClearLine(2);
	if(globalvar.displayonce==1)
	{
			globalvar.displayonce=0;
			sysparas=tmp_sysparas;        
			if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
			{
					Fyd12864DispPrintf(3,1,"    设置成功");   
			}
			else
			{
					Fyd12864DispPrintf(3,1,"    设置失败");   
			}
	} 
	Fyd12864DispPrintf(4,1,"返回请按【返回】");
	TaskAdd(KEY_RET,DispSetPressure,NULL);
}
void SetPressureRatio(void)
{
	uint32_t rd;
	tmp_sysparas=sysparas;
	Fyd12864DispPrintf(1,1,"  设置压力系数");
	rd=PlADCGetValue();
	globalvar.U32Temp=rd;
	Fyd12864DispPrintf(2,1,"当前AD值:%ld",rd);
	Fyd12864DispPrintf(3,1,"请输入");
	ScreenInput.inputmaxlen=5;
	NumKeyHandler();
	Fyd12864DispPrintf(4,1,"当前压力:%s",ScreenInput.array);
	TaskAdd(KEY_RET,DispSetPressure,NULL);
	TaskAdd(KEY_OK,SetPressureRatioEnter,NULL);
}

void SetPressureZeroEnter(void)
{
	Fyd12864ClearLine(2);
	if(globalvar.displayonce==1)
	{
			globalvar.displayonce=0;
			sysparas=tmp_sysparas;        
			if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
			{
					Fyd12864DispPrintf(3,1,"    设置成功");   
			}
			else
			{
					Fyd12864DispPrintf(3,1,"    设置失败");   
			}
	} 
	Fyd12864DispPrintf(4,1,"返回请按【返回】");
	TaskAdd(KEY_RET,DispSetPressure,NULL);
}
void SetPressureZero(void)
{
	uint32_t rd;
	tmp_sysparas=sysparas;
	Fyd12864DispPrintf(1,1,"  设置压力零点");
	rd=PlADCGetValue();
	tmp_sysparas.pzero=rd;
	if(tmp_sysparas.pzero<100)
	{
		Fyd12864ClearLine(2);
		Fyd12864DispPrintf(3,1,"压力异常");
	}
	else
	{
		//globalvar.paraindex=32;
		Fyd12864DispPrintf(2,1,"当前AD值:%ld",rd);
		Fyd12864DispPrintf(3,1,"确认请按【确认】");
		TaskAdd(KEY_OK,SetPressureZeroEnter,NULL);
	}
	Fyd12864DispPrintf(4,1,"返回请按【返回】");
	TaskAdd(KEY_RET,DispSetPressure,NULL);
	
}

void DispSetPressure(void)
{
	Fyd12864DispPrintf(1,1,"  校正压力参数");
	Fyd12864DispPrintf(2,1,"1.设置压力零点");
	Fyd12864DispPrintf(3,1,"2.设置压力系数");
	Fyd12864ClearLine(4);
	TaskAdd(KEY_RET, DispCommonOpt1,NULL);
	TaskAdd(KEY_1,SetPressureZero,NULL);
	TaskAdd(KEY_2,SetPressureRatio,NULL);
}


//加气模式设置
void DispSetFuelMode(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1)||(tmp==2))
    {
        tmp_sysparas.fuelmode=tmp;
        globalvar.paraindex=60;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.fuelmode,"加气模式设置");
    Fyd12864DispPrintf(2,1,"0.手动  1.后台");
    Fyd12864DispPrintf(3,1,"2.电子标签");
    TaskAdd(KEY_RET, DispCommonOpt1,NULL);
    TaskAdd(KEY_OK, DispOK,DispCommonOpt1);
}

//设置日期和时间显示确认
void DispSetDateTimeOK(void)
{
    uint8_t tmp[6],i,ret=FALSE;
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        for(i=0;i<6;i++)
        {
            tmp[i]=AscToBcd(&ScreenInput.array[2*i]);
        }
        ret=Ds3232SetTime(tmp);
        if(ret==1)  Fyd12864DispPrintf(4,1,"设置成功");
        else        Fyd12864DispPrintf(4,1,"设置失败");
    }
    TaskMessage(10, DispCommonOpt);
}

//日期时间设置
//ycx20180201增加光标闪烁控制
void DispSetDateTime(void)
{
    uint8_t *tmp,i,ret=TRUE;
	Time    newtime={0};
	if(globalvar.KeyPreValue!=KEY_PT)
	{
		Fyd12864DispPrintf(1,1,"日期时间设置");    
		NumKeyHandler();
		ScreenInput.inputmaxlen=12;
		tmp=(uint8_t*)&newtime;
		newtime=time;
		for(i=0;i<(ScreenInput.inputlen/2+1);i++)
		{
			*(tmp+i)=AscToBcd(&ScreenInput.array[2*i]);
		}		
		Fyd12864DispPrintf(2,1,"%02X年%02X月%02X日",
								newtime.year,
								newtime.month,
								newtime.day);
								
		Fyd12864DispPrintf(3,1,"%02X时%02X分%02X秒",
								newtime.hour,
								newtime.minute,
								newtime.second);	
		if(ScreenInput.inputlen>5) ret=CheckDate(tmp);
		if(ScreenInput.inputlen>9) ret=CheckTime(tmp+3);
		if(ret==FALSE)
		{
			Fyd12864DispPrintf(4,1,"输入格式错误");
			return;
		}
		else
		{
			Fyd12864ClearLine(4);               
		}
        if(ScreenInput.inputlen<2)
            Fyd12864CurrsorBlink(2,1,1);
        else if(ScreenInput.inputlen<4)
            Fyd12864CurrsorBlink(2,3,1);
        else if(ScreenInput.inputlen<6)
            Fyd12864CurrsorBlink(2,5,1);
        else if(ScreenInput.inputlen<8)
            Fyd12864CurrsorBlink(3,1,1);
        else if(ScreenInput.inputlen<10)
            Fyd12864CurrsorBlink(3,3,1);
        else if(ScreenInput.inputlen<12)
            Fyd12864CurrsorBlink(3,5,1);
		TaskAdd(KEY_RET, DispCommonOpt1,NULL);
		if(ScreenInput.inputlen>9)//输入完毕后才能按确认键
		{
			TaskAdd(KEY_OK, DispSetDateTimeOK,NULL);
		}  
	}	
}

//小票打印方式设置
void DispSetPrintMode(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.printmode=tmp;
        globalvar.paraindex=61;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.printmode,"打印方式设置");
    Fyd12864DispPrintf(2,1,"0.手动打印");
    Fyd12864DispPrintf(3,1,"1.自动打印");
    TaskAdd(KEY_RET, DispCommonOpt,NULL);
    TaskAdd(KEY_OK, DispOK,DispCommonOpt);
}

//是否取整设置
void DispSetIntNum()
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.intnumflag = tmp;
        globalvar.paraindex=85;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.intnumflag,"是否取整设置");
    Fyd12864DispPrintf(2,1,"0.不能取整");
    Fyd12864DispPrintf(3,1,"1.可以取整");
    TaskAdd(KEY_RET, DispCommonOpt,NULL);
    TaskAdd(KEY_OK, DispOK,DispCommonOpt);
}

//常用操作设置
void DispCommonOpt(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.屏幕选择");
	Fyd12864DispPrintf(2,1,"2.联机模式");
    Fyd12864DispPrintf(3,1,"3.是否取整");
    Fyd12864DispPrintf(4,1,"4.打印方式");
	TaskAdd(KEY_1,DispSetPing, NULL);
	TaskAdd(KEY_2,DispSetOffline, NULL);//huanghao 20161025    
	TaskAdd(KEY_3,DispSetIntNum, NULL);//ycx20161202
	TaskAdd(KEY_4,DispSetPrintMode ,NULL);
    TaskAdd(KEY_DOWN, DispCommonOpt1,NULL);
    TaskAdd(KEY_RET, DispMenu_2,NULL);
}
//常用操作设置1。
void DispCommonOpt1(void)
{
	Fyd12864ClearLine(4);
	Fyd12864DispPrintf(1,1,"1.压力调校");
    Fyd12864DispPrintf(2,1,"2.加气模式设置");
    Fyd12864DispPrintf(3,1,"3.日期时间设置");
    TaskAdd(KEY_1,DispSetPressure ,NULL);
    TaskAdd(KEY_2,DispSetFuelMode ,NULL);
    TaskAdd(KEY_3,DispSetDateTime ,NULL);
	TaskAdd(KEY_RET, DispMenu_2, NULL);
	TaskAdd(KEY_UP, DispCommonOpt, NULL);
}
//脱机模式选择
void DispSetOffline(void)//huanghao 20161025
{
	uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.offlinemode = tmp;
        globalvar.paraindex=80;
		if(tmp == 1)
        {
			tmp_sysparas.yesnolock=0; //解锁
			tmp_sysparas.usecard=0;   //可以不使用卡
        }
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.offlinemode,"联机模式设置");
    Fyd12864DispPrintf(2,1,"0.在线模式");
    Fyd12864DispPrintf(3,1,"1.脱机模式");
    TaskAdd(KEY_RET, DispCommonOpt,NULL);
	if(tmp_sysparas.offlinemode && globalvar.KeyValue == KEY_OK)
		CloseUartRcvIRQ(3);
	else
		OpenUartRcvIRQ(3);
    TaskAdd(KEY_OK, DispOK,DispCommonOpt);
}
//屏幕选择函数。
void DispSetPing(void)
{
    uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
    tmp=atol((char *)ScreenInput.array);   
    if((tmp==0)||(tmp==1))
    {
        tmp_sysparas.lcdtype=tmp;
        globalvar.paraindex=81;
        //设置屏幕时，刷一次屏ycx20170330
        if(tmp==0) OsplcShowB(fuelrecordinfo.volume,fuelrecordinfo.money,fuelrecordinfo.price,0,1);
        else OsplcShowN(fuelrecordinfo.volume,fuelrecordinfo.money,fuelrecordinfo.price);
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.lcdtype,"屏幕选择设置");
    Fyd12864DispPrintf(2,1,"0.蓝屏");
    Fyd12864DispPrintf(3,1,"1.大屏");
    TaskAdd(KEY_RET, DispCommonOpt,NULL);
    TaskAdd(KEY_OK, DispOK,DispCommonOpt);
}

//菜单界面2显示
void DispMenu_2(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.通用信息设置");
    Fyd12864DispPrintf(2,1,"2.控制参数设置");
    Fyd12864DispPrintf(3,1,"3.通讯参数设置");
    Fyd12864DispPrintf(4,1,"4.常用操作设置");
    TaskAdd(KEY_1,DispGeneralInfo ,NULL);
    TaskAdd(KEY_2,DispSetControl1,NULL);
    TaskAdd(KEY_3,DispCommPara ,NULL);
    TaskAdd(KEY_4,DispCommonOpt ,NULL);
    TaskAdd(KEY_UP, DispMenu_1,NULL);
    TaskAdd(KEY_DOWN, DispMenu_3,NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

//单价版本号设置
void DispSetPriceVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.pricever=atol((char *)ScreenInput.array);   
    globalvar.paraindex=62;
    DispSet(0,sysparas.pricever,"单价版本号设置");
    TaskAdd(KEY_RET,DispVerNum1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum1);
}

//黑名单版本号设置
void DispSetBlackVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.blackver=atol((char *)ScreenInput.array);  
    globalvar.paraindex=63;
    DispSet(0,sysparas.blackver,"黑名单版本号设置");
    TaskAdd(KEY_RET,DispVerNum1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum1);
}

//白名单版本号设置
void DispSetWhiteVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.whitever=atol((char *)ScreenInput.array);   
    globalvar.paraindex=64;
    DispSet(0,sysparas.whitever,"白名单版本号设置");
    TaskAdd(KEY_RET,DispVerNum1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum1);
}

//新增黑名单版本号设置
void DispSetAddBkVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.addbkver=atol((char *)ScreenInput.array);
    globalvar.paraindex=65;
    DispSet(0,sysparas.addbkver,"新增黑名单版本号");
    TaskAdd(KEY_RET,DispVerNum1 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum1);
}

//新删黑名单版本号设置
void DispSetDelBkVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.delbkver=atol((char *)ScreenInput.array);    
    globalvar.paraindex=66;
    DispSet(0,sysparas.delbkver,"新删黑名单版本号");
    TaskAdd(KEY_RET,DispVerNum2 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum2);
}

//通用信息版本号设置
void DispSetCommInfoVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.comminfover=atol((char *)ScreenInput.array);
    globalvar.paraindex=67;
    DispSet(0,sysparas.comminfover,"通用信息版本号");
    TaskAdd(KEY_RET,DispVerNum2 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum2);
}

//密钥信息版本号设置
void DispSetLockVer(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=3;
    tmp_sysparas.lockver=atol((char *)ScreenInput.array); 
    globalvar.paraindex=68;
    DispSet(0,sysparas.lockver,"密钥信息版本号");
    TaskAdd(KEY_RET,DispVerNum2 ,NULL);
    TaskAdd(KEY_OK, DispOK,DispVerNum2);
}

//版本编号设置1
void DispVerNum1()
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.单价版本号");
    Fyd12864DispPrintf(2,1,"2.黑名单版本号");
    Fyd12864DispPrintf(3,1,"3.白名单版本号");
    Fyd12864DispPrintf(4,1,"4.新增黑版本号");
    TaskAdd(KEY_1, DispSetPriceVer,NULL);
    TaskAdd(KEY_2, DispSetBlackVer,NULL);
    TaskAdd(KEY_3, DispSetWhiteVer,NULL);
    TaskAdd(KEY_4, DispSetAddBkVer,NULL);
    //TaskAdd(KEY_UP,DispVerNum2 ,NULL);
    TaskAdd(KEY_DOWN, DispVerNum2,NULL);
    TaskAdd(KEY_RET, DispMenu_3,NULL);
}

//版本编号设置2
void DispVerNum2()
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.新删黑版本号");
    Fyd12864DispPrintf(2,1,"2.通用信息版本号");
    Fyd12864DispPrintf(3,1,"3.密钥信息版本号");

    Fyd12864ClearLine(4);
    TaskAdd(KEY_1,DispSetDelBkVer ,NULL);
    TaskAdd(KEY_2,DispSetCommInfoVer ,NULL);
    TaskAdd(KEY_3,DispSetLockVer ,NULL);
    TaskAdd(KEY_UP,DispVerNum1 ,NULL);
    //TaskAdd(KEY_DOWN,DispVerNum1 ,NULL);
    TaskAdd(KEY_RET, DispMenu_3,NULL);
}

//判断密码是否正确
void DispJudgePwd(void)
{
    InputInit();
    if(globalvar.U8Temp==1)
    {
        if(globalvar.U32Temp!=sysparas.menupwd)
        {
            Fyd12864DispPrintf(4,1,"密码错误");
            TaskAdd(KEY_CLR,DispInputOldPwd ,NULL);
            TaskAdd(KEY_RET,DispInputOldPwd ,NULL);
            TaskMessage(10,DispInputOldPwd);
        }
        else
        {
            DispUI=DispSetMenuPwd;
        }
    }
    else if(globalvar.U8Temp==2)
    {
        if(globalvar.U32Temp!=sysparas.qcpwd)
        {
            Fyd12864DispPrintf(4,1,"密码错误");
            TaskAdd(KEY_CLR,DispInputOldPwd ,NULL);
            TaskAdd(KEY_RET,DispInputOldPwd ,NULL);
            TaskMessage(10,DispInputOldPwd);
        }
        else
        {
            DispUI=DispSetQcPwd;
        }
    }
    else if(globalvar.U8Temp==3)
    {
        if(globalvar.U32Temp!=sysparas.testpwd)
        {
            Fyd12864DispPrintf(4,1,"密码错误");
            TaskAdd(KEY_CLR,DispInputOldPwd ,NULL);
            TaskAdd(KEY_RET,DispInputOldPwd ,NULL);
            TaskMessage(10,DispInputOldPwd);
        }
        else
        {
            DispUI=DispSetTestPwd;
        }
    }
    else if(globalvar.U8Temp==4)
    {
        if(globalvar.U32Temp!=sysparas.resetpwd)
        {
            Fyd12864DispPrintf(4,1,"密码错误");
            TaskAdd(KEY_CLR,DispInputOldPwd ,NULL);
            TaskAdd(KEY_RET,DispInputOldPwd ,NULL);
            TaskMessage(10,DispInputOldPwd);
        }
        else
        {
            DispUI=DispSetResetPwd;
        }
    }
	else if(globalvar.U8Temp==5)
	{
		if(globalvar.U32Temp!=sysparas.onoffworkpwd)
        {
            Fyd12864DispPrintf(4,1,"密码错误");
            TaskAdd(KEY_CLR,DispInputOldPwd ,NULL);
            TaskAdd(KEY_RET,DispInputOldPwd ,NULL);
            TaskMessage(10,DispInputOldPwd);
        }
        else
        {
            DispUI=DispSetonoffworkpwd;
        }
	}
}

//旧密码输入
void DispInputOldPwd(void)
{
    char     buf[6]={0},i;    
    ScreenInput.inputmaxlen=6;
    NumKeyHandler(); 
    Fyd12864ClearLine(1);
    Fyd12864DispPrintf(2,1,"请输入旧密码");
    Fyd12864ClearLine(4);
    for(i=0;i<ScreenInput.inputlen;i++) buf[i]='*';
    Fyd12864DispPrintf(3,1,buf);
    globalvar.U32Temp=atol((char *)ScreenInput.array);
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    TaskAdd(KEY_OK, DispJudgePwd,NULL);
}

//判断两次密码是否一致
void JudgePwdTwice(void)
{
    uint32_t pwd;
    pwd = atol((char *)ScreenInput.array); 
    if(pwd != globalvar.afterdex)//第一次输入密码时，已经赋值
    {
        Fyd12864DispPrintf(3,1,"两次密码不一致");
        TaskMessage(10,DispOthMsg1);
    }
    else
    {
        globalvar.displayonce=1;
        DispUI = DispOK;
    }
}

//再次输入新密码
void InputNewPwdAgain(void)
{
    char i,tmpbuf[6]={0};    
    if(globalvar.displayonce==1)
    {
        memset(&ScreenInput,0,sizeof(Screeninput));
        globalvar.displayonce=0;
    }
    ScreenInput.inputmaxlen=6;
    NumKeyHandler();
    Fyd12864DispPrintf(1,1,"再次输入新密码:");
    for(i=0;i<ScreenInput.inputlen;i++) tmpbuf[i]='*';
        Fyd12864DispPrintf(2,1,"%s",tmpbuf);    
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, JudgePwdTwice,DispOthMsg1);
    }
}

//菜单密码设置
void DispSetMenuPwd(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    tmp_sysparas.menupwd=atol((char *)ScreenInput.array);    
    globalvar.paraindex=73;
    DispSet(6,sysparas.menupwd,"菜单密码设置");
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain,NULL);
    }
}

//质检密码设置
void DispSetQcPwd(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    tmp_sysparas.qcpwd=atol((char *)ScreenInput.array);  
    globalvar.paraindex=74;
    DispSet(6,sysparas.qcpwd,"质检密码设置");
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain,NULL);
    }
}

//维护密码设置
void DispSetTestPwd(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    tmp_sysparas.testpwd=atol((char *)ScreenInput.array);  
    globalvar.paraindex=75;
    DispSet(6,sysparas.testpwd,"维护密码设置");
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain,NULL);
    }
}

//恢复出厂密码设置
void DispSetResetPwd(void)
{
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    tmp_sysparas.resetpwd=atol((char *)ScreenInput.array); 
    globalvar.paraindex=76;
    DispSet(6,sysparas.resetpwd,"恢复出厂密码设置");
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain,NULL);
    }
}

void DispSetonoffworkpwd(void)//huanghao 20161025
{
	tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    tmp_sysparas.onoffworkpwd=atol((char *)ScreenInput.array); 
    globalvar.paraindex=77;
    DispSet(6,sysparas.onoffworkpwd,"上下班密码设置");
    TaskAdd(KEY_RET,DispOthMsg1 ,NULL);
    if(ScreenInput.inputlen>0)//防止直接按了确认键
    {
        TaskAdd(KEY_OK, InputNewPwdAgain,NULL);
    }
}
//其他信息设置1
void DispOthMsg1(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.菜单密码设置");
    Fyd12864DispPrintf(2,1,"2.质检密码设置");
    Fyd12864DispPrintf(3,1,"3.维护密码设置");
    Fyd12864DispPrintf(4,1,"4.恢复出厂密码");
    if(globalvar.KeyValue==KEY_1)
    {
        globalvar.U8Temp=1;
        TaskAdd(KEY_1, DispInputOldPwd,NULL);
    }
    if(globalvar.KeyValue==KEY_2)
    {
        globalvar.U8Temp=2;
        TaskAdd(KEY_2, DispInputOldPwd,NULL);
    }
    if(globalvar.KeyValue==KEY_3)
    {
        globalvar.U8Temp=3;
        TaskAdd(KEY_3, DispInputOldPwd,NULL);
    }
    if(globalvar.KeyValue==KEY_4)
    {
        globalvar.U8Temp=4;
        TaskAdd(KEY_4, DispInputOldPwd,NULL);
    }

    //TaskAdd(KEY_UP,DispOthMsg2 ,NULL);
    TaskAdd(KEY_DOWN,DispOthMsg2 ,NULL);
    TaskAdd(KEY_RET, DispMenu_3,NULL);
}

//设置加密版
void SetPwdBoard(void)
{
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        sysparas=tmp_sysparas; 
        if(tmp_sysparas.usepwdboard==0)
        {
           if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
           {
               SetSuccess();
           }else
           {
               SetFault();
           }
        }else
        {
            LockDataStore();
        }
    } 
    if(DispNextUI!=NULL)
    {
        TaskMessage(10, DispNextUI);
    }      
}

//加密板设置
void DispSetPwdBoard(void)
{
	uint8_t tmp;
	if(sysparas.usepwdboard==1 && SI_INDATE!=0)			//使用加密板但出于正常模式
	{
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
        Fyd12864DispPrintf(2,1,"加气机Normal模式");
		TaskAdd(KEY_RET, DispOthMsg2,NULL);
	}
	else
	{
		tmp_sysparas=sysparas;
		ScreenInput.inputmaxlen=1;
		tmp=atol((char *)ScreenInput.array);   
		if((tmp==0)||(tmp==1))
		{
			tmp_sysparas.usepwdboard=tmp;
            globalvar.paraindex=79;
		}
		else if(ScreenInput.inputlen>0)
		{
			Fyd12864DispPrintf(4,1,"输入错误");
			NumKeyHandler(); 
			return;
		}
		DispSet(5,sysparas.usepwdboard,"加密板设置");
		Fyd12864DispPrintf(2,1,"0.不使用加密板");
		Fyd12864DispPrintf(3,1,"1.使用加密板");
		TaskAdd(KEY_RET, DispOthMsg2,NULL);
		TaskAdd(KEY_OK, SetPwdBoard,DispOthMsg2);
	}
}

//密码输入界面
void DispInputPwd(char* pwdbuf,fun funname)
{
    char     buf[6]={0},i;    
    ScreenInput.inputmaxlen=6;
    NumKeyHandler(); 
    Fyd12864ClearLine(1);          //清第一行。
    Fyd12864DispPrintf(2,1,pwdbuf);//显示“请输入菜单密码”。
    Fyd12864ClearLine(4);          //清第四行。
    for(i=0;i<ScreenInput.inputlen;i++) buf[i]='*';
    Fyd12864DispPrintf(3,1,buf);
    globalvar.U32Temp=atol((char *)ScreenInput.array);
    TaskAdd(KEY_RET,DispFirst,NULL);
    TaskAdd(KEY_OK, funname,NULL); 
}

//菜单密码验证
void DispJudgeMenuPwd(void)
{
    if(globalvar.U32Temp!=sysparas.menupwd)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,DispInputMenuPwd ,NULL);
        TaskAdd(KEY_RET,DispInputMenuPwd ,NULL);
        TaskMessage(10,DispInputMenuPwd);
    }
    else
    {
        DispUI=DispMenu_1;
    }
}

//输入菜单密码
void DispInputMenuPwd(void)
{
    DispInputPwd("请输入菜单密码",DispJudgeMenuPwd);    
}

//质检密码验证
void DispJudgeQcPwd(void)
{
    if(globalvar.U32Temp!=sysparas.qcpwd)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,DispInputQcPwd ,NULL);
        TaskAdd(KEY_RET,DispInputQcPwd ,NULL);
        TaskMessage(10,DispInputQcPwd);
    }
    else
    {
        DispUI=DispQcPara;
    }
}

//输入质检密码
void DispInputQcPwd(void)
{
    DispInputPwd("请输入质检密码",DispJudgeQcPwd);    
}


//维护密码验证
void DispJudgeTestPwd(void)
{
    if(globalvar.U32Temp!=sysparas.testpwd)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,DispInputTestPwd ,NULL);
        TaskAdd(KEY_RET,DispInputTestPwd ,NULL);
        TaskMessage(10,DispInputTestPwd);
    }
    else
    {
        DispUI=DeviceTestMenu;
    }
}

//输入维护密码
void DispInputTestPwd(void)
{
    DispInputPwd("请输入维护密码",DispJudgeTestPwd);
}

//恢复出厂设置密码验证
void DispJudgeResetPwd(void)
{
    if((globalvar.U32Temp!=sysparas.resetpwd)&&(globalvar.U32Temp!=760103))
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,DispInputResetPwd ,NULL);
        TaskAdd(KEY_RET,DispInputResetPwd ,NULL);
        TaskMessage(10,DispInputResetPwd);
    }
    else
    {
        DispUI=DispUnitQcReset;
    }
}

//输入恢复出厂密码
void DispInputResetPwd(void)
{
    DispInputPwd("请输入恢复密码",DispJudgeResetPwd);
}

//恢复出厂设置
void DispFactoryReset(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"  恢复出厂设置");
    Fyd12864DispPrintf(2,1,"【确认】  继续");
    Fyd12864DispPrintf(3,1,"【返回】  取消");
    Fyd12864ClearLine(4);
    FactoryReset();
    if(globalvar.KeyValue == KEY_OK)
		ScreenInput.inputlen = 1;
    TaskAdd(KEY_OK, DispOK,SystemReStart);
    TaskAdd(KEY_RET, DispUnitQcReset,NULL);
}


//其他信息设置2
void DispOthMsg2(void)
{
	/*
    InputInit();
    Fyd12864DispPrintf(1,1,"1.加密版设置");
    Fyd12864ClearLine(2);
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1, DispSetPwdBoard,NULL);         
    TaskAdd(KEY_UP,DispOthMsg1 ,NULL);
    TaskAdd(KEY_RET, DispMenu_3,NULL);
	*/
	InputInit();
	Fyd12864DispPrintf(1,1,"1.上下班密码设置");
    Fyd12864DispPrintf(2,1,"2.加密版设置");

    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
	if(globalvar.KeyValue==KEY_1)
    {
        globalvar.U8Temp=5;
        TaskAdd(KEY_1, DispInputOldPwd,NULL);
    }
    TaskAdd(KEY_2, DispSetPwdBoard,NULL);         
    TaskAdd(KEY_UP,DispOthMsg1 ,NULL);
    TaskAdd(KEY_RET, DispMenu_3,NULL);
}

//密度设置
void DispSetDensty(void)
{   
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*10000);
    tmp_sysparas.densty=atol(buf);
    globalvar.paraindex=30;
	if(tmp_sysparas.densty > 0 && tmp_sysparas.densty < 10000)
	{
		TaskAdd(KEY_OK,DispOK,DispQcPara);
	}
	else if((ScreenInput.inputlen>5))
	{
		Fyd12864DispPrintf(4,1,"密度超限");
        NumKeyHandler(); 
		return;
	}
    DispSet(4,sysparas.densty,"密度设置");   
    TaskAdd(KEY_RET, DispQcPara,NULL);
}

//修正系数设置
void DispSetCoeff(void)
{   
    char buf[16]={0};
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=6;
    sprintf(buf,"%f",atof((char *)ScreenInput.array)*10000);
    tmp_sysparas.coeff=atol(buf);
	if(tmp_sysparas.coeff > 0 && tmp_sysparas.coeff <= 15000)
	{
		TaskAdd(KEY_OK, DispOK,DispQcPara);
	}
	else if((ScreenInput.inputlen>5))
	{
		
		Fyd12864DispPrintf(4,1,"修正系数超限");
        NumKeyHandler(); 
		return;
	}
    globalvar.paraindex=31;
    DispSet(4,sysparas.coeff,"修正系数设置");   
    TaskAdd(KEY_RET, DispQcPara,NULL);
    TaskAdd(KEY_OK, DispOK,DispQcPara);
}

//质检参数设置
void DispQcPara(void)
{
	if(sysparas.usepwdboard==1 && SI_INDATE!=0)			//使用加密板但出于正常模式
	{
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
        Fyd12864DispPrintf(2,1,"加气机Normal模式");
		TaskAdd(KEY_RET, DispFirst,NULL);
	}
	else if(sysparas.usepwdboard==0 || (sysparas.usepwdboard==1 && LockDataCompare()))
	{													//不使用加密板或使用加密板且数据正确
		InputInit();
		Fyd12864DispPrintf(1,1,"1.密度设置");
		Fyd12864DispPrintf(2,1,"2.修正系数设置");
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
		TaskAdd(KEY_1, DispSetDensty,NULL);
		TaskAdd(KEY_2, DispSetCoeff,NULL);
		TaskAdd(KEY_RET, DispFirst,NULL);
	}
	else
	{
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		Fyd12864ClearLine(4);
        Fyd12864DispPrintf(2,1,"加密板不匹配！！");
		TaskAdd(KEY_RET, DispFirst,NULL);
	}
}

//擦除主存芯片
void EraseChipA(void)
{
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        SSTEraseChip(0);//擦除两次，确保正确
        SSTEraseChip(0);
        SetSuccess();          
    }
    TaskMessage(10,DispEraseChip);  
}

//擦除副存芯片
void EraseChipB(void)
{
    if(globalvar.displayonce==1)
    {
        globalvar.displayonce=0;
        SSTEraseChip(1);//擦除两次，确保正确
        SSTEraseChip(1);
        SetSuccess();         
    }
    TaskMessage(10,DispEraseChip);  
}

//擦除主存芯片询问
void DispEraseChipA(void)
{
    Fyd12864DispPrintf(1,1,"  确认擦除主存?");
    Fyd12864DispPrintf(2,1,"【确认】  继续");
    Fyd12864DispPrintf(3,1,"【返回】  取消");
    Fyd12864ClearLine(4);
    TaskAdd(KEY_OK,EraseChipA, NULL);
    TaskAdd(KEY_RET,DispEraseChip ,NULL);
}

//擦除副存芯片询问
void DispEraseChipB(void)
{
    Fyd12864DispPrintf(1,1,"  确认擦除副存?");
    Fyd12864DispPrintf(2,1,"【确认】  继续");
    Fyd12864DispPrintf(3,1,"【返回】  取消");
    Fyd12864ClearLine(4);
    TaskAdd(KEY_OK,EraseChipB, NULL);
    TaskAdd(KEY_RET,DispEraseChip ,NULL);
}

//Flash存储芯片擦除
void DispEraseChip(void)
{
    globalvar.displayonce=1;
    Fyd12864DispPrintf(1,1,"1.擦除主存芯片");
    Fyd12864DispPrintf(2,1,"2.擦除副存芯片");
    Fyd12864ClearLine(3);
    Fyd12864ClearLine(4);
    TaskAdd(KEY_1,DispEraseChipA ,NULL);
    TaskAdd(KEY_2,DispEraseChipB ,NULL);
    TaskAdd(KEY_RET, DispUnitQcReset,NULL);
}

// 计量单位，质检模式，恢复出厂设置
void DispUnitQcReset(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.计量单位设置");
    Fyd12864DispPrintf(2,1,"2.质检模式设置");
    Fyd12864DispPrintf(3,1,"3.存储芯片擦除");
    Fyd12864DispPrintf(4,1,"0.恢复出厂设置");  
    TaskAdd(KEY_1, DispSetUnit,NULL);
    TaskAdd(KEY_2, DispSetQCMode,NULL);
    TaskAdd(KEY_3, DispEraseChip,NULL);
    TaskAdd(KEY_0, DispFactoryReset,NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

void CirculateFuelingSet(void)
{
	uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
	tmp=ScreenInput.array[0];   
    if((tmp=='0')||(tmp=='1'))
    {
		globalvar.circulatefue = atol((char *)ScreenInput.array);
		DispUI = DispFirst;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,globalvar.circulatefue,"循环加气设置");
    Fyd12864DispPrintf(2,1,"0.正常加气");
    Fyd12864DispPrintf(3,1,"1.循环加气");
    TaskAdd(KEY_RET, DispMenu_3,NULL);
    TaskAdd(KEY_OK, DispOK,DispMenu_3);
}


//菜单界面3显示
void DispMenu_3(void)
{
    InputInit();
    Fyd12864DispPrintf(1,1,"1.版本编号设置");
    Fyd12864DispPrintf(2,1,"2.密码信息设置");
	Fyd12864DispPrintf(3,1,"3.模拟加气设置");
    Fyd12864DispPrintf(4,1,"4.循环加气设置");
 
    TaskAdd(KEY_1, DispVerNum1,NULL);
    TaskAdd(KEY_2, DispOthMsg1,NULL);
	TaskAdd(KEY_3, SimulatorSet, NULL);
    TaskAdd(KEY_4, CirculateFuelingSet,NULL);
    TaskAdd(KEY_UP, DispMenu_2,NULL);
    //TaskAdd(KEY_DOWN, DispMenu_1,NULL);
    TaskAdd(KEY_RET, DispFirst,NULL);
}

//上班。
void OnWork(void)
{
    uint8_t *tmp;
    if(sysparas.offonwork==1)
    {
         Fyd12864ClearLine(1);
         Fyd12864ClearLine(3);
         Fyd12864ClearLine(4);
         Fyd12864DispPrintf(2,1,"不能重复上班");
         TaskAdd(KEY_RET, DispFirst,NULL);
    }
    else
    {
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(2);
		Fyd12864ClearLine(4);
    	
		sysparas.shiftransnum++;
		shiftrecordinfo.classid=sysparas.shiftransnum;          //上下班记录流水号，4字节。
		shiftrecordinfo.transactiontype=0;                      //交易类型，1字节。
		sysparas.shiftnum++;
		if(sysparas.shiftnum>sysparas.shiftmaxnum)              //判断是否大于最大班号。
		{
			sysparas.shiftnum=1;
		}
		shiftrecordinfo.classnum=sysparas.shiftnum;             //班号，1字节。
		if(sysparas.offlinemode == 1)
		{
			shiftrecordinfo.cardstaffnum[0] = 0;
			shiftrecordinfo.cardstaffnum[1] = 0;
			shiftrecordinfo.cardstaffnum[2] = 0;
			shiftrecordinfo.cardstaffnum[3] = 0;
			shiftrecordinfo.cardstaffnum[4] = 0;
			shiftrecordinfo.cardstaffnum[5] = 0;
			shiftrecordinfo.cardstaffnum[6] = 0;
			shiftrecordinfo.cardstaffnum[7] = 0;
		}
		else
		{
			memcpy(&shiftrecordinfo.cardstaffnum[0],&m1card.cardnum[0],8);	//员工卡号，BCD码。
		}
				
		shiftrecordinfo.classstarttime[0]=0x20;                 //班开始时间，7字节，BCD码。
		memcpy(&shiftrecordinfo.classstarttime[1],&time.year,6);
		memcpy(&sysparas.shiftstarttime[0],&time.year,6);
		  
		memset(&shiftrecordinfo.classendtime[0],0,7);           //班结束时间，7字节，BCD码。
		memset(&sysparas.shiftendtime[0],0,6);                  
		
		shiftrecordinfo.allgunnum=sysparas.totvolume;           //枪气量累计，4字节。
		shiftrecordinfo.allgunmoney=sysparas.totmoney;          //枪金额累计,4字节。
		shiftrecordinfo.classgasnum=0;                          //班气量，4字节。
		shiftrecordinfo.classgasmoney=0;                        //班金额,4字节。
		shiftrecordinfo.classtimes=0;                           //班加气次数，4字节。
		sysparas.shiftfueltimes=0;
		shiftrecordinfo.devicetype=sysparas.devicetype;         //气品代码，1字节，0-CNG，1-LNG。
		shiftrecordinfo.measuretype=sysparas.unit;              //计量单位，1字节，0-标方，1-Kg。
		shiftrecordinfo.classiding=sysparas.shiftransnum;       //上班记录流水号。
	
		sysparas.shiftotvol=0;                                  //班气量清0。
		sysparas.shiftotmon=0;                                  //班金额清零。
		sysparas.shiftstartvol=sysparas.totvolume;              //班开始枪累。
		sysparas.shiftstartmon=sysparas.totmoney;               //班开始金额。
		
		tmp=(uint8_t *)(&shiftrecordinfo.classid);
		shiftrecordinfo.crc=CRC16Calculate(tmp, sizeof(shiftrecordinfo)-2);
		if(SaveFuelRecord(1))          //存班流水。
		{
			sysparas.offonwork=1;       //上班标志。
			globalvar.workstate=StaIdle;//上班成功使其成为空闲状态。
			if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
			{
				Fyd12864ClearLine(1);
				Fyd12864DispPrintf(2,1,"上班成功！"); 
				Fyd12864ClearLine(3);
				Fyd12864ClearLine(4);
			}
			else
			{							//一旦出错主界面会报错
//				sysparas.offonwork = 0;  //如果失败就强制置0让其重新上班， 
				Fyd12864ClearLine(1);
				Fyd12864DispPrintf(2,1,"上班失败！");
				Fyd12864ClearLine(3);
				Fyd12864DispPrintf(4,1,"存储参数失败！");
			}
					
		}
		else
		{
			Fyd12864ClearLine(1);
			Fyd12864DispPrintf(2,1,"上班失败！");
			Fyd12864ClearLine(3);
			Fyd12864DispPrintf(4,1,"存储班流水失败！"); 
		}	
    }
    DispUI=Diswork;  
}

//下班。
void OffWork(void)
{
    uint8_t *tmp;
    if(sysparas.offonwork==0)
    {
         Fyd12864ClearLine(1);
         Fyd12864ClearLine(3);
         Fyd12864ClearLine(4);
         Fyd12864DispPrintf(2,1,"不能重复下班");
         TaskAdd(KEY_RET, DispFirst,NULL);
    }
    else
    {
		Fyd12864ClearLine(1);
		Fyd12864ClearLine(3);
		
		sysparas.shiftransnum++;
		shiftrecordinfo.classid=sysparas.shiftransnum;          //上下班记录流水号，4字节。
		shiftrecordinfo.transactiontype=1;                      //交易类型，1字节。
		shiftrecordinfo.classnum=sysparas.shiftnum;             //班号，1字节。
			
		if(sysparas.offlinemode == 1)
		{
			shiftrecordinfo.cardstaffnum[0] = 0;
			shiftrecordinfo.cardstaffnum[1] = 0;
			shiftrecordinfo.cardstaffnum[2] = 0;
			shiftrecordinfo.cardstaffnum[3] = 0;
			shiftrecordinfo.cardstaffnum[4] = 0;
			shiftrecordinfo.cardstaffnum[5] = 0;
			shiftrecordinfo.cardstaffnum[6] = 0;
			shiftrecordinfo.cardstaffnum[7] = 0;
		}
		else
		{
			memcpy(&shiftrecordinfo.cardstaffnum[0],&m1card.cardnum[0],8);//员工卡号，BCD码。
		}
				  
		shiftrecordinfo.classstarttime[0]=0x20;                 //班开始时间，7字节，BCD码。
		memcpy(&shiftrecordinfo.classstarttime[1],&sysparas.shiftstarttime[0],6);
		
		shiftrecordinfo.classendtime[0]=0x20;                   //班结束时间，7字节，BCD码。
		memcpy(&shiftrecordinfo.classendtime[1],&time.year,6);
		memcpy(&sysparas.shiftendtime[1],&time.year,6);
		
		shiftrecordinfo.allgunnum=sysparas.totvolume;           //枪气量累计，4字节。
		shiftrecordinfo.allgunmoney=sysparas.totmoney;          //枪金额累计,4字节。
		shiftrecordinfo.classgasnum=sysparas.shiftotvol;        //班气量，4字节。
		shiftrecordinfo.classgasmoney=sysparas.shiftotmon;      //班金额,4字节。
		shiftrecordinfo.classtimes=sysparas.shiftfueltimes;     //班加气次数，4字节。
		shiftrecordinfo.devicetype=sysparas.devicetype;         //气品代码，1字节，0-CNG，1-LNG。
		shiftrecordinfo.measuretype=sysparas.unit;              //计量单位，1字节，0-标方，1-Kg。
		shiftrecordinfo.classiding=(sysparas.shiftransnum-1);   //上班记录流水号。
		
		tmp=(uint8_t *)(&shiftrecordinfo.classid);
		shiftrecordinfo.crc=CRC16Calculate(tmp, sizeof(shiftrecordinfo)-2);
		if(SaveFuelRecord(1))//存班流水。
		{
			globalvar.workstate=StaOffDuty;//加气机状态，下班。
			sysparas.offonwork=0;          //下班标志。
			if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
			{
				Fyd12864ClearLine(1);
				Fyd12864DispPrintf(2,1,"下班成功！");
				Fyd12864ClearLine(3);
				Fyd12864ClearLine(4);
				PrintShifid();
			}
			else
			{
				Fyd12864ClearLine(1);
				Fyd12864DispPrintf(2,1,"下班失败！");
				Fyd12864ClearLine(3);
				Fyd12864DispPrintf(4,1,"存储参数失败！");
			}
		}
		else
		{
			Fyd12864ClearLine(1);
			Fyd12864DispPrintf(2,1,"下班失败！");
			Fyd12864ClearLine(3);
			Fyd12864DispPrintf(4,1,"存储班流水失败！");
		}	
	}         
	DispUI=Diswork;    
}

void Diswork(void)
{
    TaskMessage(10, DispFirst);
}


//设置模拟加气。
void SimulatorSet(void)
{
	uint8_t tmp;
    tmp_sysparas=sysparas;
    ScreenInput.inputmaxlen=1;
	tmp=ScreenInput.array[0];   
    if((tmp=='0')||(tmp=='1'))
    {
		sysparas.simulation = atol((char *)ScreenInput.array);
        globalvar.paraindex=84;
    }
    else if(ScreenInput.inputlen>0)
    {
        Fyd12864DispPrintf(4,1,"输入错误");
        NumKeyHandler(); 
        return;
    }
    DispSet(5,sysparas.simulation,"模拟加气设置");
    Fyd12864DispPrintf(2,1,"0.正常加气");
    Fyd12864DispPrintf(3,1,"1.模拟加气");
    TaskAdd(KEY_RET, DispMenu_3,NULL);
    TaskAdd(KEY_OK, DispOK,DispMenu_3);
}
	
void WriteICcard(void)
{
	m1card.cardstate = 1;
	Mt318WriteM1Card(KEYB, m1card.keyB, 10, &m1card.cardstate);
}

void OnWorkWithPwd(void)
{
	DispInputPwd("请输入上班密码",DispJudgeOnWorkPwd);
}
void DispJudgeOnWorkPwd(void)
{
    if(globalvar.U32Temp!=sysparas.onoffworkpwd)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,OnWorkWithPwd, NULL);
        TaskAdd(KEY_RET,OnWorkWithPwd, NULL);
    }
    else
    {
        DispUI=OnWork;
    }
}

void OffWorkWithPwd(void)
{
	DispInputPwd("请输入下班密码",DispJudgeOffWorkPwd);
}
void DispJudgeOffWorkPwd(void)
{
    if(globalvar.U32Temp!=sysparas.onoffworkpwd)
    {
        Fyd12864DispPrintf(4,1,"密码错误");
        TaskAdd(KEY_CLR,OffWorkWithPwd, NULL);
        TaskAdd(KEY_RET,OffWorkWithPwd, NULL);
    }
    else
    {
        DispUI=OffWork;
    }
}
