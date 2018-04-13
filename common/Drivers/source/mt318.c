#include "kernel.h"

Mt318Reader mt318reader;
//M1Card m1card;    //¶¨ÒåM1¿¨¿é½á¹¹Ìå¡£//huanghao20161026

/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318CrcVerify
**º¯Êý¹¦ÄÜ£ºMT318¶Á¿¨Æ÷Ð£ÑéËã·¨
**Èë¿Ú²ÎÊý£ºarray:ÐèÒªÐ£ÑéµÄÊý¾Ý
                                    length:Ð£ÑéÊý¾ÝµÄ³¤¶È
**·µ»ØÖµ£º¼ÆËãºóµÄBCCÐ£ÑéÖµ
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-3-31 16:27:23
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318CrcVerify(uint8_t *array,uint8_t length)
{
    uint8_t crcresult=0;
    uint8_t tmp;
    crcresult=*array;
    for(tmp=1; tmp<length; tmp++)
        crcresult^=*(array+tmp);
    return(crcresult);
}

/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318GetCardStatus
**º¯Êý¹¦ÄÜ£ºÍ¨¹ýMT318»ñÈ¡¿¨µÄ×´Ì¬
**Èë¿Ú²ÎÊý£ºÎÞ
**·µ»ØÖµ£ºTRUE:³É¹¦£¬FALSE:Ê§°Ü
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:35:24
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318GetCardStatus(void)
{
    //0x02 0x00 0x02 0x31 0x41 0x03 BCC
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x31,0x41,0x03};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = FALSE;

    memset((uint8_t*)mt318reader.recbuf,0,sizeof(mt318reader.recbuf));
    str[6]=Mt318CrcVerify(str,6);
    mt318reader.err_code = 0;

    SELECT_CARD();
    OpenUartRcvIRQ(CardPort);
    UARTSend(CardPort, str, 7);

    timeout =  CARD_RESPONSE_MAX_TIME;
    while(--timeout)
    {
        if(mt318reader.recbuf[5] == 0x31)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"»úÄÚÎÞ¿¨");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"Ñ°²»µ½ÉäÆµ¿¨");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x32)
        {
            ret = TRUE;
            //Fyd12864DispPrintf(2,1,"CPU B");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x33)
        {
            ret = TRUE;
            //Fyd12864DispPrintf(2,1,"CPU A");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x34)
        {
            ret = TRUE;
            //Fyd12864DispPrintf(2,1,"M1¿¨");
            break;
        }
        mt318reader.err_code = mt318reader.recbuf[6];
    }
    if(timeout == 0) ret = FALSE;
    return  ret;
}

/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318GetCardSerialNum
**º¯Êý¹¦ÄÜ£ºÍ¨¹ýMT318»ñÈ¡¿¨µÄÐòÁÐºÅ
**Èë¿Ú²ÎÊý£ºÎÞ
**·µ»ØÖµ£ºTRUE:³É¹¦£¬FALSE:Ê§°Ü
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:36:07
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318GetCardSerialNum(void)  //Ñ°¿¨»ñÈ¡¿¨ÐòÁÐºÅ
{
    uint8_t     str[7] = {0x02, 0x00, 0x02, 0x34,0x31,0x03};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = FALSE;

    memset((uint8_t*)mt318reader.recbuf,0,sizeof(mt318reader.recbuf));
    str[6]=Mt318CrcVerify(str,6);
    mt318reader.err_code = 0;
    OpenUartRcvIRQ(CardPort);
    SELECT_CARD();
    UARTSend(CardPort, str, 7);
    while(--timeout)
    {
        if(mt318reader.recbuf[5] == 0x59 &&mt318reader.recbuf[10] == 0x03)
        {
            ret = TRUE;
            //Fyd12864DispPrintf(2,1,"¿¨ÐòÁÐºÅ");
           // Fyd12864DispPrintf(3,1,"%02X%02X%02X%02X",mt318reader.recbuf[6],mt318reader.recbuf[7],
                               //mt318reader.recbuf[8],mt318reader.recbuf[9]);
            break;
        }
        else if(mt318reader.recbuf[5] == 0x4e)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"Ñ°¿¨²»³É¹¦");
            break;
        }
        else if(mt318reader.recbuf[5] == 0x45)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"»úÄÚÎÞ¿¨");
            break;
        }
        mt318reader.err_code = mt318reader.recbuf[5];
    }
    if(timeout == 0) ret = FALSE;
    return  ret;
}

