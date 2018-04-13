#include "kernel.h"

/********************************************************
**函数名称:CRCInit
**函数功能:CRC初始化
**输入参数:CRC_mode:CRC的相关参数
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-21

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void CRCInit( CRC_CFG_Type* CRC_mode )
{
    LPC_CRC->MODE = 0;
    LPC_CRC->SEED = 0;

    LPC_CRC->MODE = ((CRC_mode->poly)|(CRC_mode->bit_wr)|( CRC_mode->cmpl_wr)|\
                    (CRC_mode->bit_sum)|(CRC_mode->cmpl_sum));
    LPC_CRC->SEED = CRC_mode->seed;
    return;
}

/********************************************************
**函数名称:CRCWriteData
**函数功能:向CRC的寄存器写入要计算的数据
**输入参数:data:计算CRC的数据
					 length:计算CRC的数据长度
**输出值  :无

**创建者  :黄豪民
**创建日期:2016-04-21

**修改者  :
**修改日期:
**修改内容:
********************************************************/
void CRCWriteData(uint8_t *data, uint8_t length)
{
  uint16_t * data_word = (uint16_t *)data;
  uint32_t * data_dword = (uint32_t *)data;

  switch ( length )
  {
     case 4:                                                            /* 数据宽度为32位               */
       LPC_CRC->WR_DATA_DWORD = *data_dword;
     break;
     case 2:                                                            /* 数据宽度为16位               */
       LPC_CRC->WR_DATA_WORD = *data_word;
     break;
     case 1:                                                            /* 数据宽度为8位                */
       LPC_CRC->WR_DATA_BYTE = *data;
     break;
     default:
     break;
  }
  return;
}

/********************************************************
**函数名称:CRCGetResult
**函数功能:获取CRC计算结果
**输入参数:无
**输出值  :CRC计算结果

**创建者  :黄豪民
**创建日期:2016-04-21

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint32_t CRCGetResult(void)
{
  uint32_t regVal;
  regVal = LPC_CRC->SUM;                                                /* 读取最近的校验和数值         */
  return regVal;
}

/********************************************************
**函数名称:CRC16Calculate
**函数功能:CRC计算
**输入参数:data：计算CRC的数据
           len ：计算CRC数据的长度
**输出值  :CRC计算结果

**创建者  :黄豪民
**创建日期:2016-04-21

**修改者  :
**修改日期:
**修改内容:
********************************************************/
uint16_t CRC16Calculate(uint8_t *data, uint32_t len)
{
    CRC_CFG_Type crc_mode;
    uint16_t usCRCResult;                                               /* 校验和结果变量               */
    uint16_t i;

    crc_mode.poly     = CRC_POLY_16;                                    /* 选择CRC-16多项式             */
    crc_mode.bit_wr   = CRC_BIT_RVS_WR_EN;                              /* 输入数据位序反转             */
    crc_mode.cmpl_wr  = CRC_CMPL_WR_NO;
    crc_mode.bit_sum  = CRC_BIT_RVS_SUM_EN;                             /* 校验和位序反转               */
    crc_mode.cmpl_sum = CRC_CMPL_SUM_NO;
    crc_mode.seed     = 0xffff;                                         /* 设置初值为 0                 */
    CRCInit(&crc_mode);                                                /* 初始化CRC引擎                */

    for(i = 0; i < len; i++) {
        CRCWriteData(data, 1);                                       /* 写入待校验数据               */
        data++;
    }

    usCRCResult=CRCGetResult();                                         /* 读取校验和                   */
    return(usCRCResult);                                                /* 返回校验和                   */
}

