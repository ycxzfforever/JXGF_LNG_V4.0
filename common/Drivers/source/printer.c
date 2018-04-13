/************针对北京炜煌打印机驱动*********/
#include "kernel.h"

uint8_t stopreasonbuf[][16]=
{
    "手动停机",         // 0
    "气相流量计异常",   // 1
    "液相流量计异常",   // 2
    "瞬时数据错误",     // 3
    "压力传感器异常",   // 4
    "掉电或复位停机",   // 5
    "定气量停机",       // 6
    "定金额停机",       // 7
    "低流速停机",       // 8
    "高流速停机",       // 9
    "回气密度停机",     // 10
    "回气增益停机",     // 11
    "超压停机",         // 12
    "逃卡停机",         // 13
    "记账超额",         // 14
    "单次超额或超量",   // 15    
};

void WHPrinterInit(void)
{
    SELECT_PRINT();
    UartSendByte(PrintPort,0x1B);
    UartSendByte(PrintPort,0x40);
    UartSendByte(PrintPort,0x0D);
    SELECT_CARD();
}

void WHPrinterNormalCmd(void)
{
    unsigned char cmd[14] = {0x1b, 0x40, 0x1c, 0x26, 0x1b, 0x31, 0x08,
                             0x1b, 0x70, 0x02, 0x1b, 0x63, 0x00, 0x0D
                            };
    //0x1c 26:进入汉字打印方式
    //0x1b 56 02:正常高度两倍打印
    //0x1b 63 00:字符从右向左的反向打印
    UartSendStr(PrintPort,cmd,14);
}


void WHPrinterFreeRow(unsigned char rows)
//空行
{
    while(rows--)
    {
        UartSendByte(PrintPort,0x0A);//换行
        UartSendByte(PrintPort,0x0D);//回车，确认打印换行(及之前的)命令
    }
}
//////////////////////////////////////////////////////////////////////////////头
void WHPrinterHead(void)
{
    unsigned char buff[8] = {0x1c, 0x26, 0x1b, 0x56, 0x02, 0x1b, 0x63, 0x00};
    //0x1c 26:进入汉字打印方式
    //0x1b 56 02:正常高度两倍打印
    //0x1b 63 00:字符从右向左的反向打印
    UartSendStr(PrintPort,buff,8);
}

uint8_t WHPrinterCheck(void)
{
    uint8_t  val;
    UartSendByte(PrintPort,0x1c);
    UartSendByte(PrintPort,0x76);
    UartSendByte(PrintPort,0x0D);
    val=uart0GetByte();    
    if(val == 0xFF)    return(0xFF);
    if(val == 0x04)    return(0x04);
    return TRUE; 
}

uint8_t WHPrintReady(void)
{
    if(WHPrinterCheck()!=TRUE)    return FALSE;
    return TRUE;
}

void WHPrintf(char *fmt,...)
{
    va_list ap;
    char str[40];
    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    uart0PutString(str);
    va_end(ap);
}

