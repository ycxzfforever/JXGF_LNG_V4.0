#include "kernel.h"

/*************************************************************************
** 函数名称:OsplcPortInit
** 函数功能：初始化客显端口。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-28

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcPortInit(void)
{
   OsplcLckOut();   //lck输出。	
   OsplcClkOut();   //clk输出。
   OsplcDataOut();  //数据输出。
    
   OsplcLck_H();    //lck输出1。		
   OsplcClk_H();    //clk输出1。
   OsplcData_H();   //data输出1。
}

/*************************************************************************
** 函数名称:OsplcLatch
** 函数功能：当将所有数据送入LED显示后，使用此函数进行数据的锁存。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-28

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcLatch(void)
{
    OsplcLck_L();  //下降沿有效。硬件电路有方向器，故取反。  
    UsDelay(5);    //此延时尚未精准。必须的延时，因为触发器输入输出延时受电源电压影响较大。
    
    OsplcLck_H();  //下降沿有效。硬件电路有方向器，故取反。
    UsDelay(5);    //此延时尚未精准。必须的延时，因为触发器输入输出延时受电源电压影响较大。
    
}

/*************************************************************************
** 函数名称:OsplcEnclk
** 函数功能：控制SPLC的一次时钟使能函数。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-28

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcEnclk(void)
{
    OsplcClk_H();   //资料为下降沿有效。
    UsDelay(5);     //此延时尚未精准。必须的延时，因为触发器输入输出延时受电源电压影响较大。
    
    OsplcClk_L();
    UsDelay(5);     //此延时尚未精准。必须的延时，因为触发器输入输出延时受电源电压影响较大。
    
}

/*************************************************************************
** 函数名称:OsplcDot
** 函数功能：向蓝屏写入小数点。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcDot(uint8_t dcode)
{
    uint8_t i,temp;
    for(i=0;i<6;i++)
    {
        temp=(dcode<<i)&0x20;
        if(temp)
        {
            OsplcData_H();
        }
        else
        {
            OsplcData_L();
        }
        OsplcEnclk();
    }
}

/*************************************************************************
** 函数名称:OsplcDispB
** 函数功能：向蓝屏液晶写入一帧的数据。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcDispB(uint8_t *pdisp,bool nPoint)
{
    uint8_t SEGMAP[16] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x8C,0x86,0xFF,0xC6,0x00,0xBF};
    uint8_t i, j, showdata;
    if(nPoint)
    {
        OsplcDot(0x2a);                       //小数点编码。
    }
    else
    {
        OsplcDot(0x00);                       //小数点全显。
    }
    for(i=0;i<22;i++)                          //可显示22个位置，除开小数点。
    {
        showdata = SEGMAP[pdisp[i]];
        for(j=0;j<7;j++)
        {
            showdata = showdata << 1;
            if(showdata & 0x80)
            {
                OsplcData_H();
            }
            else
            {
                OsplcData_L();
            }
             OsplcEnclk();                    //时钟使能。
        }
    }
    OsplcLatch();                             //数据锁存。
}

/*************************************************************************
** 函数名称:OsplcShowB
** 函数功能：向蓝屏液晶某行写入数据。
** 入口参数：Row1：第一行要写入的数据；
             Row2：第二行要写入的数据；
             Row3：第三行要写入的数据。
             bStop：负号标志。
             bOverFlag：第一行，第二行第一个字符显示标志。
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcShowB(uint32_t Row1,uint32_t Row2,uint16_t Row3,bool bStop, bool bOverFlag)
{
    char buf[12] = {0};
    uint8_t dispbuf[22] = {0};       //蓝屏液晶共显示22个数字，数组排序为最低行，从右向左开始递增，依次往上递增。
    uint8_t m = 0, n = 0, len = 0;
    //uint32_t row;
    memset(dispbuf,12,sizeof(dispbuf));
   
    dispbuf[5] = sysparas.gunnum/10;                        //显示抢号。
    dispbuf[4] = sysparas.gunnum%10; 
    if(bOverFlag)  
    {
        dispbuf[13]  = 10;	               //第二行第一位显示P。
        dispbuf[21] =  13;	               //第一行第一位显示C。
    }
    /************************************************************************
    ** 第一行。
    *************************************************************************/
    memset(buf,13,sizeof(buf));
    //row=(uint32_t)(Row1*100);
    len = sprintf(buf,"%03d",Row1);         //将第一行数据写入数组，并得到长度。

    len = (7>len) ? len : 7;
    m=len;
    for(n=0; n<len; n++)
    {
        dispbuf[13+m] = buf[n]-0x30;	   //将要显示的数据存入数组。
        m--;
    }
    if(bStop)                              //第一行负数标志。如果是负数的话，就显示负数。
    {
        dispbuf[len +13] = 15;
    }
    /************************************************************************
    ** 第二行。
    *************************************************************************/
    memset(buf,0,sizeof(buf));
    //row=(uint32_t)(Row2*100);
    len = sprintf(buf,"%03d",Row2);        //将第一行数据写入数组，并得到长度。
    len = (7>len) ? len : 7;
    m=len;
    for(n=0; n<len; n++)
    {
        dispbuf[5+m] = buf[n]-0x30;	       //将要显示的数据存入数组。
        m--;
    }
    if(bStop)
    {
        dispbuf[len +5] = 15;             //第二行负数标志。如果是负数的话，就显示负数。
    }
    /************************************************************************
    ** 第三行。
    *************************************************************************/
    memset(buf,0,sizeof(buf));
    //row=(uint32_t)(Row3*100);
    len = sprintf(buf,"%03d",Row3);          //将第一行数据写入数组，并得到长度。
    len = (4>len) ? len : 4;
    m=len;
    for(n=0; n<len; n++)
    {
        m--;
        dispbuf[m] = buf[n]-0x30;	      //将要显示的数据存入数组。
    }
    OsplcDispB(dispbuf,true); 
}

