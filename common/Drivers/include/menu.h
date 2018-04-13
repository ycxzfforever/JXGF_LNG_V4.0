#ifndef __MENU_H
#define __MENU_H

typedef void(*fun)(void);  

typedef void(*fun1)(fun);  


void TaskAdd(uint32_t keyvalue,fun funname1,fun funname2);

extern fun         DispUI;          //界面刷新处理
extern fun         DispNextUI;      //用于消息显示后的下一个界面


void DispFirst(void);

// 计量单位，质检模式，恢复出厂设置
void DispUnitQcReset(void);

//输入恢复出厂密码
void DispInputResetPwd(void);

//输入维护密码
void DispInputTestPwd(void);

//维护密码验证
void DispJudgeTestPwd(void);

//输入质检密码
void DispInputQcPwd(void);

//质检密码验证
void DispJudgeQcPwd(void);

//输入菜单密码
void DispInputMenuPwd(void);

//密码输入界面
void DispInputPwd(char* pwdbuf,fun funname);

//初始化输入数据buf
void InputInit(void);

//单价设置
void DispSetPrice(void);

//优惠单价设置1
void DispSetOthPrice1(void);


//菜单界面1显示
void DispMenu_1(void);

//通用信息设置
void DispGeneralInfo(void);
//通讯参数设置
void DispCommPara(void);

//常用操作设置
void DispCommonOpt(void);

//菜单界面2显示
void DispMenu_2(void);

//版本编号设置1
void DispVerNum1(void);

//版本编号设置2
void DispVerNum2(void);

//其他信息设置1
void DispOthMsg1(void);

//其他信息设置2
void DispOthMsg2(void);


//菜单界面3显示
void DispMenu_3(void);

//系统参数设置
void DispSysSet1(void);

//系统参数设置2
void DispSysSet2(void);

//班组信息设置1
void DispSetShiftInfo1(void);

//班组信息设置2
void DispSetShiftInfo2(void);

//班组信息设置3
void DispSetShiftInfo3(void);

void DispSetPressure(void);


//IC卡信息设置1
void DispSetICInfo1(void);

//IC卡信息设置2
void DispSetICInfo2(void);

//消息显示，ms_100:为显示时间，100ms为单位，funname:时间到了过后需要执行的函数
void TaskMessage(uint32_t ms_100,fun funname);


//确认处理
void DispOK(void);

//设置参数时显示 numpoint:小数点后面位数,0,2,4有效，当大于4时为选择输入
void DispSet(uint8_t numpoint,uint32_t data,char*buf);

//旧密码输入
void DispInputOldPwd(void);

//菜单密码设置
void DispSetMenuPwd(void);
//质检密码设置
void DispSetQcPwd(void);

//维护密码设置
void DispSetTestPwd(void);
//上下班密码设置
void DispSetonoffworkpwd(void);

//恢复出厂密码设置
void DispSetResetPwd(void);

//恢复出厂设置密码验证
void DispJudgeResetPwd(void);

//恢复出厂设置
void DispFactoryReset(void);

//质检参数设置
void DispQcPara(void);

void SimulatorSet(void);

//显示界面处理
extern fun DispUI;

//上班。
void OnWork(void);
//下班。
void OffWork(void);

void Diswork(void);

void WriteICcard(void);

uint16_t memcmpp(uint8_t *tr1,uint8_t *tr2);
//设置成功显示
void SetSuccess(void);
//设置失败显示
void SetFault(void);

//密钥信息版本号设置
void DispSetLockVer(void);

//常用操作设置1。
void DispCommonOpt1(void);

//屏幕选择函数。
void DispSetPing(void);
//密码上下班
void OnWorkWithPwd(void);
void OffWorkWithPwd(void);
void DispJudgeOnWorkPwd(void);
void DispJudgeOffWorkPwd(void);
//脱机模式
void DispSetOffline(void);

void SystemReStart(void);
//Flash存储芯片擦除
void DispEraseChip(void);

#endif

