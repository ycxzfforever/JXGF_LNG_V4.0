#ifndef __KEY_H__
#define __KEY_H__

#define KEY_0       0x000FEFFF
#define KEY_1       0x000FFFFE
#define KEY_2       0x000FFFFD
#define KEY_3       0x000FFFFB
#define KEY_4       0x000FFFEF
#define KEY_5       0x000FFFDF
#define KEY_6       0x000FFFBF
#define KEY_7       0x000EFFFF
#define KEY_8       0x000DFFFF
#define KEY_9       0x000BFFFF

#define KEY_UP      KEY_6
#define KEY_DOWN    KEY_9
    
#define KEY_QRY     0x000FFFF7  //查询
#define KEY_SET     0x000FFF7F  //菜单
#define KEY_FIX     0x0007FFFF  //定量
#define KEY_BACK    0x000FBFFF  //回显(CNG)、预冷 (LNG)
#define KEY_CLR     0x000F7FFF  //清除 
#define KEY_START   0x000FFEFF  //加气
#define KEY_STOP    0x000FFDFF  //停止 
#define KEY_OK      0x000FFBFF  //确认 
#define KEY_RET     0x000FF7FF  //返回
#define KEY_PT      0x000FDFFF  //小数点   
    
#define KEY_COMB_SSET   0x000FFD7F /*STOP+SET设置*/
#define KEY_COMB_SQRY   0x000FFDF7 /*STOP+QRY查询*/
#define KEY_COMB_SFIX   0x0007FDFF /*STOP+FIX定量*/
#define KEY_COMB_SBACK  0x000FBDFF /*STOP+BACK回显*/
#define KEY_COMB_SCLR   0x000F7DFF /*STOP+CLR清除*/
#define KEY_COMB_SRET   0x000FF5FF /*STOP+RET返回*/ 
#define KEY_COMB_7KEY   0x000EFDFF /*STOP+7*/ 
#define KEY_COMB_8KEY   0x000DFDFF /*STOP+8*/ 

typedef struct def_Screeninput//主显输入
{
   uint8_t  inputlen;       //实时输入变化长度，长度从0开始计算
   uint8_t  inputmaxlen;    //最大输入长度，长度从0开始计算
   uint8_t  pointnum;       //小数点个数
   uint8_t  array[16];      //存储数据内容数组   
}Screeninput;


extern  Screeninput ScreenInput;
void KeyPinInit(void);
void KeyGetValue(void);
void NumKeyHandler(void);


#endif
