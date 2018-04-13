#ifndef __PRINTER_H
#define __PRINTER_H

void    WHPrinterInit(void);
void    WHPrinterNormalCmd(void);
void    WHPrinterFreeRow(unsigned char rows);
void    WHPrinterHead(void);
uint8_t WHPrintReady(void);
void    WHPrintf(char *fmt,...);
void    WHFuelRecord(void);
void    uart0PutString(char* string);
uint8_t uart0GetByte (void);
//´òÓ¡Á÷Ë®¡£
void Printid(void);
void WHFuelShifRecord(void);
void PrintShifid(void);

void PrintSum(void);
void WHFuelSumRecord(void);
void WHFuelSysInfo(void);
void PrintSysInfo(void);


extern  uint8_t stopreasonbuf[][16];

#endif
