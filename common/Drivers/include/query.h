#ifndef __QUERY_H
#define __QUERY_H

//总累查询显示
void DispSum(void);

//班累数据显示后面根据本记录数据读取查询
void DispShiftSum(void);

//流水记录查询显示
void DispRecord(void);

//系统信息显示1
void DispSysParas1(void);

//系统信息显示2
void DispSysParas2(void);

//系统信息显示3
void DispSysParas3(void);

//系统信息显示4
void DispSysParas4(void);

//系统信息显示5
void DispSysParas5(void);

//系统信息显示6
void DispSysParas6(void);

//系统信息显示7
void DispSysParas7(void);

//系统信息显示8
void DispSysParas8(void);


//查询菜单显示1
void DispQuery1(void);

//查询菜单显示2
void DispQuery2(void);

//请输入班流水号
void DispInputShifTransNum(void);

//班流水记录查询显示
void DispshifRecord(void);

//班流水号下键查询
void RecordShifUp(void);

//班流水号上键查询
void RecordShifDown(void);

void DispHeiBai(void);

void DispHei(void);

void DispHeiAdd(void);

void DispHeiDel(void);

void DispBai(void);

//请输入修改流水号
void DispInputXGNum(void);
//修改流水记录查询显示
void DispXGRecord(void);
//修改流水号下键查询
void RecordXGUp(void);
//修改流水号上键查询
void RecordXGDown(void);

void Printerid(void);

void Printershifid(void);

void DebugHeiBai(uint32_t addr, uint32_t num,uint8_t *ptr);


//系统信息显示9
void DispSysParas9(void);
#endif
