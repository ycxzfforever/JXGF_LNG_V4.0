#include "kernel.h"

Reply51 UpLoad51Data;
Reply52 UpLoad52Data; 
Reply53 UpLoad53Data;
Reply54 UpLoad54Data;
Reply55 UpLoad55Data;
Reply56 UpLoad56Data; 
Reply57 UpLoad57Data;
Reply58 UpLoad58Data;
Reply59 UpLoad59Data; 
Reply5A UpLoad5AData; 
Reply5B UpLoad5BData;
Reply5C UpLoad5CData;
Reply5D UpLoad5DData;
Reply5E UpLoad5EData;
Reply5F UpLoad5FData;
Receive55data receive55data;
uint8_t SendBack485Buf[BUFSIZE];
uint8_t RecBack485Buf[BUFSIZE];

BWInfo    bwinfo;
uint8_t   LoadSuccess = 0;   //下载数据是否完成。
uint16_t  allpart=0;         //后台申请下载数据总段数。

/********************************************************
**结构名称:BufReverse1(uint8_t *buf1,uint8_t num,uint8_t *buf2)
**结构功能:将需要buf1数据成员颠倒存入buf2中。
            *buf1-数组1，num-数组长度，buf2-数组2。

**创 建 者:杨传坤
**创建日期:2016-06-3

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void BufReverse1(uint8_t *buf1,uint8_t num,uint8_t *buf2)
{
    if(num==2)
    {
        buf2[0]=buf1[1];
        buf2[1]=buf1[0];
    }
   
    else if(num==4)
    {
        buf2[0]=buf1[3];
        buf2[1]=buf1[2];
        buf2[2]=buf1[1];
        buf2[3]=buf1[0];
    }
}

/********************************************************
**结构名称:BufReverse2(uint8_t *buf, uint8_t num)
**结构功能:将需要buf数据成员颠倒存入buf中。
           num-数组长度。

**创 建 者:杨传坤
**创建日期:2016-06-3

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void BufReverse2(uint8_t *buf, uint8_t num)
{
	uint8_t  temp;
	if(num == 2)
	{
		temp   = buf[0];
		buf[0] = buf[1];
		buf[1] = temp;
	}
	if(num == 4)
	{
		temp   = buf[0];
		buf[0] = buf[3];
		buf[3] = temp;
		temp   = buf[1];
		buf[1] = buf[2];
		buf[2] = temp;
	}
}

/********************************************************
**结构名称:StrReverse(uint8_t *buf,uint8_t num,uint8_t n)
**结构功能:将一串数据颠倒位置。
           buf-数据首地址；num-要转换的数据个数；n-转换数据的字节数。

**创 建 者:杨传坤
**创建日期:2016-06-3

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void StrReverse(uint8_t *buf,uint8_t num,uint8_t n)
{
    uint8_t i=0;
    if(n==2)
    {
        for(i=0;i<num;i++)
        {
            BufReverse2(buf,2);
            buf+=2;
        }
    }
    if(n==4)
    {
        for(i=0;i<num;i++)
        {
            BufReverse2(buf,4);
            buf+=4;
        }
    }
}
    
/********************************************************
**结构名称:ClearBuf(volatile uint8_t *buf,uint8_t num,uint16_t len)
**结构功能:清串口接收数组。*buf-数组，num-给数组赋的值，len-长度。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void ClearBuf(volatile uint8_t *buf,uint8_t num,uint16_t len)
{
    uint16_t i;
    for(i=0;i<len;i++)
    {
        buf[i]=num;
    }
}

/********************************************************
**结构名称:BackCommunicationHead(uint8_t cmd)
**结构功能:与后台通信，帧头。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/  
void BackCommunicationHead(uint8_t cmd)
{
    SendBack485Buf[0] = 0xBB;    			//起始符1。      
    SendBack485Buf[1] = 0xEE;    			//起始符2。    
    SendBack485Buf[2] = 0x00;    			//目标设备，  
    SendBack485Buf[3] = sysparas.gunnum;    //源设备?
    SendBack485Buf[4] = cmd;     			//命令。
}

/********************************************************
**结构名称:BackCommunicationEnd(uint16_t len)
**结构功能:与后台通信，帧尾。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void BackCommunicationEnd(uint16_t len)
{
	uint16_t CRC;
    SendBack485Buf[5] = len/256;      //消息体内容长度H。	
    SendBack485Buf[6] = len%256;      //消息体内容长度L。
	CRC= CRC16Calculate(&SendBack485Buf[2], len+5);
    SendBack485Buf[len+7]=CRC/256;
    SendBack485Buf[len+8]=CRC%256;
	SendBack485Buf[len+9] = 0xDD;     //截止1。
	SendBack485Buf[len+10] = 0xEE; 	  //截止2。
}

/********************************************************
**结构名称:SendBackCommunicationData(uint8_t cmd,uint16_t len)
**结构功能:向后台发送数据cmd-命令；len-消息体长度。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void SendBackCommunicationData(uint8_t cmd,uint16_t len)
{
    BackCommunicationHead(cmd);
    BackCommunicationEnd(len);
    UartSendStr(PCPort,&SendBack485Buf[0], len+11);
}

void GetRep51Data(void)
{
    UpLoad51Data.LCNG=sysparas.devicetype;                                                     //CNG/LNG标记。
    UpLoad51Data.status=globalvar.workstate;                                 //加气机工作状态。
    UpLoad51Data.price =realtime.price;                                      //交易价格。
    
    UpLoad51Data.classnum=sysparas.shiftnum;                                 //班号。
    memcpy(&UpLoad51Data.employeenum[0],&shiftrecordinfo.cardstaffnum[0],8); //员工号。
    memcpy(&UpLoad51Data.cardnum[0],&m1card.cardnum[0],8);                   //用户卡号。
    memcpy(&UpLoad51Data.carnum[0],&m1card.carnum[0],9);                     //车牌号。
    if(globalvar.workstate==StaIdle)
    {
        UpLoad51Data.curpressure=0;           								 //当前压力。
    
        UpLoad51Data.inflowrate=0;           								 //进液流量。
        UpLoad51Data.intemperature=0;        							     //进液温度。
        UpLoad51Data.inamount=0;              								 //进液质量。
        UpLoad51Data.indensity=0;             								 //进液密度。
        UpLoad51Data.ingain=0;                								 //进液增益。
        
        UpLoad51Data.gasnum=0;                								 //当前交易气量。
        UpLoad51Data.gasmoney=0;              								 //当前交易金额。
        
        UpLoad51Data.returnflowrate=0;        								 //回气流量。
        UpLoad51Data.returntemperature=0;     								 //回气温度。
        UpLoad51Data.returnamount=0;          								 //回气质量。
        UpLoad51Data.returndensity=0;         								 //回气密度。
        UpLoad51Data.returngain=0;            								 //回气增益。

    }else
    {
        UpLoad51Data.curpressure=realtime.cur_pressure;                 	 //当前压力。
        UpLoad51Data.inflowrate=realtime.cur_flowrate_l;                	 //进液流量。
        UpLoad51Data.intemperature=realtime.cur_temperature_l;          	 //进液温度。
        UpLoad51Data.inamount=realtime.cur_totalize_l;                  	 //进液质量。
        UpLoad51Data.indensity=realtime.cur_density_l;                  	 //进液密度。
        UpLoad51Data.ingain=realtime.cur_gain_l;                         	 //进液增益。
        if(globalvar.workstate==StaFuel)
        {
            UpLoad51Data.gasnum=fuelrecordinfo.volume;                       //当前交易气量。
            UpLoad51Data.gasmoney=fuelrecordinfo.money;                      //当前交易金额。
        }
        else
        {
            UpLoad51Data.gasnum=0;                      	 
            UpLoad51Data.gasmoney=0;
        }
        
        UpLoad51Data.returnflowrate=realtime.cur_flowrate_g;            	 //回气流量。
        UpLoad51Data.returntemperature=realtime.cur_temperature_g;      	 //回气温度。
        UpLoad51Data.returnamount=realtime.cur_totalize_g;              	 //回气质量。
        UpLoad51Data.returndensity=realtime.cur_density_g;              	 //回气密度。
        UpLoad51Data.returngain=realtime.cur_gain_g;                    	 //回气增益。
    }
    UpLoad51Data.systemalarm=syserr;                                    	 //系统报警。
    
}

/********************************************************
**结构名称:Receive51H()
**结构功能:回复51命令。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 

void Receive51H(void)
{    
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));                    //清发送数组。
    if(globalvar.workstate==StaFuel)                                    //加气机状态：加气。
    {
        UpLoad51H();
    }
    else if(globalvar.workstate==StaPay)                                //加气机状态：扣款。
    {
        if((m1card.debittimes!=0)&&(m1card.existflag==1))            		//申请扣款，10都未成功，则停止申请扣款。
        {
            UpLoad55H(0);
        } 
    }
    else if((globalvar.workstate==StaIdle)||(globalvar.workstate==StaOffDuty))                           
    {                
        if(abs(((RecBack485Buf[12] >> 4) - (time.minute >> 4))* 10 + (RecBack485Buf[12] & 0x0F) - (time.minute & 0x0F)) > 2 	
            || memcmp(&RecBack485Buf[8],&time.year,4) !=0)	            //如果后台时间和加气机时间相差2分钟，那么以后台的时间为准。
        {
            Ds3232SetTime(&RecBack485Buf[8]);
        }
        else if((RecBack485Buf[14]!=sysparas.blackver)&&(RecBack485Buf[14]!=0))            //基础黑名单版本号不等。0 表示基础黑名单不合法或没有
        {
            if(LoadSuccess==0)                       					//表示有数据需要下载。                    
            {
                UpLoad53Data.loadtype=1;             					//表示需要下载基础黑名单。
                UpLoad53H();                         					//加气机根据版本号判断,申请下载相应数据。
            }
            if(LoadSuccess==1)                       					//如果下载还没有完成，继续下载数据。
            {
                UpLoad54H();
            }
            if(LoadSuccess==2)                       					//如果下载完成。
            {
                sysparas.blackver=RecBack485Buf[14];
                LoadSuccess=0;
																		//将名单信息存入EE
                SaveEeprom(BLACK_INFO_ADDR,(uint8_t*)&bwinfo.version,sizeof(bwinfo)); 
                SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            }
        }
        else if((RecBack485Buf[15]!=sysparas.addbkver)&&(RecBack485Buf[15]!=0)) //新增黑名单版本号不等。0 表示新增黑名单不合法或没有
        {
            if(LoadSuccess==0)                       					//表示有数据需要下载。                    
            {
                UpLoad53Data.loadtype=2;             					//表示需要下载基础黑名单。
                UpLoad53H();                         					//加气机根据版本号判断,申请下载相应数据。
            }
            if(LoadSuccess==1)                       					//如果下载还没有完成，继续下载数据。
            {
                UpLoad54H();
            }
            if(LoadSuccess==2)                       					//如果下载完成。
            {
                sysparas.addbkver=RecBack485Buf[15];
                LoadSuccess=0;
																		//将名单信息存入EE
                SaveEeprom(BLACK_ADD_INFO_ADDR,(uint8_t*)&bwinfo.version,sizeof(bwinfo)); 
                SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            }
        }
        else if((RecBack485Buf[16]!=sysparas.delbkver)&&(RecBack485Buf[16]!=0))                //新增黑名单版本号不等。0 表示新删黑名单不合法或没有
        {
            if(LoadSuccess==0)                       					//表示有数据需要下载。                    
            {
                UpLoad53Data.loadtype=3;             					//表示需要下载基础黑名单。
                UpLoad53H();                         					//加气机根据版本号判断,申请下载相应数据。
            }
            if(LoadSuccess==1)                       					//如果下载还没有完成，继续下载数据。
            {
                UpLoad54H();
            }
            if(LoadSuccess==2)                       					//如果下载完成。
            {
                sysparas.delbkver=RecBack485Buf[16];
                LoadSuccess=0;
																		//将名单信息存入EE
                SaveEeprom(BLACK_DEL_INFO_ADDR,(uint8_t*)&bwinfo.version,sizeof(bwinfo)); 
                SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            }
        }
        else if((RecBack485Buf[17]!=sysparas.whitever)&&(RecBack485Buf[17]!=0))                //白名单版本号不等。0 表示白名单不合法或没有
        {
            if(LoadSuccess==0)                       					//表示有数据需要下载。                    
            {
                UpLoad53Data.loadtype=4;             					//表示需要下载基础黑名单。
                UpLoad53H();                         					//加气机根据版本号判断,申请下载相应数据。
            }
            if(LoadSuccess==1)                       					//如果下载还没有完成，继续下载数据。
            {
                UpLoad54H();
            }
            if(LoadSuccess==2)                      					//如果下载完成。
            {
                sysparas.whitever=RecBack485Buf[17];
                LoadSuccess=0;
                SaveEeprom(WHITE_INFO_ADDR,(uint8_t*)&bwinfo.version,sizeof(bwinfo)); //将名单信息存入EE
                SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            }
        }
        else if((RecBack485Buf[18]!=sysparas.pricever)&&(RecBack485Buf[18]!=0))                //单价版本号不等。0 表示单价版本不合法或没有.
        {
            UpLoad5EH();
        }
        else if((RecBack485Buf[19]!=sysparas.lockver)&&(RecBack485Buf[19]!=0)) //判断密钥版本号。
        {
            if(LoadSuccess==0)
            {
                UpLoad53Data.loadtype=6;
                UpLoad53H();                         					//加气机根据版本号判断,申请下载相应数据。
            }
            if(LoadSuccess==2)
            {
                sysparas.lockver=RecBack485Buf[19];
                LoadSuccess=0;
                SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));              
            } 
        }
        else if(sysparas.transnum>sysparas.uptransnum)//判断是否有未上传流水。
        {
           UpLoad52H();
        }
        else if(m1card.greycard==1)  //ycx20161201流水上传完了后才申请解灰。
        {
            UpLoad55H(1);
        }
        else if(m1card.checkflag==1)    //ycx20161201流水上传完了后才申请验卡。
        {
            UpLoad5CH();
        }
        else if(sysparas.shiftransnum>sysparas.shiftuptransnum)//判断是否有未上传班流水。
        {
            UpLoad5AH();
        }
        else if(sysparas.modnum>sysparas.upmodnum)//判断是否有未上传修改流水号。
        {
            UpLoad5BH();
        }
        else                                                   			//51H空闲-无事可做时上传的51H数据。
        {
            UpLoad51H(); 
        }
    }
    else                                                   				//51H空闲-无事可做时上传的51H数据。
    {
        UpLoad51H();
    }
}
/********************************************************
**结构名称:UpLoad51H(void)
**结构功能:52H命令,加气机上传交易记录。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad51H(void)
{ 
    GetRep51Data();                            
    memcpy(&SendBack485Buf[7],&UpLoad51Data.LCNG,sizeof(Reply51));
    SendBackCommunicationData(0x51,sizeof(Reply51));
}

/********************************************************
**结构名称:UpLoad(void)
**结构功能:52H命令,加气机上传交易记录。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad52H(void)
{ 
    if(globalvar.uptransnum==sysparas.uptransnum)
    {
        globalvar.UploadIdCount++;
        if(globalvar.UploadIdCount>10)//上传10次不成功，跳过此笔流水
        {
            globalvar.UploadIdCount=0;
            sysparas.uptransnum++;
            SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            return;
        }        
    } 
    globalvar.uptransnum=sysparas.uptransnum;
    fuelrecordinfo_tmp=fuelrecordinfo;
    if(ReadFuelRecord(sysparas.uptransnum,0))
    {
       memcpy(&SendBack485Buf[7],&fuelrecordinfo.transnum,sizeof(fuelrecordinfo)-2);
       fuelrecordinfo=fuelrecordinfo_tmp;
       SendBackCommunicationData(0x52,sizeof(fuelrecordinfo)-2);
    } 
}

/********************************************************
**结构名称:Receive52H()
**结构功能:回复52H命令,后台系统回复加气机上传交易记录。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive52H(void)
{
    
    if(RecBack485Buf[7]==0)       //上传失败。
    {
        globalvar.UploadIdCount++;          //满十次之后，是否清零？？？？      
    }
    else if(RecBack485Buf[7]==1)  //上传成功。
    {
		globalvar.UploadIdCount=0;        
        if(sysparas.uptransnum<sysparas.transnum)
		{
			 sysparas.uptransnum++;
             SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
		}        
    }
}

/********************************************************
**结构名称:UpLoad53H()
**结构功能:53H命令，加气机根据版本号判断是否需要下载相应数据。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad53H(void)
{
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));        //发送数组清零。
    memcpy(&SendBack485Buf[7],&UpLoad53Data.loadtype,7);
    SendBackCommunicationData(0x53,7);
}

/********************************************************
**结构名称:Receive53H()
**结构功能:回复53命令。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive53H(void)
{
    uint32_t len=0;
    if(RecBack485Buf[7]==6)
    {
        if(SaveEeprom(LOCKVER_ADDR,&RecBack485Buf[12],16));//存储密钥。
        {
           LoadSuccess=2;
        } 
    }
    else
    {
        BufReverse2(&RecBack485Buf[8], 4);       			//大小端调换。
        memcpy(&len,&RecBack485Buf[8],4);        			//得到数据长度。
        allpart=len ; 					         			//得到段数（名单个数非字节数）。
        
        UpLoad54Data.loadtype = RecBack485Buf[7];			//要下载的数据。
        UpLoad54Data.partoffset=0;               			//已下载卡号个数。  
       
        UpLoad54Data.partnum = 20;        		 			//申请下载卡号个数。
        memcpy(&SendBack485Buf[7],&UpLoad54Data.loadtype,4); 
        SendBackCommunicationData(0x54,4);
    }
    
}

/********************************************************
**结构名称:UpLoad54H()
**结构功能:54H命令,加气机申请下载数据内容。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad54H(void)
{
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));        //发送数组清零。
    memcpy(&SendBack485Buf[7],&UpLoad54Data.loadtype,4); 
    SendBackCommunicationData(0x54,4);
}

/********************************************************
**结构名称:Receive54H()
**结构功能:回复54命令。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive54H(void)
{
	uint16_t loaded; 						   //已下载段数。
	uint16_t loading;						   //正在下载的段数。
	uint32_t addr;
	uint8_t  src[160];
    
    BufReverse2(&RecBack485Buf[8], 2);         //大小端调换。
	memcpy(&loaded,&RecBack485Buf[8],2);
	loading=RecBack485Buf[10];
	memcpy(src,&RecBack485Buf[27],loading*8);
	CloseUartRcvIRQ(3);
	CloseUartRcvIRQ(0);
    switch(RecBack485Buf[7])
    {
		
        case 1:								   //存基础黑名单。
			addr=FlashBlkSADDR+loaded*FlashBlkLen;
            SaveFlash(0,addr,src, loading*8);
			SaveFlash(1,addr,src, loading*8);
            break;
        case 2:								   //存新增黑名单。
            addr=FlashAddBlkSADDR+loaded*FlashAddBlkLen;
            SaveFlash(0,addr,src, loading*8);
			SaveFlash(1,addr,src, loading*8);
            break;
        case 3:								   //存新删黑名单。
            addr=FlashDelBlkSADDR+loaded*FlashDelBlkLen;
            SaveFlash(0,addr,src, loading*8);
			SaveFlash(1,addr,src, loading*8);
            break;
        case 4:								   //存白名单。
            addr=FlashWhiteSADDR+loaded*FlashWhiteLen;
            SaveFlash(0,addr,src, loading*8);
			SaveFlash(1,addr,src, loading*8);
            break;
        default:
            break;
    }
	OpenUartRcvIRQ(PCPort);
	OpenUartRcvIRQ(CardPort);
	
    if((loaded+loading)<allpart)               //判断下载是否完毕。
    {
		UpLoad54Data.loadtype=RecBack485Buf[7];//需要下载的数据代码
		UpLoad54Data.partoffset=loaded+loading;//已下载卡号个数。
		UpLoad54Data.partnum=20;               //申请下载卡号个数。
        LoadSuccess=1;                     	   //下载未完成。
    }
    else                                   	   //下载完成。
    {										   //下载成功后将名单信息保存至结构体
	    memcpy(&bwinfo.version, &RecBack485Buf[11], sizeof(bwinfo));  
        LoadSuccess=2;
        if(RecBack485Buf[7]==1)
        {
            sysparas.blacknum=allpart;
        }else if(RecBack485Buf[7]==2)
        {
            sysparas.blackaddnum=allpart;
        }else if(RecBack485Buf[7]==3)
        {
            sysparas.blackdelnum=allpart;
        }else if(RecBack485Buf[7]==4)
        {
            sysparas.whitenum=allpart;
        }
        SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
    }
}

/********************************************************
**结构名称:UpLoad55H()
**结构功能:上传55H命令，加气机申请解灰。0：申请扣款1：申请解灰

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad55H(uint8_t type)
{
	UpLoad55Data.type = type;
    UpLoad55Data.cardtype=m1card.cardtype;						   //卡类型。
	memcpy(UpLoad55Data.cardnum, m1card.cardnum, 8);
    if(type==0)													   //申请扣款。
    {
        UpLoad55Data.gasnum=fuelrecordinfo.volume;        		   //当前加气气量。
        UpLoad55Data.money=fuelrecordinfo.money;    			   //当前加气金额。
        memcpy(UpLoad55Data.stationnum, sysparas.stationcode, 4);  //站编码。
        UpLoad55Data.gunnum = sysparas.gunnum;      			   //最后一次加气枪号。
        UpLoad55Data.gunnumber = sysparas.transnum; 			   //最后一次加气流水号。
    }else if(type==1)											   //申请解灰。
    {
        UpLoad55Data.gasnum=0;
        UpLoad55Data.money=0;
        memcpy(UpLoad55Data.stationnum, m1card.laststationcode, 4);
        UpLoad55Data.gunnum = m1card.gunnum;
        UpLoad55Data.gunnumber = m1card.transnum;
    }
	UpLoad55Data.cardover = m1card.cardbalance;					   //卡余额。
	
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));        	   //发送数组清零。
    memcpy(&SendBack485Buf[7],&UpLoad55Data.type,sizeof(Reply55)); //申请解灰或扣款数据。
    SendBackCommunicationData(0x55,sizeof(Reply55));
}

/********************************************************
**结构名称:Receive55H()
**结构功能:接收55H命令，后台系统下发解灰数据内容。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive55H(void)
{
    FuelRecordInfo fueltmp1,fueltmp2;
	Receive55data *ptr;
	ptr = (Receive55data *)(&RecBack485Buf[7]);
    m1card.debittimes=0;
    StrReverse((uint8_t *)&ptr->realmoney,4,4);
    StrReverse((uint8_t *)&ptr->price,1,2);
    StrReverse((uint8_t *)&ptr->volume,5,4);
    
	if(RecBack485Buf[8] == 1)										       //解灰
	{
		if(RecBack485Buf[7]==0)											   //无此卡灰记录
		{
			Fyd12864DispPrintf(3,1,"无此卡灰记录！");					   //未找到灰记录。
		}
		else if(RecBack485Buf[7]==1)
		{
			if(memcmp(&RecBack485Buf[9],&m1card.cardnum,8) == 0)		   //判断卡号是否正确
			{
                //块5
				m1card.lastoptmode = 3;                             	   //软件解灰 
                m1card.trademoney=ptr->realtrademoney;              	   //交易额。
                m1card.cardbalance=ptr->aftermoney;                 	   //余额。
                //块8
                m1card.sumtimes=ptr->totaltimes;                    	   //累计消费次数。
                m1card.summoney=ptr->totalmoney;                    	   //累计消费金额。
                m1card.sumvolme=ptr->totalvolume;                   	   //累计消费气量。
                //块10
                m1card.cardstate = 1;	                            	   //解灰
                
                fueltmp1=fuelrecordinfo;
                sysparas.transnum++;
                fueltmp2.transnum=sysparas.transnum;                       //流水号。
                fueltmp2.classnum=sysparas.shiftnum;                       //班号。
                														   //员工号。
                memcpy(&fueltmp2.employeenum[0],&shiftrecordinfo.cardstaffnum[0],8);
                fueltmp2.transactiontype=2;                                //补扣交易。
                memcpy(&fueltmp2.usernum[0],&m1card.cardnum[0],8);         //卡号。
                fueltmp2.cardtype=m1card.cardtype;                     	   //卡类型。
                memcpy(&fueltmp2.carnum[0],&m1card.carnum[0],9);       	   //车牌号。
                memcpy(&fueltmp2.stationnum[0],&sysparas.stationcode[0],4);//站编码。
				fueltmp2.starttime[0]=0x20;
				memcpy(&fueltmp2.starttime[1], &time.year, 6);       //加气开始时间。
				fueltmp2.endtime[0]=0x20;
				memcpy(&fueltmp2.endtime[1], &time.year, 6);         //加气结束时间。
                fueltmp2.price=ptr->price;                                 //交易价格。
                fueltmp2.volume=ptr->volume;                               //加气气量。
                fueltmp2.money=ptr->realmoney;                             //加气金额。
                fueltmp2.reallymoney=ptr->realtrademoney;                  //实际扣款金额。
                fueltmp2.beforebalance=ptr->beforemoney;                   //加气前余额。
                fueltmp2.afterbalance=ptr->aftermoney;                     //加气后余额。
                fueltmp2.allrechargetimes=m1card.sumchargetimes;           //累计充值次数。
                fueltmp2.allrechargemoney=m1card.sumchargemoney;           //累计充值金额。
                fueltmp2.allconsumptiontimes=ptr->totaltimes;              //累计消费次数。
                fueltmp2.allconsumptionnum=ptr->totalvolume;               //累计消费气量。
                fueltmp2.allconsumptionmoney=ptr->totalmoney;              //累计消费金额。
                fueltmp2.allgunnum=sysparas.totvolume;                     //枪气量累计。
                fueltmp2.allgunmoney=sysparas.totmoney;                    //枪金额累计。
                fueltmp2.beforepressure=0;                                 //加气前压力。
                fueltmp2.afterpressure=0;                                  //加气后压力。
                fueltmp2.density=sysparas.densty;                                        //密度。
                fueltmp2.curnum=sysparas.coeff;                            //当量。
                fueltmp2.measuretype=sysparas.unit;                        //计量单位，0-标方，1-Kg。
                fueltmp2.devicetype=sysparas.devicetype;                                       //气品代码。
                fueltmp2.stopreason=0xFF;                                     //停机原因。
                fueltmp2.jhid=ptr->removegrey;                             //解灰相关流水内码。
                fueltmp2.classid=sysparas.shiftransnum;                    //班流水号。 
                fueltmp2.fuelmode=sysparas.fuelmode;
                fueltmp2.crc=CRC16Calculate((uint8_t *)(&fueltmp2.transnum),sizeof(FuelRecordInfo)-2);
                
                StrReverse((uint8_t *)&m1card.trademoney,2,4);         	   //块5调换。
                StrReverse((uint8_t *)&m1card.sumtimes,3,4);               //块8调换。
                StrReverse((uint8_t *)&m1card.transnum,1,4);           	   //块10调换。
                 														   //将块5备份到块6。
                Mt318WriteM1Card(KEYB,&m1card.keyB[0], 6, &m1card.balancecrc);   
				if(Mt318WriteM1Card(KEYB,&m1card.keyB[0], 5, (uint8_t *)&m1card.balancecrc))
				{	
                    if(Mt318WriteM1Card(KEYB,&m1card.keyB[0], 8,(uint8_t *)&m1card.sumtimes))
                    {
                        if(Mt318WriteM1Card(KEYB,&m1card.keyB[0],10,(uint8_t *)&m1card.cardstate))
                        {
                            m1card.greycard=0;
                            Fyd12864DispPrintf(2,1,"解灰成功！");
                            if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
                            {
                                fuelrecordinfo=fueltmp2;
                                if(SaveFuelRecord(0))
                                {
                                    if(sysparas.printmode==1)  //判断是否自动打印一次。
                                    {
                                        Printid();
                                    }
                                    Fyd12864DispPrintf(4,1,"存储流水成功"); 
                                    DispUI=DispFirst;
                                }
                                else
                                {
                                    Fyd12864DispPrintf(4,1,"存储流水失败");  
                                }
                                fuelrecordinfo=fueltmp1;
                            }
                        }
                        else
                        {
                            Fyd12864DispPrintf(2,1,"解灰失败！");
                        }
                    }
					else
					{
						Fyd12864DispPrintf(2,1,"解灰失败！");
					}
				}
				else
				{
					Fyd12864DispPrintf(2,1,"解灰失败！");
				}					
			}
			else
			{
				Fyd12864DispPrintf(2,1,"无此卡号！");
			}
		}
	}
	else if (RecBack485Buf[8] == 0)									//扣款
	{
        //块5
        m1card.lastoptmode = 1; 		                            //扣款
        memcpy(m1card.lasttradetime, &fuelrecordinfo.endtime[1], 6);//最后加气流水时间。
        m1card.trademoney = ptr->realtrademoney;                    //交易额。
        
        if(m1card.cardtype==1)									    //储蓄卡。
        {
            m1card.cardbalance -= ptr->realtrademoney;              //余额。
        }
        else if(m1card.cardtype==2)									//记账卡。
        {
            m1card.cardbalance += ptr->realtrademoney;              //余额。
        }
        
        //块8
        m1card.sumtimes++;                                          //总消费次数。
        m1card.summoney += ptr->realtrademoney;                     //总消费金额。
        m1card.sumvolme += fuelrecordinfo.volume;                   //总消费气量。
        //块10
        m1card.cardstate = 1;						                //解灰
        m1card.gunnum = sysparas.gunnum;                            //最后交易枪流号。
        m1card.transnum = fuelrecordinfo.transnum;                  //最后交易流水号。
        memcpy(m1card.laststationcode, sysparas.stationcode, 4);    //站编码。

        StrReverse((uint8_t *)&m1card.trademoney,2,4);              //块5调换。
        StrReverse((uint8_t *)&m1card.sumtimes,3,4);                //块8调换。
        StrReverse((uint8_t *)&m1card.transnum,1,4);                //块10调换。
        														    //将块5备份到块6。
        Mt318WriteM1Card(KEYB, &m1card.keyB[0], 6,&m1card.balancecrc);         
        if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 5, (uint8_t *)&m1card.balancecrc))         
        {
            if(Mt318WriteM1Card(KEYB, &m1card.keyB[0], 8, (uint8_t *)&m1card.sumtimes))  
            {
                if(Mt318WriteM1Card(KEYB,&m1card.keyB[0], 10,(uint8_t *)&m1card.cardstate))
                {
                    Fyd12864DispPrintf(2,1,"扣款成功!");
                    fuelrecordinfo.transactiontype=0;                   //交易类型。
                    fuelrecordinfo.reallymoney=ptr->realtrademoney;     //本次实际扣款金额。
                    
                    StrReverse((uint8_t *)&m1card.summoney,1,4);  
                    fuelrecordinfo.allconsumptionmoney=m1card.summoney; //累计消费金额。
                    StrReverse((uint8_t *)&m1card.cardbalance,1,4);  
                    fuelrecordinfo.afterbalance=m1card.cardbalance;     //加气后余额。
                    fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
                }
                else                                                    //写块10错。
                {
                    Fyd12864DispPrintf(2,1,"块10扣款失败!");
                    fuelrecordinfo.transactiontype=0x1;                 //交易类型。
                    fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
                }
            }
            else                                                        //写块8错。                                                    
            {
                Fyd12864DispPrintf(2,1,"块8 扣款失败!");
                fuelrecordinfo.transactiontype=0x1;                     //交易类型。
                fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
            }
        }
        else                                                            //写块5错。
        {
            Fyd12864DispPrintf(2,1,"块5 扣款失败!");
            fuelrecordinfo.transactiontype=0x1;                         //交易类型。
            fuelrecordinfo.crc=CRC16Calculate((uint8_t *)(&fuelrecordinfo.transnum),sizeof(FuelRecordInfo)-2);
        }	
	}
}


/********************************************************
**结构名称:UpLoad56H()
**结构功能:上传56H命令，加气机申请查询黑白名单。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad56H(uint8_t type)
{
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));                     //发送数组清零。
    memcpy(&UpLoad56Data.cardnum[0],&m1card.cardnum[0],8); 
    UpLoad56Data.seekname=type;
    memcpy(&SendBack485Buf[7],&UpLoad56Data.cardnum[0],sizeof(Reply56)); //申请查询黑白名单。
    SendBackCommunicationData(0x56,sizeof(Reply56));
}

/********************************************************
**结构名称:Receive56H()
**结构功能:接收56H命令，后台下发黑白查询结果。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive56H(void)
{
    if(RecBack485Buf[15]==0)
    {
        m1card.blackorwhite=1;//不是黑/白卡。
    }
    else if(RecBack485Buf[15]==1)
    {
        m1card.blackorwhite=2;//是黑/白卡。
    }    
}

/********************************************************
**结构名称:Receive57H()
**结构功能:接收57H命令，后台系统读加气机累计数。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive57H(void)
{
    UpLoad57Data.transactionid = sysparas.transnum;            				//交易流水号。
	UpLoad57Data.allgasnum     = sysparas.totvolume;           				//加气气量总累计。
    UpLoad57Data.allgasmoney   = sysparas.totmoney;            				//加气金额总累计。
    UpLoad57Data.allclassnum   = sysparas.shiftotvol;          				//班累计气量。
    UpLoad57Data.allclassmoney = sysparas.shiftotmon;          			    //班累计金额。
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));           				//发送数组清零。
    memcpy(&SendBack485Buf[7], &UpLoad57Data.transactionid,sizeof(Reply57));//得到累计数。
   
    SendBackCommunicationData(0x57,sizeof(Reply57));
}

/********************************************************
**结构名称:Receive58H()
**结构功能:接收58H命令，后台系统读加气机指定流水记录。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive58H(void)
{
    uint32_t id=0;
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));//发送数组清零。
    BufReverse2(&RecBack485Buf[7], 4);
    memcpy(&id,&RecBack485Buf[7],4);                //得到指定流水Id号。
    if(id<=sysparas.transnum)                       //判断是否有此Id号。
    {
        UpLoad58Data.transactionid=id;
        UpLoad58Data.ifid=1;                        //有该笔流水号。
        id=(id-1)%FlashRecMaxID;
        fuelrecordinfo_tmp=fuelrecordinfo;
        if(ReadFuelRecord(id,0))
        {
            memcpy(&SendBack485Buf[7],&UpLoad58Data.transactionid,sizeof(Reply58));
            memcpy(&SendBack485Buf[12],&fuelrecordinfo.transnum,sizeof(fuelrecordinfo)-2);
            fuelrecordinfo=fuelrecordinfo_tmp;
            SendBackCommunicationData(0x58,(sizeof(fuelrecordinfo)-2)+5);
        }
    }
    
    else                                            //没有此Id号。
    {
        UpLoad58Data.transactionid=id;
        UpLoad58Data.ifid=0;                        //没有该笔流水号。
        memcpy(&SendBack485Buf[7],&UpLoad58Data.transactionid,sizeof(Reply58));
        SendBackCommunicationData(0x58,sizeof(Reply58));
    }
}

/********************************************************
**结构名称:Receive59H()
**结构功能:接收59H命令，后台系统读取加气机通用信息。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive59H(void)
{
    UpLoad59Data.gunnum            = sysparas.gunnum;                   //枪号。
    UpLoad59Data.transactionid     = sysparas.transnum;                 //交易流水号。
    UpLoad59Data.baseblackversion  = sysparas.blackver;                 //基础黑名单版本号。
    UpLoad59Data.newaddblackversion= sysparas.addbkver;                 //新增黑名单版本号。
    UpLoad59Data.newdelblackversion= sysparas.delbkver;                 //新删黑名单版本号。
    UpLoad59Data.whitelistversion  = sysparas.whitever;                 //白名单版本号。
    UpLoad59Data.priceversion      = sysparas.pricever;                 //单价版本号。
    UpLoad59Data.baseinfoversion   = sysparas.comminfover;              //通用信息版本号。
    memcpy(&UpLoad59Data.stationnum[0]   ,&sysparas.stationcode[0],4);  //站编码，BCD码。
    memcpy(&UpLoad59Data.provincenum[0]  ,&sysparas.shengcode[0],4);    //省代码，BCD码。
    memcpy(&UpLoad59Data.citynum[0]      ,&sysparas.citycode[0],4);     //地市代码，BCD码。
    memcpy(&UpLoad59Data.parentunitnum[0],&sysparas.parentcode[0],4);   //上级单位代码，BCD码。
                                                                        //加气站名称，ASCII码。
                                                                        //嵌入式软件版本号,ASCII码。
                                                                        //加气机厂商信息，ASCII码。
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));                    //发送数组清零。
    memcpy(&SendBack485Buf[7],&UpLoad59Data.gunnum,sizeof(Reply59));    //得到加气机通用信息。
    
    SendBackCommunicationData(0x59,sizeof(Reply59));
}

/********************************************************
**结构名称:Receive5AH()
**结构功能:接收5AH上下班命令，后台返回上传结果。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5AH(void)
{
    if(RecBack485Buf[7]==0)		 //上传失败。
    {
        globalvar.UploadIdCount++;
    }
    else if(RecBack485Buf[7]==1) //上传成功。
    {
         globalvar.UploadIdCount=0;
         sysparas.shiftuptransnum++;
         SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));           
    }  
}
/********************************************************
**结构名称:UpLoad5AH()
**结构功能:上传上下班记录

**创 建 者:杨朝旭
**创建日期:2016-12-1 16:56:03

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad5AH(void)
{
    if(globalvar.uptransnum==sysparas.shiftuptransnum)
    {
        globalvar.UploadIdCount++;
        if(globalvar.UploadIdCount>10)
        {
            globalvar.UploadIdCount=0;
            sysparas.shiftuptransnum++;
            SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            return;
        }
    }    
    globalvar.uptransnum=sysparas.shiftuptransnum;
    shiftrecordinfo_tmp=shiftrecordinfo;
    if(ReadFuelRecord(sysparas.shiftuptransnum,1))
    {
       memcpy(&SendBack485Buf[7],&shiftrecordinfo.classid,sizeof(ShiftRecordInfo)-2);
       shiftrecordinfo=shiftrecordinfo_tmp;
       SendBackCommunicationData(0x5A,sizeof(ShiftRecordInfo)-2);
    }
}


/********************************************************
**结构名称:Receive5BH()
**结构功能:接收5BH命令，后台返回上传结果。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5BH(void)
{
    if(RecBack485Buf[7]==0)		//上传失败。
    {
        globalvar.UploadIdCount++;
    }
    else if(RecBack485Buf[7]==1)//上传成功。
    {
        globalvar.UploadIdCount=0; 
        sysparas.upmodnum++;
        SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)); 
    } 
}

/********************************************************
**结构名称:UpLoad5BH()
**结构功能:上传参数修改记录

**创 建 者:杨朝旭
**创建日期:2016-12-1 16:56:03

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad5BH(void)
{
    uint32_t addr,id;                                                   //作用于0x5B命令。
    uint8_t *pt;                                                        //作用于0x5B命令。
    Reply5B tmp;
    if(globalvar.uptransnum==sysparas.upmodnum)
    {
        globalvar.UploadIdCount++;
        if(globalvar.UploadIdCount>10)
        {
            globalvar.UploadIdCount=0;
            sysparas.upmodnum++;
            SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
            return;
        }
    }    
    globalvar.uptransnum=sysparas.upmodnum;
    id=sysparas.upmodnum%FlashModMaxID;
    addr=FlashModSADDR+id*FlashModLen;
    pt=(uint8_t *)(&tmp.modifyid);  
    if(ReadFlash(0,addr,pt,sizeof(Reply5B))==FALSE)
    {
        ReadFlash(1,addr,pt,sizeof(Reply5B));
    }
    memcpy(&SendBack485Buf[7],&tmp.modifyid,sizeof(Reply5B));
    SendBackCommunicationData(0x5B,sizeof(Reply5B));
}


/********************************************************
**结构名称:UpLoad5CH(void)
**结构功能:5CH命令，加气机验证IC卡可用性。

**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad5CH(void)
{
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));    //发送数组清零。
    memcpy(&SendBack485Buf[7],m1card.cardnum,8);		//需要验证的IC卡卡号。
	memcpy(&SendBack485Buf[15],&m1card.cardbalance,4);	//余额
	SendBack485Buf[19] = 0x20;							//世纪
	memcpy(&SendBack485Buf[20],m1card.lasttradetime,6);	//年月日时分秒
    SendBackCommunicationData(0x5C,sizeof(Reply5C));						
}

/********************************************************
**结构名称:Receive5CH(void)
**结构功能:5CH命令，后台恢复加气机卡验证结果。

**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5CH(void)
{ 
	// 0-正常卡。
    // 1-请求卡号不存在。 
    // 2-黑名单号。  
    // 3-余额异常。
    // 4-最后交易时间异常。
    m1card.checkresult = RecBack485Buf[15];
    m1card.checkflag=0;
}

/********************************************************
**结构名称:UpLoad5DH(void)
**结构功能:5DH命令，加气机返回下班结果。

**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad5DH(void)
{
    memset(SendBack485Buf,0,sizeof(SendBack485Buf));                  //发送数组清零。
    memcpy(&SendBack485Buf[7],&UpLoad5DData.OffWork,sizeof(Reply5D)); //得到发送数据。
    SendBackCommunicationData(0x5D,sizeof(Reply5D));
}

/********************************************************
**结构名称:Receive5DH()
**结构功能:接收5DH命令，后台对加气机下发下班命令。

**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5DH(void)
{
	uint8_t *tmp;
	
	if(globalvar.workstate!=StaIdle)
	{
		UpLoad5DData.OffWork = 0;
		UpLoad5DH();
		return;
	}
	sysparas.shiftransnum++;
	shiftrecordinfo.classid=sysparas.shiftransnum;								  //上下班记录流水号，4字节。
	shiftrecordinfo.transactiontype=1;											  //下班。
	shiftrecordinfo.classnum=sysparas.shiftnum; 								  //班号，1字节。
																				  //员工卡号和上班时一样，保持不变，BCD码。
	shiftrecordinfo.classstarttime[0]=0x20; 									  //班开始时间，7字节，BCD码。
	memcpy(&shiftrecordinfo.classstarttime[1], &sysparas.shiftstarttime[0], 6);
	
	shiftrecordinfo.classendtime[0]=0x20;										  //班结束时间，7字节，BCD码。
	memcpy(&shiftrecordinfo.classendtime[1],&time.year,6);
	memcpy(&sysparas.shiftendtime[1],&time.year,6);
	
	shiftrecordinfo.allgunnum=sysparas.totvolume;								  //枪气量累计，4字节。
	shiftrecordinfo.allgunmoney=sysparas.totmoney;								  //枪金额累计,4字节。
	shiftrecordinfo.classgasnum=sysparas.shiftotvol;							  //班气量，4字节。
	shiftrecordinfo.classgasmoney=sysparas.shiftotmon;							  //班金额,4字节。
	shiftrecordinfo.classtimes=sysparas.shiftfueltimes; 						  //班加气次数，4字节。
	shiftrecordinfo.devicetype=sysparas.devicetype; 							   //设备类型;													  //气品代码，1字节，0-CNG，1-LNG。
	shiftrecordinfo.measuretype=sysparas.unit;									  //计量单位，1字节，0-标方，1-Kg。
	shiftrecordinfo.classiding=sysparas.shiftransnum-1; 						  //上班记录流水号。
   
	tmp=(uint8_t *)(&shiftrecordinfo.classid);
	shiftrecordinfo.crc=CRC16Calculate(tmp, sizeof(shiftrecordinfo)-2);
//	  SaveFuelRecord(1);	//存班流水。
//	  sysparas.offonwork = 0;														//下班。
//	  globalvar.workstate=StaOffDuty;
//	  memcpy(&sysparas.shiftendtime[0], &time.year, 6);
//	  SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)); 		//存系统参数。
	if(SaveFuelRecord(1))//存班流水。
	{
		globalvar.workstate=StaOffDuty;//加气机状态，下班。
		sysparas.offonwork=0;		   //下班标志。
		memcpy(&sysparas.shiftendtime[0], &time.year, 6);
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
	
	UpLoad5DData.OffWork = 1;
   
	UpLoad5DH();																  //上传下班结果。
}


/********************************************************
**结构名称:Receive5EH()
**结构功能:接收5EH命令，后台返回下发单价数据内容。

**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5EH(void)
{
    StrReverse(&RecBack485Buf[17],RecBack485Buf[16],2);
    memcpy(&sysparas.price[0], &RecBack485Buf[17], RecBack485Buf[16] * 2);
    if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
    {
        OsplcShow(0,0,sysparas.price[0],0,0);
        sysparas.pricever = RecBack485Buf[7]; 
    } 
}

/********************************************************
**结构名称:UpLoad5EH(void)
**结构功能:加气机申请下发单价数据。

**创 建 者:杨传坤
**创建日期:2016-07-15

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void UpLoad5EH(void)
{
     UpLoad5EData.lcng=0;
	 memset(SendBack485Buf,0,sizeof(SendBack485Buf));//发送数组清零。
     SendBack485Buf[7]=UpLoad5EData.lcng;
	 SendBackCommunicationData(0x5E,sizeof(Reply5E));
}

/********************************************************
**结构名称:Receive5FH(void)
**结构功能:后台解锁或者锁定加气机。

**创 建 者:杨传坤
**创建日期:2016-07-15

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive5FH(void)
{
    if(RecBack485Buf[7]==0)     					 //解锁加气机。
    {
        sysparas.yesnolock= 0;
    }
    else if(RecBack485Buf[7]==1)					 //锁加气机。
    {
        sysparas.yesnolock= 1;
    }
    SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas));
    
    UpLoad5FData.commdwork=1;
    memset(SendBack485Buf,0,sizeof(SendBack485Buf)); //发送数组清零。
    SendBack485Buf[7] = UpLoad5FData.commdwork;
    SendBackCommunicationData(0x5F,sizeof(Reply5F));
    
}

/********************************************************
**结构名称:Receive60H()
**结构功能:接收后台发送的电子标签命令，并向后台返回加气机状态。

**创 建 者:杨传坤
**创建日期:2016-011-22

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
void Receive60H(void)
{
	
	if((globalvar.workstate==StaOffDuty)||                       //加气机下班。
		(globalvar.workstate==StaIdle)||						 //加气机空闲。
		(globalvar.workstate==StaFault))						 //加气机故障。
	{
		globalvar.startstate = RecBack485Buf[7];				 //电子标签，启动状态。
		globalvar.startways  = RecBack485Buf[8];				 //电子标签，启动方式。
		memcpy(&globalvar.startnum[0], &RecBack485Buf[9], 8);    //电子标签，启动序列号。
		globalvar.handnum    = RecBack485Buf[17];				 //电子标签，手持机编号。
	
		memset(SendBack485Buf,0,sizeof(SendBack485Buf));         //发送数组清零。
		SendBack485Buf[7]= globalvar.workstate;   
		SendBackCommunicationData(0x60,1);
	}	
}

/********************************************************
**结构名称:Receive61H()
**结构功能:用于没有后台的情况下，测试主板后台串口是否通讯正常;
		   接收数据:0x11,0x22,0x33,0x44,0x55;
		   返回数据:0x01,0x02,0x03,0x04,0x05;
**创 建 者:杨传坤
**创建日期:2017-11-27

**修改者  :
**修改日期:
**修改内容:
********************************************************/

