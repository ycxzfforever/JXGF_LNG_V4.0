#ifndef __MODBUS_H
#define __MODBUS_H

#define  _MODBUS_G_AUTO_BUF_LEN		5 	
#define  _MODBUS_L_AUTO_BUF_LEN		5 

#define  MODBUS_OK			    	1


#define  MODBUS_TIMEOUT             500000

enum    e_modbus_type   //流量计类型
{
	MODBUS_TYPE_NO = 0,
	MODBUS_TYPE_RM,       // 1为罗斯蒙特
	MODBUS_TYPE_EH,       // 2为E+H
};

enum    e_modbus_addr   //流量计地址
{
    _MODBUS_L_ = 1,       // 1液相
    _MODBUS_G_ = 2,       // 2气相    
};


typedef struct MassFlowModbus
{
  uint8_t   sendbuf[10];    //向流量计发送数据buffer
  uint8_t   recbuf[50];     //接收流量计数据buffer
  uint8_t   flag;           //等于1时表示数据接收正确
}MassFlowModbus;
extern  MassFlowModbus      modbus;


typedef struct RealTimeData //读流量计和压力传感器实时数据
{    
    uint8_t           lcng;                  //CNG/LNG标记。
    uint32_t          transnum;              //当次流水号。
    uint8_t           workstatus;            //加气机工作状态。
    uint16_t          price;                 //当前交易单价。
    uint32_t          backgas;               //回气量
    uint32_t          volume;                //当前加气量。
    uint32_t          money;                 //当前加气金额。
    uint8_t           shiftnum;              //当前班号。
    uint8_t           employeenum[8];        //当前员工卡号。
    uint8_t           cardnum[8];            //当前用户卡卡号。
    uint8_t           carnum[9];             //车牌号。
    uint8_t           cardtype;              //卡类型。
    uint8_t           transactiontype;       //交易类型，1字节。
    uint8_t           starttime[7];          //加气开始时间。
    uint8_t           endtime[7];            //加气结束时间。
    uint32_t          reallymoney;           //本次实际扣款金额。
    uint32_t          beforebalance;         //加气前余额，4字节。
    uint32_t          afterbalance;          //加气后余额，4字节。
    uint32_t          allrechargetimes;      //累计充值次数，4字节。
    uint32_t          allrechargemoney;      //累计充值金额，4字节。
    uint32_t          allconsumptiontimes;   //累计消费次数，4字节。
    uint32_t          allconsumptionnum;     //累计消费气量，4字节。
    uint32_t          allconsumptionmoney;   //累计消费金额，4字节。
    uint16_t          beforepressure;        //加气前压力，2字节。
    uint16_t          afterpressure;         //加气后压力，2字节。
    
    uint32_t          curnum;                //当量，4字节。修正系数。
    uint8_t           measuretype;           //计量单位，1字节，1-标方，0-Kg。
    uint8_t           stopreason;            //停机原因。 
    uint32_t          jhid;                  //解灰相关流水号。
    uint32_t          classid;               //班流水号。
    uint8_t           fuelmode;              //启动方式
    uint32_t          cur_pressure;          //当前压力，压力读出时就已经扩大了100倍。
    uint8_t           powerdown;             //是否掉电，0-正常；1-掉电。
    float             cur_flowrate_l;        //当前液相流速
    float             cur_temperature_l;     //当前液相温度
    float             cur_totalize_l;        //当前液相总量
    float             cur_density_l;         //当前液相密度
    float             cur_gain_l;            //当前液象增益
    
    float             cur_flowrate_g;        //当前气相流速
    float             cur_temperature_g;     //当前气相温度    
    float             cur_totalize_g;        //当前气相总量      
    float             cur_density_g;         //当前气相密度    
    float             cur_gain_g;            //当前气象增益        
}RealTimeData;


uint16_t ModbusCrc16(uint8_t *buf, uint16_t Len);
uint8_t  ModbusReadReg(uint8_t modbus_addr,uint16_t RegAddr, uint8_t regnum );
uint8_t  ModbusReadScanReg(uint8_t modbus_addr,uint16_t RegAddr, uint8_t regnum);
uint8_t  ModbusWriteReg(uint8_t modbus_addr, uint8_t modbus_coil, uint16_t reg_id, uint16_t v_value);
uint8_t  ModbusReadDeviceAddress(uint8_t modbus_addr,uint16_t *rdaddress);
uint8_t  ModbusReadTotal(uint8_t modbus_addr,float *total);
uint8_t  ModbusReadFlow(uint8_t modbus_addr,float *flow);
uint8_t  ModbusClearTotal(uint8_t modbus_addr);
uint8_t  ModbusInitReg(void);
void     ModbusReadScanBuffer(uint8_t modbus_addr);
uint8_t  ModbusComTest(uint8_t modbus_addr);
void     ModbusDataDisposs(void);

void     UART1_IRQHandler( void );
void     ModbusRecDataCheck(void);


#endif

