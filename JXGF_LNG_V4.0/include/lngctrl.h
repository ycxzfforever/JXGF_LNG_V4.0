#ifndef __LNGCTRL_H__
#define __LNGCTRL_H__


void    PumpSwitch(unsigned char state);
void    StartFueling(void);
void    StopFueling(void);
void    StartPrecooling(void);
void    DispStartFueling(void);
void 	FuelEndDisplay(void);
void    GetCardData(void);
void    DispCardInfo(void);
void    GreyLock(void);
void    YesOrNoCard(void);
uint8_t YesOrNoCard1(void);
void    ApplyVerif(void);
void    VerificationCard(void);
void    ApplyHeiBai(void);
void    GetBlack(void);
void    GetWhite(void);
void 	SaveFm25l04(void);
void 	PowerDown(void);
void 	PowerDownRealDate(void);
void 	FixGas(void);
void 	FixNum(void);
void 	FixMoney(void);
void 	DisPlayLock(void);
uint8_t SearchBlackList(uint8_t type,uint8_t *cardNo);
void 	BcdToStr(uint8_t bcd[],uint8_t str[]);
void 	ComparePasswd(void);
void 	InputIccardPasswd(void);
void 	IcCardPasswd(void);
void 	DisplayGP(void);
void 	RemoveStringChar(uint8_t *buf,uint8_t ch,uint16_t len);
void    DispScanRFID(void);


extern  double          lngvolume;//½øÒºÁ¿
extern  RealTimeData    realtime;

#endif