/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318VerifyKeyB
**º¯Êý¹¦ÄÜ£ºÑéÖ¤¿¨µÄBÃÜ
**Èë¿Ú²ÎÊý£ºkey:ÃÜÂë
                                secter_num:ÉÈÇøºÅ
**·µ»ØÖµ£ºTRUE:³É¹¦£¬FALSE:Ê§°Ü
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:37:19
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318VerifyKey(uint8_t keyAB,uint8_t * key,uint8_t secter_num)
{
    uint8_t     i,str[14] = {0x02, 0x00, 0x09, 0x34,0x39,0xFF,'2','2','2','2','2','2'};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = FALSE;

    memset((uint8_t*)mt318reader.recbuf,0,sizeof(mt318reader.recbuf));
    str[4] = keyAB;
    str[5] = secter_num;
    for(i=0; i<6; i++)
    {
        str[6+i]=key[i];
    }
    str[12]= 0x03;
    str[13]=Mt318CrcVerify(str,13);

    mt318reader.err_code = 0;
    SELECT_CARD();
    OpenUartRcvIRQ(CardPort);
    UARTSend(CardPort, str, 14);
    while(--timeout)
    {
        if(mt318reader.recbuf[6] == 0x59)
        {
            ret = TRUE;
            //Fyd12864DispPrintf(2,1,"ÃÜÂëÑéÖ¤³É¹¦");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x30)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"Ñ°²»µ½ÉäÆµ¿¨");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x33)
        {
            ret = FALSE;
            if(mt318reader.recbuf[4]==KEYA);
                //Fyd12864DispPrintf(3,1,"A ÃÜÑéÖ¤Ê§°Ü");
            else if(mt318reader.recbuf[4]==KEYB);
                //Fyd12864DispPrintf(3,1,"B ÃÜÑéÖ¤Ê§°Ü");
            break;
        }
        else if(mt318reader.recbuf[6] == 0x45)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(2,1,"¿¨»úÄÚÎÞ¿¨");
            break;
        }
        mt318reader.err_code = mt318reader.recbuf[6];
    }
    if(timeout == 0) ret = FALSE;
    return  ret;
}

/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318ReadM1Card
**º¯Êý¹¦ÄÜ£º¶ÁÈ¡M1¿¨ÖÐµÄÄÚÈÝ
**Èë¿Ú²ÎÊý£ºkeyAB:ÑéÖ¤ÃÜÂëÊ±²ÉÓÃAÃÜÑéÖ¤»¹ÊÇBÃÜÑéÖ¤
                                key:ÃÜÂë
                                blocks:¿éºÅ
                                rdbuf:½«¶ÁÈ¡µÄÊý¾ÝÄÚÈÝ×°Èë¸Ãbuf
**·µ»ØÖµ£ºTRUE:³É¹¦£¬FALSE:Ê§°Ü
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:41:23
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318ReadM1Card(uint8_t keyAB,uint8_t * key,uint8_t blocks, uint8_t* rdbuf)
{
    uint8_t     secter = 0;
    uint8_t     str[10] = {0x02, 0x00, 0x04, 0x34,0x33};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = FALSE;
    WATCH_DOG();
    secter = blocks/4;
    if(Mt318VerifyKey(keyAB,key,secter) == FALSE)
    {
        return FALSE;
    }

    str[5] = secter;
    str[6] = blocks-4*secter;
    str[7] = 0x03;
    str[8]=Mt318CrcVerify(str,8);

    memset((uint8_t*)mt318reader.recbuf,0,sizeof(mt318reader.recbuf));
    mt318reader.err_code = 0;
    SELECT_CARD();
    OpenUartRcvIRQ(CardPort);
    UARTSend(CardPort, str, 9);
    while(--timeout)
    {
        if((mt318reader.recbuf[7] == 0x59)&&(mt318reader.recbuf[24] == 0x03))
        {
            ret = TRUE;
            memcpy(rdbuf, (uint8_t*)&mt318reader.recbuf[8], 16);
            //Fyd12864DispPrintf(4,1,"¶Á¿é³É¹¦:%d,%x",blocks,mt318reader.recbuf[11]);
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"Ñ°²»µ½RF¿¨");
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"ÃÜÂëÑéÖ¤´íÎó%x,%x",mt318reader.recbuf[5],mt318reader.recbuf[6]);
            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"¶ÁÊý¾Ý´í");
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = FALSE;
           // Fyd12864DispPrintf(4,1,"¿¨»úÄÚÎÞ¿¨");
            break;
        }
        mt318reader.err_code = mt318reader.recbuf[7];
    }
    if(timeout == 0) ret = FALSE;
    return  ret;
}


