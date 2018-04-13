#ifndef _TIMER_H
#define _TIMER_H


typedef struct Timer0{
    uint32_t tick_100ms;
    uint32_t tick_start;
    uint32_t tick_current;
    uint8_t  flag_100ms;
    uint8_t  flag_500ms;
    uint8_t  flag_1s; 
}Timer0;


extern Timer0   timer0;
void     timer0Init(void);
void     timer1Init(void);
void     timer1on(void);
void     timer1off(void);
extern   void ReadModbusAndPressure(void);

#endif
