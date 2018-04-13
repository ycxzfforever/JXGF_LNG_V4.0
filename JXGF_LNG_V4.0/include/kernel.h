#ifndef KERNEL_H_
#define KERNEL_H_

#pragma pack(1)
#define VERSION "LTY.V4.0170329" 
#define HardIcTest 1 //硬件平台IC卡测试,及其它测试宏定义。1测试，0不测试。

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "LPC177x_8x.h"
#include "type.h"
#include "ctrl.h"
#include "ssp.h"
#include "mt318.h"
#include "uart.h"
#include "fm25l04.h"
#include "timer.h"
#include "sst25vf064c.h"
#include "keyboard.h"
#include "fyd12864.h"
#include "printer.h"
#include "modbus.h"
#include "threedes.h"
#include "delay.h"
#include "lngctrl.h"
#include "turn.h"
#include "splc.h"
#include "at24c512.h"
#include "ds3232.h"
#include "eeprom.h"
#include "i2c0.h"
#include "i2c1.h"
#include "rtc.h"
#include "adc.h"
#include "menu.h"
#include "stdbool.h"
#include "math.h"   
#include "controlparas.h"
#include "query.h"
#include "crc.h"
#include "devicetest.h"
#include "app.h"
#include "password.h"
#include "eleclabel.h"



//位定义
#define BIT0		0X00000001
#define BIT1		0X00000002
#define BIT2		0X00000004
#define BIT3		0X00000008
#define BIT4		0X00000010
#define BIT5		0X00000020
#define BIT6		0X00000040
#define BIT7		0X00000080
#define BIT8		0X00000100
#define BIT9		0X00000200
#define BIT10		0X00000400
#define BIT11		0X00000800
#define BIT12		0X00001000
#define BIT13		0X00002000
#define BIT14		0X00004000
#define BIT15		0X00008000
#define BIT16		0X00010000
#define BIT17		0X00020000
#define BIT18		0X00040000
#define BIT19		0X00080000
#define BIT20		0X00100000
#define BIT21		0X00200000
#define BIT22		0X00400000
#define BIT23		0X00800000
#define BIT24		0X01000000
#define BIT25		0X02000000
#define BIT26		0X04000000
#define BIT27		0X08000000
#define BIT28		0X10000000
#define BIT29		0X20000000
#define BIT30		0X40000000
#define BIT31		0X80000000

#define GetBit(dat,i) (((dat)&(1ul<<(i)))?1:0)  //判断dat第i位是否为1
#define SetBit(dat,i) ((dat)|=(1ul<<(i)))        //将dat第i位置1
#define ClrBit(dat,i) ((dat)&=(~(1ul<<(i))))    //将dat第i位置0

#define PowerCheck()  LPC_GPIO2->DIR &= ~(1ul<<27) // 方向-输入。
#define PowerCheckIn   ((LPC_GPIO2->PIN)&(1ul<<27))//掉电管脚输入电平检测。


extern uint8_t CirculateTbl[10];//循环模拟加气时间数组。
//加气机状态定义
#define StaOffDuty      0xA0    // 下班
#define StaIdle         0xA1    // 空闲
#define StaSPrecool     0xA2    // 小循环
#define StaBPrecool     0xA3    // 大循环
#define StaFuel         0xA4    // 加气中
#define StaPay          0xA5    // 待扣费
#define StaClose        0xA6    //  关闭
#define StaOtherFuel    0xA7    //对方加液
#define StaFault        0xF1    // 设备故障
#define StaClok         0xF0    //加气机已锁