/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318WriteM1Card
**º¯Êý¹¦ÄÜ£ºÏòM1¿¨ÖÐÐ´ÈëÊý¾Ý
**Èë¿Ú²ÎÊý:   keyAB:ÑéÖ¤ÃÜÂëÊ±²ÉÓÃAÃÜÑéÖ¤»¹ÊÇBÃÜÑéÖ¤º
                                key:ÃÜÂë
                                blocks:¿éºÅ
                                wtbuf:½«wtbufµÄÊý¾ÝÐ´Èëm1¿¨ÖÐ
**·µ»ØÖµ£ºTRUE:³É¹¦£¬FALSE:Ê§°Ü
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:43:34
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
uint8_t Mt318WriteM1Card(uint8_t keyAB,uint8_t * key,uint8_t blocks, uint8_t *wtbuf)
{
    uint8_t     secter = 0;
    uint8_t     str[26] = {0x02, 0x00, 0x14, 0x34,0x34};
    uint32_t    timeout =  CARD_RESPONSE_MAX_TIME;
    uint8_t     ret = FALSE;
    WATCH_DOG();
    secter = blocks/4;
    if(Mt318VerifyKey(keyAB,key,secter) == FALSE)
    {
        return FALSE;
    }
    str[5] = secter;
    str[6] = blocks-4*secter;
    memcpy((uint8_t *)&str[7], wtbuf, 16);
    str[23] =  0x03;
    str[24]=Mt318CrcVerify(str,24);

    memset((uint8_t*)mt318reader.recbuf,0,sizeof(mt318reader.recbuf));
    mt318reader.err_code = 0;
    SELECT_CARD();
    OpenUartRcvIRQ(CardPort);
    UARTSend(CardPort, str, 25);

    while(--timeout)
    {
        if(mt318reader.recbuf[7] == 0x59)
        {
            ret = TRUE;
            Fyd12864DispPrintf(4,1,"Ð´¿é³É¹¦:%d",blocks);
            break;
        }
        else if(mt318reader.recbuf[7] == 0x30)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"Ñ°²»µ½RF¿¨");
            break;
        }
        else if(mt318reader.recbuf[7] == 0x31)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"²Ù×÷ÉÈÇøºÅ´í");
            break;
        }
        else if(mt318reader.recbuf[7] == 0x33)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"ÃÜÂëÑéÖ¤´í%x,%x",mt318reader.recbuf[5],mt318reader.recbuf[6]);
            break;
        }
        else if(mt318reader.recbuf[7] == 0x34)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"Ð£ÑéÐ´Èë¿éÊý¾Ý´í");
            break;
        }
        else if(mt318reader.recbuf[7] == 0x45)
        {
            ret = FALSE;
            //Fyd12864DispPrintf(4,1,"¿¨»úÄÚÎÞ¿¨");
            break;
        }
        mt318reader.err_code = mt318reader.recbuf[7];
    }
    if(timeout == 0) ret = FALSE;
    return  ret;
}