/*************************************************************************
** 函数名称:OsplcHardwarechk
** 函数功能：检测液晶是否能正常工作。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcHardwarechk(void)
{
    if(sysparas.lcdtype==0)                            //测试蓝屏。
    {
        uint8_t disp[22] = {0}; 
        uint8_t  m =0;
        for(m=0;m<=9;m++)
        {
            memset(&disp[0],m,22);
            OsplcDispB(disp,true); //显示。
            WATCH_DOG();
            MsDelay(200);
        }
        OsplcShowB(fuelrecordinfo.volume,fuelrecordinfo.money,fuelrecordinfo.price,0,1);
    }   
    else                            //测试大屏。
    {
        OsplcDispInit();
        OsplcShowN(fuelrecordinfo.volume,fuelrecordinfo.money,fuelrecordinfo.price);
    } 
}

/*************************************************************************
** 函数名称:OsplcInit
** 函数功能：初始化客显液晶。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcInit(void)
{
    OsplcPortInit();
    OsplcHardwarechk();
}

/*************************************************************************
** 函数名称:OsplcDispN
** 函数功能：向大屏液晶写入一帧的数据。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcDispN(void)                
{
    uint8_t m, n, showdata;
    for(m = 0; m < 40; m++)
    {
        showdata = DispMem[m];
        for(n = 0; n < 8; n++)
        {
            if(showdata & 0x80) 
            {
                OsplcData_H();
            }
            else
            {
                OsplcData_L();
            }
            OsplcEnclk();
            showdata = showdata << 1;
        }
    }
    OsplcLatch();
}

/*************************************************************************
** 函数名称:OsplcDispInit
** 函数功能：初始化大屏液晶。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-04-5

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcDispInit(void)
{
    uint32_t temp1[10]= {00000000,11111111,22222222,33333333,44444444,55555555,66666666,77777777,88888888,99999999};
    uint32_t temp2[10]= {0000,1111,2222,3333,4444,5555,6666,7777,8888,9999};
    uint32_t temp3[10]= {000,111,222,333,444,555,666,777,888,999};
    uint8_t temp4[10]= {00,11,22,33,44,55,66,77,88,99};
    float temp5[10]= {00.0,11.1,22.2,33.3,44.4,55.5,66.6,77.7,88.8,99.9};
    uint8_t i;

    memset(DispMem,0xFF,sizeof(DispMem));
    OsplcTitleOn();
    for(i = 0; i <10; i++)
    {
        WATCH_DOG();
        OsplcPrice(temp1[i]);
        OsplcNum(temp1[i]);
        OsplcSum(temp1[i]);
        OsplcFlRate(temp2[i]);
        OsplcFlrateBack(temp2[i]);
        OsplcDrvgain(temp3[i]);
        OsplcPressure(temp3[i]);
        OsplcTemperature(-temp5[i]);
        OsplcWarnning(temp4[i]);
        OsplcGunNum(temp4[i]);
        if(i==0)
            memset(DispMem,0x0,sizeof(DispMem));
        OsplcDispN();
        WATCH_DOG();
        MsDelay(200);  
    }
}

/*************************************************************************
** 函数名称:OsplcTitleOn
** 函数功能：点亮液晶上的名称，单位等位置。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcTitleOn(void)
{
    OsplcKGON();       //气量
    OsplcSUMON();      //金额    
    OsplcPRICEON();    //单价   
    OsplcKGMINON();    //进气KG/MIN   
    OsplcWARNINGON();  //报警 
    OsplcGNUMON();     //枪号   
    OsplcFLNUMON();    //进气流量S9     
    OsplcSTATEON();    //回气流量S10  
    OsplcSTATEUPAYON();//回气KG/MIN
    OsplcSTATEIDLEON();//增益    
    OsplcTEMPON();     //温度 
    OsplcTEMPUNITON();   
    OsplcPRESSUREON(); //压力
    OsplcLOGOON();     //LOGO

}

uint16_t OsplcNumArr[8][12] =
{
    {0x2120, 0xF901,0x46FE,0xF6FF,0x27FE,0xA6FE,0x96FF,0x8EFE,0x0eFE,0xE6FF,0x06FE,0x86FE}, 
    {0x20FF, 0xFE00,0x21FF,0xF9FF,0x13FF,0x51FF,0xC9FF,0x45FF,0x05FF,0xF1FF,0x01FF,0x41FF},
    {0x1F1E, 0x9E03,0x61FE,0xF9FF,0x73FC,0x71FD,0xE9FD,0x65FD,0x65FC,0xF1FF,0x61FC,0x61FD}, 
    {0x1F1E, 0x20FC,0xDF23,0xDFFB,0xFF13,0xDF53,0xDFCB,0xDF47,0xDF07,0xDFF3,0xDF03,0xDF43}, 
    {0x04FF, 0x7F00,0x90FF,0xFCFF,0x89FF,0xA8FF,0xE4FF,0xA2FF,0x82FF,0xF8FF,0x80FF,0xA0FF}, 
    {0x0403, 0x803F,0x7FC8,0x7FFB,0xFFC1,0x7FD1,0x7FF2,0x7FD4,0x7FC4,0x7FF9,0x7FC0,0x7FD0}, 
    {0x0302, 0xC01F,0x3FE4,0x3FFF,0x7FE2,0x3FEA,0x3FF9,0xBFE8,0xBFE0,0x3FFE,0x3FE0,0x3FE8}, 
    {0x0201, 0xE00F,0x1FF2,0x9FFF,0x3FF1,0x1FF5,0x9FFC,0x5FF4,0x5FF0,0x1FFF,0x1FF0,0x1FF4}, 
};

void OsplcNumFunc(uint8_t id,uint8_t num)
{
    uint8_t hB, lB;
    hB = OsplcNumArr[id][0]/0x100;
    lB = OsplcNumArr[id][0]%0x100;
    
    DispMem[hB] |= OsplcNumArr[id][1]/0x100;
    if(num <= 9)
    {
        DispMem[hB] &= OsplcNumArr[id][num+2]/0x100;
    }
        
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcNumArr[id][1]%0x100;
        if(num <= 9)
        {
            DispMem[lB] &= OsplcNumArr[id][num+2]%0x100;
        }   
    }
}

/*************************************************************************
** 函数名称:OsplcNum
** 函数功能：大屏液晶气量显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcNum(uint32_t vnum)
{
    uint8_t num, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(vnum < 100)
    {
        OsplcNumFunc(7,10);
        OsplcNumFunc(6,10);
        OsplcNumFunc(5,10);
        OsplcNumFunc(4,10);
        OsplcNumFunc(3,10);
        OsplcNumFunc(2,0);
        OsplcNumFunc(1,vnum/10);
        OsplcNumFunc(0,vnum%10);
    }
    else
    {
        len=sprintf(buf,"%d",vnum);
        len = (9>len) ? len : 8;
        if(len < 3) 
        {
            return false;
        }
        m = len-1;
        for(n=0; n<len; n++)
        {
            num = buf[n]-0x30;
            OsplcNumFunc(m,num);
            m--;
        }
    }
    DispMem[31] &= 0xFE;
    return true;
}

uint16_t OsplcSumArr[8][12] =
{
    {0x1B1A, 0xF007,0x0FF9,0xCFFF,0x9FF8,0x8FFA,0x4FFE,0x2FFA,0x2FF8,0x8FFF,0x0FF8,0x0FFA}, //NUM16
    {0x1A19, 0xF803,0x87FC,0xE7FF,0x0FFD,0x47FD,0x67FE,0x57FC,0x17FC,0xE7FD,0x07FC,0x47FC}, //NUM15
    {0x2219, 0x03F8,0xFC87,0xFFE7,0xFC4F,0xFD47,0xFF27,0xFD17,0xFC17,0xFFC7,0xFC07,0xFD07}, //NUM14
    {0x2221, 0xF806,0x87F9,0xE7FF,0x4FF9,0x47FD,0x27FF,0x17FD,0x17F9,0xC7FF,0x07F9,0x07FD}, //NUM13
    {0x0100, 0xF007,0x4FF8,0xEFFD,0x9FF8,0xAFF8,0x2FFD,0x2FFA,0x0FFA,0xEFFC,0x0FF8,0x2FF8}, //NUM12
    {0x1800, 0x03F8,0xFC87,0xFFE7,0xFC4F,0xFD47,0xFF27,0xFD17,0xFC17,0xFFC7,0xFC07,0xFD07}, //NUM11
    {0x1817, 0xFC04,0x43FB,0xF3FF,0x27FB,0xA3FB,0x93FF,0x8BFB,0x0BFB,0xE3FF,0x03FB,0x83FB}, //NUM10
    {0x17FF, 0xFBFF,0x24FF,0xFCFF,0x16FF,0x54FF,0xCCFF,0x45FF,0x05FF,0xF4FF,0x04FF,0x44FF}, //NUM9
};

void OsplcSumFunc(uint8_t id, uint8_t sum)
{
    uint8_t hB, lB;
    
    hB = OsplcSumArr[id][0]/0x100;
    lB = OsplcSumArr[id][0]%0x100;
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcSumArr[id][1]%0x100;
        if(sum <= 9)
            DispMem[lB] &= OsplcSumArr[id][sum+2]%0x100;
    }
    DispMem[hB] |= OsplcSumArr[id][1]/0x100;
    if(sum <= 9)
        DispMem[hB] &= OsplcSumArr[id][sum+2]/0x100;
}

/*************************************************************************
** 函数名称:OsplcSum
** 函数功能：大屏液晶金额显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcSum(uint32_t fvsum)
{
    uint8_t sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(fvsum < 100)
    {
        OsplcSumFunc(7,10);
        OsplcSumFunc(6,10);
        OsplcSumFunc(5,10);
        OsplcSumFunc(4,10);
        OsplcSumFunc(3,10);
        OsplcSumFunc(2,0);
        OsplcSumFunc(1,fvsum/10);
        OsplcSumFunc(0,fvsum%10);
    }
    else
    {
        len=sprintf(buf,"%d",fvsum);
        len = (9>len) ? len : 8;
        if(len <3) 
        {
            return false;
        }
        m = len-1;
        for(n=0; n<len; n++)
        {  
            sum = buf[n]-0x30;
            OsplcSumFunc(m,sum);
            m--;
        }
    }
    DispMem[25] &= 0xFB;
    return true;
}

uint16_t DispPriceArr[5][12] =
{
    {0x1D1C, 0xF806,0x87F9,0xE7FF,0x4FF9,0x47FD,0x27FF,0x17FD,0x17F9,0xC7FF,0x07F9,0x07FD}, //气量NUM21
    {0x1C1B, 0xF901,0x46FE,0xF6FF,0x27FE,0xA6FE,0x96FF,0x8EFE,0x0EFE,0xE6FF,0x06FE,0x86FE}, //气量NUM20
    {0x0B1B, 0x0F0E,0xF2F1,0xFFF9,0xF1F3,0xF5F1,0xFCF9,0xF4F5,0xF0F5,0xFFF1,0xF0F1,0xF4F1}, //气量NUM19
    {0x0CFF, 0xFEFF,0x09FF,0x3FFF,0x45FF,0x15FF,0x33FF,0x91FF,0x81FF,0x3DFF,0x01FF,0x11FF}, //气量NUM18
    {0x0D0C, 0xF901,0x16FE,0x7FFE,0x8EFE,0x2EFE,0x67FE,0x26FF,0x06FF,0x7EFE,0x06FE,0x26FE}, //气量NUM17
};

void OsplcPriceFunc(uint8_t id, uint8_t pnum)
{
    uint8_t hB, lB;
    
    hB = DispPriceArr[id][0]/0x100;
    lB = DispPriceArr[id][0]%0x100;
    
    DispMem[hB] |= DispPriceArr[id][1]/0x100;
    if(pnum <= 9)
    {
        DispMem[hB] &= DispPriceArr[id][pnum+2]/0x100;
    }
    
    if(lB != 0xFF)
    {
        DispMem[lB] |= DispPriceArr[id][1]%0x100;
        if(pnum <= 9)
        {
            DispMem[lB] &= DispPriceArr[id][pnum+2]%0x100;
        }          
    }
}

/*************************************************************************
** 函数名称:OsplcPrice
** 函数功能：大屏液晶价格显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcPrice(uint32_t vprice)
{
    uint8_t sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(vprice < 100)
    {
        OsplcPriceFunc(4,10);
        OsplcPriceFunc(3,10);
        OsplcPriceFunc(2,0);
        OsplcPriceFunc(1,vprice/10);
        OsplcPriceFunc(0,vprice%10);
    }
    else
    {
        len=sprintf(buf,"%d",vprice);
        len = (6>len) ? len : 5;
        if(len <3) 
        {
            return false;
        }
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            OsplcPriceFunc(m,sum);
            m--;
        }
    }
    DispMem[11] &= 0xEF;
    return true;
}

/*************************************************************************
** 函数名称:OsplcGunNum
** 函数功能：大屏液晶枪号显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcGunNum(uint8_t GunNum) //NUM24 NUM25
{
    uint8_t OsplchGarr[10] =  {0x09, 0x3F, 0x45, 0x15, 0x33, 0x91, 0x81, 0x3D, 0x01, 0x11};
    uint8_t OsplclGarr[10] =  {0x81, 0x9F, 0xA2, 0x8A, 0x9C, 0xC8, 0xC0, 0x9B, 0x80, 0x88};

    if(GunNum >99) 
    {
        return;
    }
    DispMem[19] |= 0xFE; // DispMem[19] &= OsplchGarr[GunNum%10];
    if(GunNum >= 10)
    {
        DispMem[19] &= OsplchGarr[GunNum/10];
    }
    else
    {
        DispMem[19] &= OsplchGarr[0];
    }
    DispMem[18] |= 0x7F;
    DispMem[18] &= OsplclGarr[GunNum%10];
}

uint16_t OsplcFrateArr[4][12] =
{
    {0x0EFF, 0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF}, //??NUM29
    {0x100F, 0x20FC,0xDF0B,0xFF3F,0xDF47,0xDF17,0xFF33,0xDF93,0xDF83,0xDF3F,0xDF03,0xDF13}, //??NUM28
    {0x100F, 0x9E03,0x69FC,0x7FFE,0x65FD,0x75FC,0x73FE,0xF1FC,0xE1FC,0x7DFE,0x61FC,0x71FC}, //??NUM27
    {0x1110, 0xFC01,0x13FE,0x7FFE,0x8BFE,0x2BFE,0x67FE,0x23FF,0x03FF,0x7BFE,0x03FE,0x23FE}, //??NUM26
};

void OsplcFrateFunc(uint8_t id, uint8_t frnum)
{
    uint8_t hB, lB;
    
    hB = OsplcFrateArr[id][0]/0x100;
    lB = OsplcFrateArr[id][0]%0x100;
    
    DispMem[hB] |= OsplcFrateArr[id][1]/0x100;
    if(frnum <= 9)
    {
        DispMem[hB] &= OsplcFrateArr[id][frnum+2]/0x100;
    }
    
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcFrateArr[id][1]%0x100;
        if(frnum <= 9)
            DispMem[lB] &= OsplcFrateArr[id][frnum+2]%0x100;
    }
    
}

/*************************************************************************
** 函数名称:OsplcFlRate
** 函数功能：大屏液晶进气流量显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcFlRate(uint32_t nfrNum)
{
    uint8_t sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
  
    if(nfrNum < 100)
    {
      
        OsplcFrateFunc(1,nfrNum/10);
        OsplcFrateFunc(0,nfrNum%10);
    }
    else
    {
        len=sprintf(buf,"%d",nfrNum);
        len = (5>len) ? len : 4;
        if(len <3) 
        {
            return false;
        }
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            OsplcFrateFunc(m,sum);
            m--;
        }
    }
    DispMem[16] &= 0xBF;
    return true;
}

/*************************************************************************
** 函数名称:OsplcPressure
** 函数功能：大屏液晶压力显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcPressure(uint32_t npNum)  
{
    uint32_t vpNum, DisphhWarr[10] = {/*0x0807, 0xC01F*/0x3FE1,0xFFE7,0xBFE8,0xBFE2,0x7FE6,0x3FF2,0x3FF0,0xBFE7,0x3FE0,0x3FE2};
    uint32_t DisphWarr[10] = {0xE4FC,0xFFFC,0xE2FE,0xEAFC,0xF9FC,0xE8FD,0xE0FD,0xFEFC,0xE0FC,0xE8FC};
    uint32_t DisplWarr[10] = {0x9F87,0xFF9F,0xBFA3,0xBF8B,0xDF9B,0x9FCB,0x9FC3,0xBF9F,0x9F83,0x9F8B};
    if(npNum >999) 
    {
        return;
    }
    vpNum = npNum/10;
    if(vpNum >= 10)
    {
        DispMem[8] |= 0xC0;
        DispMem[7] |= 0x1F;
        DispMem[8] &= DisphhWarr[vpNum/10]/0x100;
        DispMem[7] &= DisphhWarr[vpNum/10]%0x100;
    }
    else
    {
        DispMem[8] |= 0xC0;
        DispMem[7] |= 0x1F;
       
    }
    DispMem[6] |= 0x1F; //0x0605,0x1F03,
    DispMem[5] |= 0x03;
    DispMem[6] &= DisphWarr[vpNum%10]/0x100;
    DispMem[5] &= DisphWarr[vpNum%10]%0x100;

    DispMem[6] |= 0x60;  //0x0605,0x607C,
    DispMem[5] |= 0x7C;
    DispMem[6] &= (DisplWarr[npNum%10]/0x100)&0x7F;
    DispMem[5] &= DisplWarr[npNum%10]%0x100;
}

