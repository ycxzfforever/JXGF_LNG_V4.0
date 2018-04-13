#ifndef  __RTC_H
#define  __RTC_H

#define ILR_RTCCIF    0x01
#define ILR_RTCALF    0x02

#define CCR_CLKEN     0x01
#define CCR_CTCRST    0x02
#define CCR_CCALEN    0x10

typedef struct {
  uint32_t RTC_Sec;                        /* 秒 - [0,59]           */
	uint32_t RTC_Min;                        /* 分钟- [0,59]           */
	uint32_t RTC_Hour;                       /* 小时 - [0,23]             */
	uint32_t RTC_Mday;                       /* 本月的第几天 - [1,31] */
	uint32_t RTC_Mon;                        /* 月 - [1,12]            */
	uint32_t RTC_Year;                       /* 年 - [0,4095]           */
	uint32_t RTC_Wday;                       /* 周的第几天(可以不管) - [0,6]       */
	uint32_t RTC_Yday;                       /* 年的第几天(可以不管) - [1,365]     */
}RTCTime;

void RTCSetTime(RTCTime Time);
RTCTime RTCGetTime(void);
void RTCInit(void);
void RTCStart(void);
void RTCStop(void);
#endif