//班记录信息
typedef struct ShiftRecordInfo
{   
    uint32_t  classid;                //上下班记录流水号，4字节。
    uint8_t   transactiontype;        //交易类型，1字节。
    uint8_t   classnum;               //班号，1字节。
    uint8_t   cardstaffnum[8];        //员工卡号，BCD码。
    uint8_t   classstarttime[7];      //班开始时间，7字节，BCD码。
    uint8_t   classendtime[7];        //班结束时间，7字节，BCD码。
    uint32_t  allgunnum;              //枪气量累计，4字节。
    uint32_t  allgunmoney;            //枪金额累计,4字节。
    uint32_t  classgasnum;            //班气量，4字节。
    uint32_t  classgasmoney;          //班金额,4字节。
    uint32_t  classtimes;             //班加气次数，4字节。
    uint8_t   devicetype;             //设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint8_t   measuretype;            //计量单位，1字节，1-标方，0-Kg。
    uint32_t  classiding;             //上班记录流水号。
//以下为备份参数，当增加一个存储变量后，
//需要删除以下对应的字节数，以保证crc正确
    uint8_t     backup[20];
    uint16_t  crc;                    //crc存储校验。
}ShiftRecordInfo;


//加气流水记录信息
typedef struct FuelRecordInfo
{   
    uint32_t  transnum;               //交易流水号，4字节。
    uint8_t   classnum;               //班号，1字节。
    uint8_t   employeenum[8];         //员工号码，10字节，BCD码。
    uint8_t   transactiontype;        //交易类型，1字节。
    uint8_t   usernum[8];             //用户卡号，10字节,BCD码。
    uint8_t   cardtype;               //卡类型 ， 1字节。
    uint8_t   carnum[9];              //车牌号码，7字节，ASCII码。
    uint8_t   stationnum[4];          //加气站编码，4字节，BCD码。
    uint8_t   starttime[7];           //加气开始时间，7字节，BCD码。
    uint8_t   endtime[7];             //加气结束时间，7字节，BCD码。
    uint16_t  price;                  //本次加气单价,2字节。
    uint32_t  volume;                 //当前加气气量，4字节。
    uint32_t  money;                  //当前加气金额，4字节。
    uint32_t  reallymoney;            //本次实际扣款金额。
    uint32_t  beforebalance;          //加气前余额，4字节。
    uint32_t  afterbalance;           //加气后余额，4字节。
    uint32_t  allrechargetimes;       //累计充值次数，4字节。
    uint32_t  allrechargemoney;       //累计充值金额，4字节。
    uint32_t  allconsumptiontimes;    //累计消费次数，4字节。
    uint32_t  allconsumptionnum;      //累计消费气量，4字节。
    uint32_t  allconsumptionmoney;    //累计消费金额，4字节。
    uint32_t  allgunnum;              //枪气量累计，4字节。
    uint32_t  allgunmoney;            //枪金额累计,4字节。
    uint16_t  beforepressure;         //加气前压力，2字节。
    uint16_t  afterpressure;          //加气后压力，2字节。
    uint32_t  density;                //密度，4字节。
    uint32_t  curnum;                 //当量，4字节。修正系数。
    uint8_t   devicetype;             //设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint8_t   measuretype;            //计量单位，1字节，1-标方，0-Kg。
    uint8_t   stopreason;             //停机原因。 
    uint32_t  jhid;                   //解灰相关流水号。
    uint32_t  classid;                //班流水号。
    uint8_t   fuelmode;               //加气方式
    uint8_t   startserial[8];         //启动序列号
    uint8_t   posnum;                 //手持机编号
    uint32_t  backgas;                //回气量
//以下为备份参数，当增加一个存储变量后，
//需要删除以下对应的字节数，以保证crc正确
    uint8_t     backup[20];
    uint16_t    crc;                  //16位数据校验
}FuelRecordInfo;

