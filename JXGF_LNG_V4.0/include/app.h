#ifndef APP_H_
#define APP_H_

extern uint8_t    LoadSuccess ;      //下载数据是否完成。
extern void StrReverse(uint8_t *buf,uint8_t num,uint8_t n);
extern void BufReverse2(uint8_t *buf, uint8_t num);
extern uint8_t SendBack485Buf[BUFSIZE];
extern uint8_t RecBack485Buf[BUFSIZE];
void ClearBuf(volatile uint8_t *buf,uint8_t num,uint16_t len);
bool RecBlckDataCheck(void);
void GetRep51Data(void);
void UpLoad51H(void);
void UpLoad52H(void);
void UpLoad53H(void);
void UpLoad54H(void);
void UpLoad55H(uint8_t type);
void UpLoad56H(uint8_t type);
void UpLoad5AH(void);
void UpLoad5BH(void);
void UpLoad5CH(void);
void UpLoad5EH(void);
void BGCommunication(uint8_t data);
/********************************************************
**结构名称:Reply51
**结构功能:后台51巡检命令来临时，加气机回应后台的数据结构。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/  
typedef struct 
{
    uint8_t   LCNG;                   //CNG/LNG标记
    uint8_t   status;                 //加气机状态,   1字节。
    uint16_t  price;                  //交易单价，    2字节。
    uint32_t  gasnum;                 //当前加气气量，4字节。
    uint32_t  gasmoney;               //当前加气金额，4字节。
    uint8_t   classnum;               //班号，    1字节。
    uint8_t   employeenum[8];         //员工号码，8字节，BCD码。
    uint8_t   cardnum[8];             //用户卡码，8字节，BCD码。
    uint8_t   carnum[9];              //车牌号码，7字节，ASCII码。
    uint16_t  curpressure;            //当前压力，2字节。
    float     inflowrate;             //进气流量，4字节。
    float     intemperature;          //进气温度，4字节。
    float     inamount;               //进气质量，4字节。
    float     indensity;              //进气密度，4字节。
    float     ingain;                 //进气增益，4字节。
    float     returnflowrate;         //回气流量，4字节。
    float     returntemperature;      //回气温度，4字节。
    float     returnamount;           //回气质量，4字节。
    float     returndensity;          //回气密度，4字节。
    float     returngain;             //回气增益，4字节。
    uint32_t  systemalarm;            //系统报警，4字节。
}Reply51;

extern Reply51 UpLoad51Data; 

/********************************************************
**结构名称:Reply52
**结构功能:52命令，加气机上传交易记录。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint32_t  transactionid;          //交易流水号，4字节。
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
    uint32_t  gasnum;                 //当前加气气量，4字节。
    uint32_t  gasmoney;               //当前加气金额，4字节。
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
    uint32_t  curnum;                 //当量，4字节。
    uint8_t   devicetype;             //设备类型0： LNG 加气机1： CNG 加气机2： 加气柱3： 卸气柱
    uint8_t   measuretype;            //计量单位，1字节，1-标方，0-Kg。
    uint8_t   stopreason;             //停机原因。 
    uint32_t  jhid;                   //解灰相关流水号。
    uint32_t  classid;                //班流水号。
}Reply52;

extern Reply52 UpLoad52Data; 

/********************************************************
**结构名称:Reply53
**结构功能:53命令，加气机请求下载数据。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct
{
    uint8_t  loadtype;               //需要下载数据的类型，1字节。
    uint8_t  presurve[6];            //保留，6字节。
}Reply53;

extern Reply53 UpLoad53Data; 

/********************************************************
**结构名称:Reply54
**结构功能:54命令，加气机请求下载数据内容。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct
{
    uint8_t   loadtype;               //需要下载数据的类型，1字节。
    uint16_t  partoffset;             //已下载卡号个数。
    uint8_t   partnum;                //申请下载卡号个数。
}Reply54;

extern Reply54 UpLoad54Data; 

/********************************************************
**结构名称:Reply55
**结构功能:55命令，加气机申请解灰。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct
{
    uint8_t  type;                   //类型。
    uint8_t  cardtype;               //卡类型。新加的，20160728。
    uint8_t  cardnum[8];             //卡号，BCD码，8字节。
    uint32_t gasnum;                 //当前加气气量。
    uint32_t money;                  //当前加气金额。
    uint32_t cardover;               //卡余额。
    uint8_t  stationnum[4];          //站编码，BCD码。
    uint8_t  gunnum;                 //枪最后一次加气枪号。
    uint32_t gunnumber;              //卡最后一次加气枪的加气记录流水号。
   
}Reply55;

extern Reply55 UpLoad55Data; 

typedef struct	
{
	uint8_t		greyrecord;		//是否找到灰记录0：未找到 1：找到
	uint8_t		type;			//类型0：申请扣款 1：申请解灰
	uint8_t		cardnum[8];		//卡号BCD
	uint32_t	realmoney;		//实际加气金额
	uint32_t	realtrademoney;	//实际扣款金额
	uint32_t	beforemoney;	//加气前余额
	
	uint32_t	aftermoney;		//加气后余额
	uint16_t	price;			//单价
	
	uint32_t	volume;			//气量
	uint32_t	totaltimes;		//累计消费次数
	uint32_t	totalmoney;		//累计消费金额
	
	uint32_t	totalvolume;	//累计消费气量
	uint32_t	removegrey;		//解灰相关流水内码
}	Receive55data;

extern Receive55data receive55data;

/********************************************************
**结构名称:Reply56
**结构功能:56命令，加气机申请查询黑白名单。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t  cardnum[8];             //卡号，BCD码，8字节。
    uint8_t  seekname;               //申请查黑/白，0-查黑，1-查白。
}Reply56;

extern Reply56 UpLoad56Data;  

/********************************************************
**结构名称:Reply57
**结构功能:57命令，加气机返回累计数给后台系统。

**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint32_t  transactionid;          //交易流水号，4字节。
    uint32_t  allgasnum;              //加气气量总累计。
    uint32_t  allgasmoney;            //加气金额总累计。
    uint32_t  allclassnum;            //班升数累计。
    uint32_t  allclassmoney;          //班金额累计。
}Reply57;

extern Reply57 UpLoad57Data; 

/********************************************************
**结构名称:Reply58
**结构功能:58命令，后台系统读加气机指定流水记录。
          加气机返回流水记录给后台系统，有以下两种情况：
          a. 若有该记录，则按照命令字“52H”消息体内容回复；
          b. 若没有该记录，则回复如下结构数据。
**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint32_t  transactionid;          //交易流水号，4字节。
    uint8_t   ifid;                   //是否有该笔流水记录。 
}Reply58;

extern Reply58 UpLoad58Data;
/********************************************************
**结构名称:Reply59
**结构功能:59命令，加气机返回加气机信息给后台系统。
       
**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t  gunnum;                 //枪号,BCD码。
    uint32_t transactionid;          //交易流水号，4字节。
    uint8_t  baseblackversion;       //基础黑名单版本号。
    uint8_t  newaddblackversion;     //新增黑名单版本号。
    uint8_t  newdelblackversion;     //新删黑名单版本号。
    uint8_t  whitelistversion;       //白名单版本号。
    uint8_t  priceversion;           //单价版本号。
    uint8_t  baseinfoversion;        //通用信息版本号。
    uint8_t  stationnum[4];          //站编码，BCD码。
    uint8_t  provincenum[2];         //省代码，BCD码。
    uint8_t  citynum[2];             //地市代码，BCD码。
    uint8_t  parentunitnum[4];       //上级单位代码，BCD码。
    uint8_t  stationname[40];        //加气站名称，ASCII码。
    uint8_t  hardsoftwareversion[16];//嵌入式软件版本号,ASCII码。
    uint8_t  manufacturerinfo[40];   //加气机厂商信息，ASCII码。
}Reply59;

extern Reply59 UpLoad59Data; 


/********************************************************
**结构名称:Reply5A 
**结构功能:5A命令，加气机上传上下班记录。
       
**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
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
}Reply5A;

extern Reply5A UpLoad5AData; 

/********************************************************
**结构名称:Reply5B 
**结构功能:5B命令，加气机上传参数修改记录。
       
**创 建 者:杨传坤
**创建日期:2016-05-17

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint32_t  modifyid;               //参数修改记录流水号，4字节。
    uint8_t   classnum;               //班号，1字节。
    uint8_t   cardstaffnum[8];        //维护卡、质检卡等，BCD码。
    uint16_t  paraindex;              //参数索引号。
    uint8_t   dexnum;                 //小数位数。
    uint32_t  beforeparameter;        //修改前参数值。 
    uint32_t  afterparameter;         //修改后参数值。    
    uint8_t   modifytime[7];          //修改时间，7字节，BCD码。   
}Reply5B;

extern Reply5B UpLoad5BData; 

/********************************************************
**结构名称:Reply5C 
**结构功能:5C命令，加气机验证IC卡可用性。
       
**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t   cardnum[8];       //IC卡卡号，BCD码。
    uint32_t  balance;          //余额。
    uint8_t   endtime[7];       //最后交易时间。
}Reply5C;

extern Reply5C UpLoad5CData; 

/********************************************************
**结构名称:Reply5D 
**结构功能:5D命令，加气机返回下班结果。
       
**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t OffWork;        //下班是否成功。
}Reply5D;

extern Reply5D UpLoad5DData;

/********************************************************
**结构名称:Reply5E 
**结构功能:5E命令，加气机申请下发单价。
       
**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t lcng;        //气品代码。0：lng；1：cng。
}Reply5E;

extern Reply5E UpLoad5EData;


/********************************************************
**结构名称:Reply5F 
**结构功能:5F命令，加气机返回是否锁定结果。
       
**创 建 者:杨传坤
**创建日期:2016-06-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t commdwork;        //是否锁定加气机。
}Reply5F;

extern Reply5F UpLoad5FData;
/********************************************************
**结构名称:BWInfo 
**结构功能:黑白名单版本信息。
       
**创 建 者:杨传坤
**创建日期:2016-07-8

**修改者  :
**修改日期:
**修改内容:
********************************************************/ 
typedef struct 
{
    uint8_t version;      //版本号。   
    uint8_t starttime[4]; //生效日期。
    uint8_t stoptime[4];  //截止日期。
    uint8_t effectarea[2];//黑白名单有效区域。
    uint32_t namenum;     //名单数量。
    uint8_t reserve;      //保留。
}BWInfo;

extern BWInfo bwinfo;
#endif
