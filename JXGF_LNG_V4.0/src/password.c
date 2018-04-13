/*
 * GetPassword.c 
 * 说明：本文档函数用于加密板功能使用。
 * Created: 2016-5-11 13:51:16
 *  Author: yck
 */ 
#include "kernel.h"
uint8_t Password[6];
/***************************************************************************
 **name:GetGunNum().
 **功能：将枪号转化为BCD码-yck20160512。
****************************************************************************/
void GetGunNum(void)
{		
	Password[0]=sysparas.gunnum;
}

/***************************************************************************
 **name:GetTimeSec(void)
 **功能：将时间秒转化为BCD码-yck20160512。
****************************************************************************/
void GetTimeSec(void)
{
    Password[1]=time.second;
}

/***************************************************************************
 **name:GetGunSum(void)
 **功能：取枪累计消费金额最低字节-yck20160512。
****************************************************************************/
void GetGunSum(void)
{
	Password[2]=(uint8_t)sysparas.totmoney; // 枪累计消费金额最低字节。
}

/***************************************************************************
 **name:GetGunGas(void)
 **功能：取枪累计消费气量最低字节-yck20160512。
****************************************************************************/
void GetGunGas(void)
{
	Password[3]=(uint8_t)sysparas.totvolume;// 枪累计消费气量最低字节。
}

/***************************************************************************
 **name:GetTimeMin(void)
 **功能：将时间分转化为BCD码-yck20160512。
****************************************************************************/
void GetTimeMin(void)
{
	Password[4]=time.minute;          // 时间分。
}

/***************************************************************************
 **name:GetDataCrc(void)
 **功能：得到加密数据CRC校验码-yck20160512。
****************************************************************************/
void GetDataCrc(void)
{
	Password[5] = (uint8_t)CRC16Calculate(&Password[0], 5);   // CRC校验。
}

/***************************************************************************
 **name:LockPortInit(void)
 **功能：加密版端口初始化-yck20160512。
****************************************************************************/
void LockPortInit(void)
{
	S1_IN;         //S1键，设置为输入。
}

/***************************************************************************
 **name:GetLockdata(void)
 **功能：得到加密数据-yck20160512。
****************************************************************************/
void GetLockdata(void)
{
	GetGunNum(); // 抢号。
	GetTimeSec();// 秒。
	GetGunSum(); // 枪金额累最小字节。
	GetGunGas(); // 枪气量累最小字节。
	GetTimeMin();// 分。
	GetDataCrc();// CRC。
}

/***************************************************************************
 **name:LockDataStore(void)
 **功能：在Flash和加密版里面存储加密数据-yck20160512。
****************************************************************************/
void LockDataStore(void)
{
    uint8_t i1,i2;
    uint8_t buf[6];
    uint8_t tmp[6];
    GetLockdata(); //得到加密数据。
    
    At24C512WriteNByte(LOCK_ADDR,&Password[0],6);
    MsDelay(100);
    At24C128WriteNByte(0,        &Password[0],6);
    
    if(At24C128ReadNByte(0, buf, 6))
    {
        for(i1=0;i1<6;i1++)
        {
            if(Password[i1]!=buf[i1]) break;
        }
      
    }
    if(At24C512ReadNByte(LOCK_ADDR, tmp, 6))
    {
        for(i2=0;i2<6;i2++)
        {
            if(Password[i2]!=tmp[i2]) break;
        }
      
    }
    if((i1==6)&&(i2==6))
    {
        if(SaveSysparas(SYSPARA_ADDR,(uint8_t*)&sysparas.head,sizeof(Sysparas)))
        {
            SetSuccess();
        } 
		else
	    {
	    	sysparas.usepwdboard=0;
	        SetFault();
	    }
    }
    else
    {
    	sysparas.usepwdboard=0;
        SetFault();
    }
}

/***************************************************************************
 **name:LockDataCompare(void)
 **功能：密码比对。返回值：；0-比对失败；1-比对成功-yck20160512。
****************************************************************************/
unsigned char LockDataCompare(void)
{
	uint8_t temp_lock0[6];
	uint8_t temp_lock1[6];
	uint8_t i;
	At24C512ReadNByte(LOCK_ADDR, temp_lock0, 6);
    MsDelay(100);
    At24C128ReadNByte(0,         temp_lock1, 6);
	
	for (i=0;i<6;i++)                     //比对密码。
	{
		if (temp_lock0[i]!=temp_lock1[i])
		{
			return 0;                     //比对失败。
		}
	}
	return 1;                             //比对成功。
}