//加气机参数
typedef struct def_SysParas
{
    uint8_t     head;       		// 0 头		
    uint32_t    writeE2num;		 	// 1写eeprom次数
    
    uint8_t     gunnum;     		// 2枪号
    uint32_t    transnum;   		// 3交易流水号     
    uint32_t    uptransnum; 		// 4已传交易流水号     
    
    uint32_t    totvolume;  		//5枪总气量    2位小数
    uint32_t    totmoney;   		//6枪总金额    2位小数

    uint16_t    price[20];  		//7-26 20个单价，单价0为默认单价，根据单价等级选择相应单价

    uint8_t     modbustype; 		//27流量计类型，1-罗斯蒙特，2-E+H
    uint8_t     unit;       		//28计量单位 0-kg,1-标方
    uint8_t     qcmode;     		//29质检模式设置 

    uint32_t    densty;             //30密度              4位小数//以下4个参数这里有16位修改为32位查找表需要改变 
    uint32_t    coeff;              //31修正系数  4位小数
    uint32_t    pzero;              //32压力零点  
    uint32_t    pcoeff;             //33压力系数  2位小数      
    
    uint32_t    modnum;     		//34修改记录流水号     
    uint32_t    upmodnum;   		//35已传修改记录流水号        
    
    //班组信息
    uint8_t     shiftnum;           //36班号
    uint8_t     shiftmaxnum;        //37最大班号
    uint32_t    shiftransnum;       //38班流水号     
    uint32_t    shiftuptransnum;    //39已传班流水号     
    uint32_t    shiftfueltimes;     //40班加气次数
    uint32_t    shiftstartvol;      //41班开始枪累    2位小数
    uint32_t    shiftstartmon;      //42班开始金额    2位小数
    uint32_t    shiftotvol;         //43班累计气量    2位小数
    uint32_t    shiftotmon;         //44班累计金额    2位小数
    uint8_t     shiftstarttime[6];  //45班开始时间   BCD
    uint8_t     shiftendtime[6];    //46班结束时间    BCD

    //通用信息
    uint8_t     shengcode[2];       //47省编码
    uint8_t     stationcode[4];     //48站编码
    uint8_t     citycode[2];        //49地市代码
    uint8_t     parentcode[4];      //50上级单位代码

    //卡信息
    uint32_t    cardmaxyue;         //51储蓄卡余额上限    2位小数
    uint32_t    cardminyue;         //52储蓄卡卡余额下限    2位小数
	uint32_t	accountcardmaxyue;	//53记账卡余额上限    2位小数
    uint8_t     cardpsamnum;        //54psam卡卡座选择
    uint16_t    cardmaxday;         //55卡未使用最大间隔天数
    uint8_t     cardcheck;          //56是否检验钢瓶
    uint8_t     usecard;            //57  0-不使用卡，1-必须用卡

    //波特率设置
    uint32_t    modbusbaud;         //58流量计波特率
    uint32_t    pcbaud;             //59后台波特率

    uint8_t     fuelmode;           //60加气模式
    uint8_t     printmode;          //61小票打印方式

    //版本编号设置
    uint8_t     pricever;           //62单价版本号
    uint8_t     blackver;           //63黑名单版本号
    uint8_t     whitever;           //64白名单版本号
    uint8_t     addbkver;           //65新增黑名单版本号
    uint8_t     delbkver;           //66新删黑名单版本号
    uint8_t     comminfover;        //67通用信息版本号
	uint8_t     lockver;            //68密钥版本号。
	uint32_t	blacknum;  	        //69黑名单数。
	uint32_t    blackaddnum;        //70新增黑名单数。
	uint32_t	blackdelnum;        //71新删黑名单数。
	uint32_t	whitenum;	        //72白名单数量。

    //密码设置
    uint32_t    menupwd;            //73菜单进入密码
    uint32_t    qcpwd;              //74质检密码
    uint32_t    testpwd;            //75测试密码，如按键、电磁阀等测试
    uint32_t    resetpwd;           //76恢复出厂密码
	uint32_t	onoffworkpwd;		//77上下班密码
    uint8_t     pwdboard[6];        //78加密板数据
    uint8_t     usepwdboard;        //79是否使用加密板    
	
	uint8_t		offlinemode;		//80脱机模式 1：脱机 0：在线
	uint8_t     lcdtype;            //81屏幕选择，0蓝屏，1大屏
	uint8_t     offonwork;          //82加气机上下班状态 1上班，0下班
    uint8_t     yesnolock;          //83加气机锁定标志  1锁定，0解锁
    uint8_t     simulation;         //84    1:模拟加气；0：正常加气。
    uint8_t     intnumflag;         //85    0:加气时不能取整 1:可以取整
    uint32_t    maxvolumeonce;      //86单次最大加气量
    uint32_t    maxmoneyonce;       //87单次最大加气金额
    uint8_t     devicetype;         //88设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint8_t     backup1[20];
    
    //LNG控制参数    
    uint16_t    backgascalc;        //201是否回气计量，0-不计量，1-计量
    uint16_t    precoolmode;        //202预冷方式，0-小循环，1-大循环
    uint16_t    precooltemp;        //203预冷临界温度 ，负数
    uint16_t    precoolgain;        //204预冷临界增益
    uint16_t    precooldensty;      //205预冷临界密度，4位小数，0-1.0000
    uint16_t    precooltimeout;     //206预冷超时时间
    uint16_t    delaystoptime;      //207刚开始加液时,不判断停机条件的延时时间
    uint16_t    lowratestop;        //208低流速停机
    uint16_t    highratestop;       //209高流速停机
    uint16_t    backdenstystop;     //210回气密度停机，4位小数，0-1.0000
    uint16_t    backgainstop;       //211回气增益停机
    uint16_t    pressurestop;       //212停机压力
    uint16_t    stoppumptime;       //213加液后延时停泵
    uint16_t    precoolfinishedtime;//214重新预冷延时
    uint16_t    backup4;            //215与CNG参数统一，LNG未用
    uint16_t    backup5;            //216与CNG参数统一，LNG未用
    uint16_t    backup6;            //217与CNG参数统一，LNG未用
//以下为备份参数，当增加一个系统参数后，
//需要删除以下对应的字节数，以保证crc正确
    uint8_t     backup2[30];
	/************************end**********************/
    uint16_t     crc;               //16位数据校验
}Sysparas; 