void Receive61H(void)
{
	memset(SendBack485Buf,0,sizeof(SendBack485Buf)); //发送数组清零。
    SendBack485Buf[7]  = 0x01;
	SendBack485Buf[8]  = 0x02;
	SendBack485Buf[9]  = 0x03;
	SendBack485Buf[10] = 0x04;
	SendBack485Buf[11] = 0x05;
    SendBackCommunicationData(0x61,5);
}

#if 0
/********************************************************
**结构名称:RecBlckDataCheck(void)
**结构功能:串口接收后台数据检测，返回值无效。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
bool RecBlckDataCheck(void)
{
    uint8_t i=0;
    uint16_t len=0;
    uint16_t CRC1=0x1122;
    uint16_t CRC2=0;
    if(UART3Buffer[0]!=0xAA)                     //接收第一个字符后，判断第一个字节是否为起始符1。
    {
        UART3Count=0;
        ClearBuf(UART3Buffer,0,sizeof(UART3Buffer));
        return 0;
    }
    if((UART3Count==1)&&(UART3Buffer[1]!=0xFF))  //接收帧头第一个字符后，判断第二个字节是否为起始符2。
    {
        return 0;
    }
    if((UART3Count==2)&&(UART3Buffer[1]!=0xFF))  //接收帧头第2个字符后，判断第二个字节是否为起始符2。
    {
        UART3Count=0;
        ClearBuf(UART3Buffer,0,sizeof(UART3Buffer));
        return 0;
    }
    if((UART3Buffer[UART3Count-2]==0xAA)&&(UART3Buffer[UART3Count-1]==0xFF)&&(UART3Count>2))//在帧头外判断是否还有起始符1，起始符2。
    {
        UART3Count=2;
        ClearBuf(UART3Buffer,0,sizeof(UART3Buffer));
        UART3Buffer[0]=0xAA;
        UART3Buffer[1]=0xFF;
        return 0;
    }
    if((UART3Buffer[0]==0xAA)&&(UART3Buffer[1]==0xFF))//判断帧头是否为起始符。
    {
        if((UART3Buffer[UART3Count-2]==0xCC)&&(UART3Buffer[UART3Count-1]==0xFF))//判断帧尾是否为截止符。
        {
            CloseUartRcvIRQ(3);                                  //关接收中断。
            memset(RecBack485Buf,0,sizeof(RecBack485Buf));
            for(i=0;i<UART3Count;i++)
            {
                RecBack485Buf[i]=UART3Buffer[i];                 //将接收缓冲区赋值给数组。
            }
            UART3Count=0;                                        //将缓冲长度清零。
            ClearBuf(UART3Buffer,0,sizeof(UART3Buffer));
            
            len  = RecBack485Buf[5]*256+RecBack485Buf[6];        //消息体长度。 
            CRC1 = CRC16Calculate(&RecBack485Buf[2], len+5);     //CRC校验。
            CRC2 = RecBack485Buf[len+7]*256+RecBack485Buf[len+8];//后台数据自带CRC。
            if(CRC1 != CRC2)                                     //CRC比对。
            {
                memset(RecBack485Buf,0,sizeof(RecBack485Buf));
                OpenUartRcvIRQ(PCPort);
                return 0;
            }
            
            MsDelay(1); 
            globalvar.comfailcount=0;                            //后台与主板通信是否正常标志
            globalvar.comreceiveflag=1;                             
            return 0;
        }
    }
    return 0;
}
#endif

/********************************************************
**结构名称:RecBlckDataCheck(void)
**结构功能:串口接收后台数据检测，返回值无效。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :黄豪民
**修改日期:2016-11-3
**修改内容:从新写了判断函数
********************************************************/ 
bool RecBlckDataCheck(void)
{
	uint16_t len=0;
    uint16_t CRC1=0x1122;
    uint16_t CRC2=0;
	len=UART3Buffer[5]*256+UART3Buffer[6]; 
	if(UART3Count>=(len+9))
	{
		CloseUartRcvIRQ(3);                                  		  //关接收中断
		CRC1 = CRC16Calculate((uint8_t *)&UART3Buffer[2], len+5);     //CRC校验。
		CRC2 = UART3Buffer[len+7]*256+UART3Buffer[len+8];			  //后台数据自带CRC
		if(CRC1==CRC2)
		{
			memcpy(&RecBack485Buf[0],(uint8_t *)&UART3Buffer[0],len+9);
			if(RecBack485Buf[2]==sysparas.gunnum)
			{
				globalvar.comfailcount=0;                             //后台与主板通信是否正常标志
				globalvar.comreceiveflag=1;
			}
			else
			{
				UART3Count=0;     
				memset((uint8_t *)&UART3Buffer[0],0,sizeof(RecBack485Buf));	
				OpenUartRcvIRQ(3);
			}
		}
		else
		{
			UART3Count=0;     
			memset((uint8_t *)&UART3Buffer[0],0,sizeof(RecBack485Buf));	
			OpenUartRcvIRQ(3);
		}
	}
	if(UART3Count>=BUFSIZE)
	{
		UART3Count=0;     
		memset((uint8_t *)&UART3Buffer[0],0,sizeof(RecBack485Buf));	
		OpenUartRcvIRQ(3);
	}
	return 0;
}