uint16_t OsplcTemperatureArr[3][12] =
{
    {0x0908, 0x803F,0x7FC2,0xFFCF,0x7FD4,0x7FC5,0xFFC9,0x7FE1,0x7FE0,0x7FCF,0x7FC0,0x7FC1}, //NUM32
    {0x09FF,0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF},  //31
    {0x0B0A,0x607C,0x9F87,0xFF9F,0xBFA3,0xBF8B,0xDF9B,0x9FCB,0x9FC3,0xBF9F,0x9F83,0x9F8B},  //NUM30
};

void OsplcTemperatureFunc(uint8_t id, uint8_t tnum)
{
    uint8_t hB, lB;
    hB = OsplcTemperatureArr[id][0]/0x100;
    lB = OsplcTemperatureArr[id][0]%0x100;
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcTemperatureArr[id][1]%0x100;
        if(tnum <= 9)
        {
            DispMem[lB] &= OsplcTemperatureArr[id][tnum+2]%0x100;
        }
    }
    DispMem[hB] |= OsplcTemperatureArr[id][1]/0x100;
    if(tnum <= 9)
    {
        DispMem[hB] &= OsplcTemperatureArr[id][tnum+2]/0x100;
    }
}

bool OsplcTemperatureEx(int nTnum) //NUM30/1/2/3  温度
{
    uint8_t num, n ,m, len = 0; //小数点  DispMem[10] |= 0x80
    char buf[33];
    memset(buf,0,sizeof(buf));
    OsplcTemperatureFunc(3,10);
    OsplcTemperatureFunc(2,10);
    OsplcTemperatureFunc(1,0);
    OsplcTemperatureFunc(0,0);
    len=sprintf(buf,"%d",nTnum);
    len = (4>len) ? len : 3;
    m = len-1;
    for(n=0; n<len; n++)
    {
        num = buf[n]-0x30;
        OsplcTemperatureFunc(m,num);
        m--;
    }
    return true;
}