//全局变量
typedef struct GlobalVar
{
    uint8_t         U8Temp;         //临时存储u8整型值
    uint32_t        U32Temp;        //临时存储u32整型值
    
    uint32_t        KeyValue;       //按键值,全局变量
    uint32_t        KeyPreValue;    //先前按键值    
    
    uint8_t         MsgTime;        //用于显示某些简短的消息
    uint8_t         MsgFlag;        //等于1时，msg未处理
    uint8_t         displayonce;    //等于1时，界面只显示一次或只执行一次

    uint8_t         circulatefue;   //循环加气设置。1-循环，0-停止。
    uint8_t         fuelstep;       //加气步骤。  
    uint32_t        fixnum;         //定气量。
    uint32_t        fixmoney;       //定金额。
    uint8_t         fix;            //定量标志，1-定气量，2-定金额。  
    
    uint8_t         workstate;      //加气机工作状态    
    uint8_t         BeepCount;      //蜂鸣器响的时间 100ms为单位
    uint8_t         eepromflag;     //0-不需要更换，1-需要更换
    
    uint16_t        paraindex;      //参数索引号。
    uint32_t        xgtransnum;     //修改流水号变量，用于查询。
    uint32_t        uptransnum;     //上传流水时的已传流水号
    uint8_t         UploadIdCount;  //上传流水失败次数。
    uint8_t         comreceiveflag; //后台来命令标志。
    uint8_t         comfailcount;   //后台与主板通信是否正常标志，如果主板10s未收到后台信息，则表示已掉线。                
    
    uint8_t         modbus_g_error; //加液时，气相流量计出错次数
    uint8_t         modbus_l_error; //加液时，液相流量计出错次数
    uint8_t         pressure_error; //压力传感器错误次数

    uint32_t        beforedex;      //参数修改前的值。
    uint32_t        afterdex;       //参数修改后的值。
    uint8_t         pointnum;       //参数小数点个数。
      
    uint8_t			startstate;		//电子标签，启动状态，等于0xff时没有标签命令来临。
	uint8_t         startways;      //电子标签，启动方式。
	uint8_t         startnum[8];    //电子标签，启动序列号。
	uint8_t         handnum;		//电子标签，手持机编号。
			
    uint32_t        timeout;        //超时时间
    uint16_t        stoptimes;      //停机条件满足次数
    uint16_t        stoppumptime;   //加液后停泵延时    
	uint8_t         otherstate;     //一泵双机时，对方加液机状态0-对方加液，1-对方待机
	uint8_t         devicetest;     //等于1时在进行电磁阀测试
	uint8_t         precoolfinished;//预冷已经完成，加液时直接加液
	uint8_t         finishedtimeout;//预冷完成后超时时间

    float           starttotalizeG; //气象流量计开始总累
    float           starttotalizeL; //液象流量计开始总累
    float           pretotalG;      //气象流量计上一次总累，用于判断瞬时数据
    float           pretotalL;      //液象流量计上一次总累，用于判断瞬时数据
	
}GlobalVar;