void WHFuelRecord(void)
//打印流水记录
{
    SELECT_PRINT();

    WHPrinterHead();
    WHPrintf("         LCNG加气凭条\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintf("枪号:%d\r\n",sysparas.gunnum);
    WHPrintf("班号:%d\r\n",fuelrecordinfo.classnum);
    WHPrintf("流水号:%ld\r\n",fuelrecordinfo.transnum);
    WHPrintf("单价:%ld.%02d 元/kg\r\n",fuelrecordinfo.price/100,fuelrecordinfo.price%100);
    WHPrintf("气量:%ld.%02d kg\r\n", fuelrecordinfo.volume/100,fuelrecordinfo.volume%100);
    WHPrintf("金额:%ld.%02d 元\r\n", fuelrecordinfo.money/100,fuelrecordinfo.money%100);
    WHPrintf("卡号:%02X%02X%02X%02X\r\n", fuelrecordinfo.usernum[4],fuelrecordinfo.usernum[5],
                                          fuelrecordinfo.usernum[6],fuelrecordinfo.usernum[7]);
    WHPrintf("实扣金额:%ld.%02d 元\r\n", fuelrecordinfo.reallymoney/100,fuelrecordinfo.reallymoney%100);
    WHPrintf("余额:%ld.%02d 元\r\n", fuelrecordinfo.afterbalance/100,fuelrecordinfo.afterbalance%100);
    if((sysparas.backgascalc==1)&&(fuelrecordinfo.stopreason!=0xFF))//解灰数据不打印进液量和回气量
    {
        WHPrintf("进液量:%ld.%02d kg\r\n", (fuelrecordinfo.volume+fuelrecordinfo.backgas)/100,(fuelrecordinfo.volume+fuelrecordinfo.backgas)%100);
        WHPrintf("回气量:%ld.%02d kg\r\n", fuelrecordinfo.backgas/100,fuelrecordinfo.backgas%100);
    }
    if(fuelrecordinfo.stopreason==0xFF)
        WHPrintf("交易类型:解灰\r\n");
    else
        WHPrintf("停机原因:%s\r\n", stopreasonbuf[fuelrecordinfo.stopreason]);
    WHPrintf("开始时间:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.starttime[1],fuelrecordinfo.starttime[2],fuelrecordinfo.starttime[3],
                                                          fuelrecordinfo.starttime[4],fuelrecordinfo.starttime[5],fuelrecordinfo.starttime[6]);
    WHPrintf("结束时间:20%02X-%02X-%02X %02X:%02X:%02X\r\n", fuelrecordinfo.endtime[1],fuelrecordinfo.endtime[2],fuelrecordinfo.endtime[3],
                                                          fuelrecordinfo.endtime[4],fuelrecordinfo.endtime[5],fuelrecordinfo.endtime[6]);
    
    WHPrinterFreeRow(1);
    WHPrintf("     欢迎光临  出行平安\r\n");//下分割线
    WHPrintf("     <请妥善保存此凭条>\r\n");//脚注 
    WHPrinterFreeRow(6); 
    SELECT_CARD();
}

//打印流水。
void Printid(void)
{
    WHPrinterInit();//初始化。
    if(WHPrintReady())
    {
        WHFuelRecord();
    }
}

void WHFuelShifRecord(void)
//打印班流水记录
{
    SELECT_PRINT();

    WHPrinterHead();
    WHPrintf("         LCNG班累记录\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintf("枪号:%d\r\n",sysparas.gunnum);
    WHPrintf("班号:%ld\r\n",shiftrecordinfo.classnum);
    WHPrintf("班流水号:%ld\r\n",shiftrecordinfo.classid);
    if(shiftrecordinfo.transactiontype==0)
        WHPrintf("交易类型:上班\r\n");
    else if(shiftrecordinfo.transactiontype==1)
        WHPrintf("交易类型:下班\r\n");
    WHPrintf("员工卡号:%02X%02X%02X%02X\r\n", shiftrecordinfo.cardstaffnum[4],shiftrecordinfo.cardstaffnum[5],
                                             shiftrecordinfo.cardstaffnum[6],shiftrecordinfo.cardstaffnum[7]);
    WHPrintf("开始时间:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classstarttime[1],shiftrecordinfo.classstarttime[2],shiftrecordinfo.classstarttime[3],
                                                           shiftrecordinfo.classstarttime[4],shiftrecordinfo.classstarttime[5],shiftrecordinfo.classstarttime[6]);
    WHPrintf("结束时间:%02X-%02X-%02X %02X:%02X:%02X\r\n", shiftrecordinfo.classendtime[1],shiftrecordinfo.classendtime[2],shiftrecordinfo.classendtime[3],
                                                           shiftrecordinfo.classendtime[4],shiftrecordinfo.classendtime[5],shiftrecordinfo.classendtime[6]);
    
    WHPrintf("枪气累计:%ld.%02d\r\n",shiftrecordinfo.allgunnum/100,shiftrecordinfo.allgunnum%100);
    WHPrintf("枪金累计:%ld.%02d\r\n",shiftrecordinfo.allgunmoney/100,shiftrecordinfo.allgunmoney%100);
    WHPrintf("班气累计:%ld.%02d\r\n",shiftrecordinfo.classgasnum/100,shiftrecordinfo.classgasnum%100);
    WHPrintf("班金累计:%ld.%02d\r\n",shiftrecordinfo.classgasmoney/100,shiftrecordinfo.classgasmoney%100);
    
    WHPrintf("班加气数:%ld\r\n", shiftrecordinfo.classtimes);

    if(shiftrecordinfo.devicetype==0)
        WHPrintf("气品代码:LNG\r\n");
    else if(shiftrecordinfo.devicetype==1)
        WHPrintf("气品代码:CNG\r\n");    
    if(shiftrecordinfo.measuretype==0)
        WHPrintf("计量单位:公斤\r\n");
    else if(shiftrecordinfo.measuretype==1)
        WHPrintf("计量单位:标方\r\n");
    
    WHPrintf("上班流水:%ld\r\n", shiftrecordinfo.classiding);

    WHPrinterFreeRow(1);
    WHPrintf("     欢迎光临  出行平安\r\n");//下分割线
    WHPrintf("     <请妥善保存此凭条>\r\n");//脚注 
    WHPrinterFreeRow(6); 
    SELECT_CARD();
}

//打印总累
void PrintSum(void)
{
	WHPrinterInit();//初始化。
	if(WHPrintReady())
    {
        WHFuelSumRecord();
    }
}
void WHFuelSumRecord(void)
{
	SELECT_PRINT();

    WHPrinterHead();
    WHPrintf("         LCNG总累记录\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintf("枪号:%d\r\n",sysparas.gunnum);
    WHPrintf("总累流水号:%ld\r\n",sysparas.transnum);
    WHPrintf("总气累计:%ld.%02d\r\n",sysparas.totvolume/100,sysparas.totvolume%100);
    WHPrintf("总金累计:%ld.%02d\r\n",sysparas.totmoney/100,sysparas.totmoney%100);   
    if(fuelrecordinfo.devicetype==0)
        WHPrintf("气品代码:LNG\r\n");
    else if(fuelrecordinfo.devicetype==1)
        WHPrintf("气品代码:CNG\r\n");    
    if(fuelrecordinfo.measuretype==0)
        WHPrintf("计量单位:公斤\r\n");
    else if(fuelrecordinfo.measuretype==1)
        WHPrintf("计量单位:标方\r\n");   
    WHPrinterFreeRow(1);
    WHPrintf("     <请妥善保存此凭条>\r\n");//脚注 
    WHPrinterFreeRow(6); 
    SELECT_CARD();
}
//打印系统参数
void PrintSysInfo(void)
{
	WHPrinterInit();//初始化。
    if(WHPrintReady())
    {
        WHFuelSysInfo();
    }
}
void WHFuelSysInfo(void)
{
	SELECT_PRINT();

    WHPrinterHead();
    WHPrintf("         LCNG系统参数\r\n");
    WHPrinterFreeRow(1);
    WHPrinterNormalCmd();
    WHPrintf("枪号:%d\r\n",sysparas.gunnum);
	WHPrintf("班号:%d\r\n",sysparas.shiftnum);
	WHPrintf("最大班号:%d\r\n",sysparas.shiftmaxnum); 
    WHPrintf("单价:%d.%02d\r\n",sysparas.price[0]/100,sysparas.price[0]%100);
	WHPrintf("总气量:%ld.%02d\r\n",sysparas.totvolume/100,sysparas.totvolume%100);
    WHPrintf("总金额:%ld.%02d\r\n",sysparas.totmoney/100,sysparas.totmoney%100);
    WHPrintf("班开始气量:%ld.%02d\r\n",sysparas.shiftstartvol/100,sysparas.shiftstartvol%100);
    WHPrintf("班开始金额:%ld.%02d\r\n",sysparas.shiftstartmon/100,sysparas.shiftstartmon%100);    
    WHPrintf("班加气次数:%ld\r\n",sysparas.shiftfueltimes);    
    WHPrintf("班气量:%ld.%02d\r\n",sysparas.shiftotvol/100,sysparas.shiftotvol%100);
    WHPrintf("班金额:%ld.%02d\r\n",sysparas.shiftotmon/100,sysparas.shiftotmon%100);    
    WHPrintf("流水号:%ld\r\n",sysparas.transnum);
    WHPrintf("已传流水号:%ld\r\n",sysparas.uptransnum); 
    WHPrintf("班流水号:%ld\r\n",sysparas.shiftransnum);
    WHPrintf("已传班流水号:%ld\r\n",sysparas.shiftuptransnum); 
    WHPrintf("修改流水号:%ld\r\n",sysparas.modnum);
    WHPrintf("已传修改号:%ld\r\n",sysparas.upmodnum);   
	WHPrintf("流量计类型:%d\r\n",sysparas.modbustype);
    WHPrintf("计量单位:%d\r\n",sysparas.unit);
    WHPrintf("质检模式:%d\r\n",sysparas.qcmode);
    WHPrintf("EE写次数:%ld\r\n",(SYSPARA_ADDR/sizeof(Sysparas))*EEPROM_W_TIMES+sysparas.writeE2num); 
	WHPrintf("密度:%d.%04d\r\n",sysparas.densty/10000,sysparas.densty%10000);
    WHPrintf("修正系数:%d.%04d\r\n",sysparas.coeff/10000,sysparas.coeff%10000);
    WHPrintf("压力零点:%d\r\n",sysparas.pzero);
    WHPrintf("压力系数:%d\r\n",sysparas.pcoeff); 	
	WHPrintf("卡上限:%ld.%02d\r\n",sysparas.cardmaxyue/100,sysparas.cardmaxyue%100);
    WHPrintf("卡下限:%ld.%02d\r\n",sysparas.cardminyue/100,sysparas.cardminyue%100);
    WHPrintf("PSAM卡座:%d\r\n",sysparas.cardpsamnum);
    WHPrintf("间隔天数:%d\r\n",sysparas.cardmaxday); 		
	WHPrintf("流量计波特:%ld\r\n",sysparas.modbusbaud);
    WHPrintf("后台波特率:%ld\r\n",sysparas.pcbaud);
    WHPrintf("加气模式:%d\r\n",sysparas.fuelmode);
    WHPrintf("打印方式:%d\r\n",sysparas.printmode); 	
	WHPrintf("单价版号:%d\r\n",sysparas.pricever);
    WHPrintf("黑名单版号:%d\r\n",sysparas.blackver);
    WHPrintf("白名单版号:%d\r\n",sysparas.whitever);
    WHPrintf("新增黑版号:%d\r\n",sysparas.addbkver); 	
	WHPrintf("新删黑版号:%d\r\n",sysparas.delbkver);
    WHPrintf("通用信息版号:%d\r\n",sysparas.comminfover);
    WHPrintf("密钥版号:%d\r\n",sysparas.lockver);
    WHPrintf("是否使用加密板:%d\r\n",sysparas.usepwdboard); 
	WHPrintf("系统版本号:%s\r\n",VERSION);
 
    WHPrinterFreeRow(1);
    WHPrintf("     <请妥善保存此凭条>\r\n");//脚注 
    WHPrinterFreeRow(6); 
    SELECT_CARD();
}

//打印流水。
void PrintShifid(void)
{
    WHPrinterInit();//初始化。
    if(WHPrintReady())
    {
        WHFuelShifRecord();
    }
}

void uart0PutString(char* string)
//发送字符串    
{
    while(*string!='\0')
    {
        UartSendByte(PrintPort,*string);
        string++;
    }
}

 /***************************************************************************
    **函数名称： uart0GetByte
    **函数功能：从串口接收1字节数据，使用查询方式接收
    **入口参数：无
    **返回值： 接收到的数据
    **创建者：杨朝旭
    **创建日期：2016-3-29 08:59:46
    **
    **                            LPC_UART0->IER = 0;  // 关接收中断
                                    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS; //开接收中断
    **修改者：
    **修改日期：
    **修改内容：
    ***************************************************************************/
uint8_t uart0GetByte (void)             //查询法
{
    uint8_t     ucRcvData=0xFF;
    uint32_t    tickcount=0;
    while (((LPC_UART0->LSR & 0x01))&&(++tickcount<50000) == 0);  //UnLSR[0] 该位是否置一，决定能否从FIFO中读取数据                                                           
    if(tickcount<50000) ucRcvData = LPC_UART0->RBR;                //UnRBR为接收缓存寄存器                            /* 读取数据                     */
    return (ucRcvData);   
}