/*************************************************************************
** 函数名称:OsplcTemperature
** 函数功能：大屏液晶温度显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcTemperature(float vfTemp)
{
    uint32_t npNum;
    OsplcTEMPNOFF(); 
    if(vfTemp<0) //显示负号。
    {
        OsplcTEMPNON();    
    }
    npNum = (uint32_t)(fabs(vfTemp)*10);
    OsplcTemperatureEx(npNum);
    DispMem[10] |= 0x80;
    if(npNum<1000) //显示小数点。
    {
        DispMem[10] &= (~0x80);
    }
    return true;
}

/*************************************************************************
** 函数名称:OsplcWarnning
** 函数功能：大屏液晶异常停机报警显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcWarnning(uint8_t WNO) //NUM22 NUM23
{
    uint32_t OsplchWarr[10] =  {0x07FE, 0x9FFF, 0x57FC, 0x17FD, 0x8FFD, 0x27FD, 0x27FC, 0x97FF, 0x07FC, 0x07FD};
    uint32_t OsplclWarr[10] =  {0x13FE, 0x7FFE, 0x8BFE, 0x2BFE, 0x67FE, 0x23FF, 0x03FF, 0x7BFE, 0x03FE, 0x23FE};
    if(WNO >99) return;
    if(WNO >= 10)
    {
        DispMem[21] |= 0xF8;
        DispMem[20] |= 0x03;
        DispMem[21] &= OsplchWarr[WNO/10]/0x100;
        DispMem[20] &= OsplchWarr[WNO/10]%0x100;
    }
    else
    {
        DispMem[21] |= 0xF8;
        DispMem[20] |= 0x03;
        DispMem[21] &= OsplchWarr[0]/0x100;
        DispMem[20] &= OsplchWarr[0]%0x100;
    }
    DispMem[20] |= 0xFC;
    DispMem[19] |= 0x01;
    DispMem[20] &= OsplclWarr[WNO%10]/0x100;
    DispMem[19] &= OsplclWarr[WNO%10]%0x100;
}

void OsplcDrvgainFunc(uint8_t id, uint8_t drvgainum)
{
    uint8_t hB, lB;
    uint32_t OsplcDrvgainArr[3][12] =  //??
    {
        {0x1615,0xFA01,0x25FE,0xFDFE,0x15FF,0x55FE,0xCDFE,0x47FE,0x07FE,0xF5FE,0x05FE,0x45FE}, //NUM42
        {0x2316,0xF805,0x27FA,0xFFFA,0x17FB,0x57FA,0xCFFA,0x47FE,0x07FE,0xF7FA,0x07FA,0x47FA},  //NUM41
        {0x2423,0xF007,0x4FF8,0xFFF9,0x2FFA,0xAFF8,0x9FF9,0x8FFC,0x0FFC,0xEFF9,0x0FF8,0x8FF8},  //NUM40
    };
    hB = OsplcDrvgainArr[id][0]/0x100;
    lB = OsplcDrvgainArr[id][0]%0x100;
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcDrvgainArr[id][1]%0x100;
        if(drvgainum <= 9)
        {
            DispMem[lB] &= OsplcDrvgainArr[id][drvgainum+2]%0x100;
        }
            
    }
    DispMem[hB] |= OsplcDrvgainArr[id][1]/0x100;
    if(drvgainum <= 9)
    {
        DispMem[hB] &= OsplcDrvgainArr[id][drvgainum+2]/0x100;
    }       
}

/*************************************************************************
** 函数名称:OsplcDrvgain
** 函数功能：大屏液晶增益显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcDrvgain(int nTnum)
{
    uint8_t num, n ,m, len = 0; //小数点  DispMem[10] |= 0x80
    char buf[33];
    memset(buf,0,sizeof(buf));
    
    OsplcDrvgainFunc(2,10);
    OsplcDrvgainFunc(1,0);
    OsplcDrvgainFunc(0,10);
    len=sprintf(buf,"%d",nTnum);
    len = (4>len) ? len : 3;
    m = len-1;
    for(n=0; n<len; n++)
    {
        num = buf[n]-0x30;
        OsplcDrvgainFunc(m,num);
        m--;
    }
    DispMem[5] &= 0x7F;
    return true;
}

void OsplcFrateBack(uint8_t id, uint8_t ftnum)
{
    uint8_t hB, lB;
    uint32_t OsplcFrateBack[4][12] =  //回气流速。
    {
        {0x2524, 0xE00F,0x9FF0,0xFFF3,0x5FF4,0x5FF1,0x3FF3,0x1FF9,0x1FF8,0xDFF3,0x1FF0,0x1FF1}, //NUM39
        {0x2625, 0xC01F,0x3FE1,0xFFE7,0xBFE8,0xBFE2,0x7FE6,0x3FF2,0x3FF0,0xBFE7,0x3FE0,0x3FE2}, //NUM38
        {0x2726, 0x803F,0x7FC2,0xFFCF,0x7FD1,0x7FC5,0xFFCC,0x7FE4,0x7FE0,0x7FCF,0x7FC0,0x7FC4}, //NUM37
        {0x27FF, 0x7FFF,0x84FF,0x9FFF,0xA2FF,0x8AFF,0x99FF,0xC8FF,0xC0FF,0x9EFF,0x80FF,0x88FF}, //NUM36
    };
    hB = OsplcFrateBack[id][0]/0x100;
    lB = OsplcFrateBack[id][0]%0x100;
    if(lB != 0xFF)
    {
        DispMem[lB] |= OsplcFrateBack[id][1]%0x100;
        if(ftnum <= 9)
        {
            DispMem[lB] &= OsplcFrateBack[id][ftnum+2]%0x100;
        }
      
    }
    DispMem[hB] |= OsplcFrateBack[id][1]/0x100;
    if(ftnum <= 9)
    {
        DispMem[hB] &= OsplcFrateBack[id][ftnum+2]/0x100;
    }      
}

/*************************************************************************
** 函数名称:OsplcFlrateBack
** 函数功能：大屏液晶回气流速显示。
** 入口参数：无
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
bool OsplcFlrateBack(uint32_t nfrNum)
{
    uint8_t sum, n ,m, len = 0;
    char buf[33];
    memset(buf,0,sizeof(buf));
    if(nfrNum < 100)
    {
        OsplcFrateBack(3,10);
        OsplcFrateBack(2,0);
        OsplcFrateBack(1,nfrNum/10);
        OsplcFrateBack(0,nfrNum%10);
    }
    else
    {
        len=sprintf(buf,"%d",nfrNum);
        len = (5>len) ? len : 4;
        if(len <3) return false;
        m = len-1;
        for(n=0; n<len; n++)
        {
            sum = buf[n]-0x30;
            OsplcFrateBack(m,sum);
            m--;
        }
    }
    DispMem[13] &= 0xFD;//P11
    return true;
}

//获取一个uint32_t类型的数字的第一个1出现的位置
uint8_t GetOnePosition(uint32_t num)
{
    uint8_t i=0;
    for(i=1;i<33;i++)
    {
        if((num>>(i-1))&(0x01)==1) return i;
    }
	return 0;
}


/*************************************************************************
** 函数名称:OsplcShowN
** 函数功能：向大屏写入要显示的所有数据。
** 入口参数：Row1--气量；
             Row2--金额；
             Row3--单价。
** 返 回 值：无 
** 创 建 者：杨传坤
** 创建时间：2016-03-29

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcShowN(uint32_t Row1,uint32_t Row2,uint16_t Row3)
{
    uint8_t tmperror;
    tmperror=GetOnePosition(syserr);
    memset(DispMem,0xFF,sizeof(DispMem));
    OsplcTitleOn();         //常亮。
    OsplcNum(Row1);         //气量。
    OsplcSum(Row2);         //金额。
    OsplcPrice(Row3);       //单价。
    OsplcGunNum(sysparas.gunnum);      //抢号，此处根据需要修改值。
    OsplcFlRate((uint32_t)(realtime.cur_flowrate_l*10));      //进气流速，此处根据需要修改值。
    OsplcPressure(realtime.cur_pressure/10);    //压力，此处根据需要修改值。
    OsplcTemperature(realtime.cur_temperature_l); //温度，此处根据需要修改值。
    OsplcWarnning(tmperror);    //停机异常报警，此处根据需要修改值。
    OsplcDrvgain(realtime.cur_gain_l*10);     //增益，此处根据需要修改值。
    OsplcFlrateBack(realtime.cur_flowrate_g*100);  //回气流速，此处根据需要修改值。
    OsplcDispN();           //将所有数据写入DispMem数组之后，此函数发送给液晶显示。
}

/*************************************************************************
** 函数名称:OsplcShow
** 函数功能：向液晶屏写入数据。
** 入口参数：
             Row1：第一行要写入的数据；
             Row2：第二行要写入的数据；
             Row3：第三行要写入的数据。
             bStop：负号标志。
             bOverFlag：第一行，第二行第一个字符显示标志。
** 返 回 值：无 
** 创 建 者：杨朝旭
** 创建时间：2016-11-24 10:36:41

** 修 改 者：
** 修改如期：
** 修改内容：
*************************************************************************/
void OsplcShow(uint32_t Row1,uint32_t Row2,uint16_t Row3,bool bStop, bool bOverFlag)
{
    //以下判断是防止定量或定额加气时，过充导致显示屏数据往回跳ycx20161201
    if(globalvar.fix==1)//定量 
    {
        if(Row1>globalvar.fixnum) return;
    }
    if(globalvar.fix==2)//定额
    {
        if(Row2>globalvar.fixmoney) return;
    }
    if(sysparas.lcdtype==0)
    {
        OsplcShowB(Row1, Row2, Row3, bStop, bOverFlag);
    }
    else if(sysparas.lcdtype==1)
    {
        OsplcShowN(Row1, Row2, Row3);
    }
}