typedef	struct	M1carddata
{
	uint8_t		serialnumber[4];        //厂商序列号	
	uint32_t    key;                    //用户设的卡密码
	uint8_t     keyA[6];
	uint8_t     keyB[6];
    uint8_t     existflag;              //是否有卡存在。
    uint8_t     checkflag;              //验证卡标志。
    uint8_t		checkresult;            //卡验证信息。
    uint16_t    debittimes;             //申请扣款次数。
    uint8_t     greycard;               //申请解灰标志。
    uint8_t     blackorwhite;           // 1-不是黑名单或者白名单卡；2-是黑名单或者白名单。
    uint8_t     shiftflag;              //换班标记    
    //块1.
	uint8_t		cardnum[8];				//BCD 卡号
	uint8_t		cardtype;				//卡类型
	uint8_t		pricelevel;				//单价等级
	uint8_t		companycode[4];			//BCD 公司编码
	uint8_t		Shiftfunction;			//换班功能
	uint8_t		jurisdiction;			//权限	
    //块2.
	uint8_t		stationcode[4];			//BCD 站编码
	uint8_t		mode;					//消费方式
	uint8_t		createcardtime[7];		//发卡时间YYYYMMDDHHMMSS
	uint8_t		reserve2[4];			//备用	
	//块4
		
	uint8_t		password[3];			//BCD 密码
	uint8_t		customername[13];		//客户姓名（左对齐右补空格）
    //块5	
	uint8_t		balancecrc;				//卡余额校验
	uint8_t		lastoptmode;			//最后一次操作方式
	uint8_t		lasttradetime[6];		//BCD 最后交易时间YYMMDDHHMMSS
	uint32_t	trademoney;				//交易额 2位小数		
	uint32_t	cardbalance;			//卡余额  2位小数	
    //块6.
	uint8_t		backupblock[16];		//备份块	
	//块8.	
	uint32_t	sumtimes;				//累计消费次数	
	uint32_t	summoney;				//累计消费金额  2位小数	
	uint32_t	sumvolme;				//累计消费气量  2位小数
	uint8_t		reserve8[4];			//备用
    //块9.
	uint32_t	sumchargetimes;			//累计充值次数
	uint32_t	sumchargemoney;			//累计充值金额
	uint8_t		reserve9[8];			//备用
    //块10.
	uint8_t		cardstate;				//卡状态
	uint8_t		gunnum;					//最后一次加气枪号
	uint32_t	transnum;				//最后一次加气流水号
	uint8_t		laststationcode[4];		//BCD最后一次加气站编码
	uint8_t		reserve10[6];			//备用
    //块12.
	uint8_t		cylindersdate[4];		//钢瓶到期日期
	uint8_t		carnum[9];				//车牌号
	uint8_t		reserve12[3];			//备用
}M1carddata;

extern M1carddata       m1card;        	   //卡结构数据。
extern uint32_t         syserr;            //系统异常
extern uint32_t         stopreason;        //停机原因

extern GlobalVar        globalvar;         //全局变量
extern ShiftRecordInfo  shiftrecordinfo;   //上下班记录
extern ShiftRecordInfo  shiftrecordinfo_tmp;
extern FuelRecordInfo   fuelrecordinfo;    //流水记录信息
extern FuelRecordInfo   fuelrecordinfo_tmp;
extern Sysparas         sysparas;          //系统参数
extern Sysparas         tmp_sysparas;      //用户修改系统参数时，临时存储需要修改的参数

#endif