/********************************************************
**结构名称:BGCommunication(uint8_t data)
**结构功能:判断后台命令类型，加气机执行不同的命令函数。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :黄豪民
**修改日期:2016-11-3
**修改内容:从新写了判断函数
********************************************************/ 
void BGCommunication(uint8_t data)
{
    switch(data)                             //判断命令。
    {
        case 0x51:
                Receive51H();  
            break;
        case 0x52:
                Receive52H();
            break;
        case 0x53:
                Receive53H();
            break;
        case 0x54:
                Receive54H();
            break;
        case 0x55:
                Receive55H();
            break;
        case 0x56:
                Receive56H();
            break;
        case 0x57:
                Receive57H();
            break;
        case 0x58:
                Receive58H();
            break;
        case 0x59:                   
                Receive59H();
            break ;
        case 0x5A:
                Receive5AH();
            break;
        case 0x5B:                   
                Receive5BH();
            break ;
        case 0x5C:                   
                Receive5CH();
            break ;
        case 0x5D:                   
                Receive5DH();
            break ;
        case 0x5E:                   
                Receive5EH();
            break ;
        case 0x5F:
                Receive5FH();
            break ;
		case 0x60:
				Receive60H();
			break;
		case 0x61:
				Receive61H();
        default:
            break; 
    }
    UART3Count=0;                                        //将缓冲长度清零。
    ClearBuf(UART3Buffer,0,sizeof(UART3Buffer));
    OpenUartRcvIRQ(PCPort);
}



    





