#ifndef __DS3232_H
#define __DS3232_H

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}Time;
extern  Time    time;
uint8_t Ds3232ReadTime(void);
uint8_t Ds3232SetTime(uint8_t *buffer);
#endif

