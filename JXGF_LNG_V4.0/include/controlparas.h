#ifndef __CONTROLPARAS_H
#define __CONTROLPARAS_H

//常用操作显示CNG和LNG需要分开编写此内容
void DispSetControl1(void);
void DispSetControl2(void);
void DispSetControl3(void);
void DispSetControl4(void);


//控制参数1查询
void DispCtrlParas1(void);

//控制参数2查询
void DispCtrlParas2(void);

//控制参数3查询
void DispCtrlParas3(void);
//控制参数4查询
void DispCtrlParas4(void);



//恢复出厂设置时，各个参数重置
void FactoryReset(void);


#endif