/***************************************************************************
**º¯ÊýÃû³Æ£º Mt318RecDataCheck
**º¯Êý¹¦ÄÜ£ºÖÐ¶Ï´¦Àíº¯ÊýÖÐ¼ì²â½ÓÊÕµ½¶Á¿¨Æ÷µÄÊý¾ÝÊÇ·ñÕýÈ·
**Èë¿Ú²ÎÊý:   ÎÞ
**·µ»ØÖµ£ºÎÞ
**´´½¨Õß£ºÑî³¯Ðñ
**´´½¨ÈÕÆÚ£º2016-4-7 14:46:34
**
**ÐÞ¸ÄÕß£º
**ÐÞ¸ÄÈÕÆÚ£º
**ÐÞ¸ÄÄÚÈÝ£º
***************************************************************************/
void Mt318RecDataCheck(void)
{
    uint32_t    len,i;
    uint32_t    datalen;//½ÓÊÕmt318Êý¾ÝÖ¡ÖÐµÄÊý¾Ý³¤¶È£¬³ý¿ªÖ¡Í·¡¢Ö¡Î²¡¢BCC
    uint8_t     crc;
    if(UART0Buffer[0]==0x02)//Ö¡Í·
    {
        if((UART0Count>2)&&((UART0Buffer[1]!=0)||(UART0Buffer[2]!=0)))
        {
            datalen=(uint32_t)UART0Buffer[1]<<8|UART0Buffer[2];
            len=5+datalen;

        }
        else if((UART0Buffer[1]==0)&&(UART0Buffer[2]==0))
        {//¿ÉÄÜÊÇMT318bug£¬µ±»ñÈ¡ÐòÁÐºÅÊ±£¬Èô»úÄÚÎÞ¿¨£¬Ôò·µ»ØµÄ³¤¶ÈÎª0            
            len=12;
        }
        if((UART0Count>=len)&&(UART0Count>2))
        {
            crc=Mt318CrcVerify((uint8_t*)UART0Buffer,len-1);
            if(crc==UART0Buffer[len-1])
            {
                CloseUartRcvIRQ(CardPort);
                for(i=0; i<len; i++)
                {
                    mt318reader.recbuf[i]=UART0Buffer[i] ;
                }
                mt318reader.flag = 1;
            }
            UART0Count=0;
        }
    }
    else if(UART0Buffer[0]==0x06)
    {
        UartSendByte(CardPort,0x05);
        UART0Count=0;
    }
    else
    {
        OpenUartRcvIRQ(CardPort);
        UART0Count=0;
    }
}



/*********************************************************************************************************
** º¯ÊýÃû³Æ£ºUART0_IRQHandler
** º¯ÊýÃèÊö£ºUART0ÖÐ¶Ï´¦Àíº¯Êý
** ÊäÈë²ÎÊý£ºÎÞ
** ·µ»ØÖµ  £ºÎÞ
*********************************************************************************************************/
void UART0_IRQHandler(void)
{
    uint8_t uiIIRValue = 0, uiLSRValue = 0;
    uint8_t uiDummy = uiDummy;

    uiIIRValue = LPC_UART0->IIR;

    uiIIRValue >>= 1;                     /* ÖÐ¶Ï±êÖ¾¼Ä´æÆ÷ÖµÓÒÒÆ1Î»*/

    uiIIRValue &= 0x07;
    if(uiIIRValue == IIR_RLS)             /* ½ÓÊÕÏß×´Ì¬ÖÐ¶Ï */
    {
        uiLSRValue = LPC_UART0->LSR;      /* ¶ÁÈ¡½ÓÊÕÏß×´Ì¬¼Ä´æÆ÷*/
        /*
                         * Òç³öÖÐ¶Ï¡¢ÆæÅ¼ÖÐ¶Ï¡¢Ö¡ÖÐ¶Ï¡¢UART RBRÖÐ°üº¬´íÎóÊý¾ÝºÍÖ¡¼ä¸ôÖÐ¶Ï´¦Àí
                         */
        if(uiLSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI))
        {
            UART0Status = uiLSRValue;
            uiDummy = LPC_UART0->RBR;
            return;
        }
        if(uiLSRValue & LSR_RDR)          /* ½ÓÊÕFIFO²»Îª¿ÕÖÐ¶Ï*/
        {
            UART0Buffer[UART0Count] = LPC_UART0->RBR;
            UART0Count++;
            if(UART0Count == BUFSIZE)
            {
                UART0Count = 0;
            }
            Mt318RecDataCheck();
        }
    }
    else if(uiIIRValue == IIR_RDA)        /* ½ÓÊÕÊý¾Ý¿ÉÓÃÖÐ¶Ï */
    {
        UART0Buffer[UART0Count] = LPC_UART0->RBR;
        UART0Count++;
        if(UART0Count == BUFSIZE)
        {
            UART0Count = 0;
        }
        Mt318RecDataCheck();
    }
    else if(uiIIRValue == IIR_CTI)                                        /* ×Ö·û³¬Ê±ÖÐ¶Ï               */
    {
        UART0Status |= 0x100;
    }
    else if(uiIIRValue == IIR_THRE)                                       /* ·¢ËÍ±£³Ö¼Ä´æÆ÷¿ÕÖÐ¶Ï       */
    {
        uiLSRValue = LPC_UART0->LSR;
        if(uiLSRValue & LSR_THRE)
        {
            UART0TxEmpty = 1;
        }
        else
        {
            UART0TxEmpty = 0;
        }
    }
}

