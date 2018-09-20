/**
******************************************************************************
* @filename			vermanage.h
* @author				祝世叶
* @Description	vermanage文件，在此文件内主要是当前系统的编译版本号、系统的
                 硬件信息以及作者和使用的OS！
*/

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VerManage_H_  //头文件
#define _VerManage_H_ 

/*     FirstBuiltDate：2017-10-31 18:06:00      */
/*    工程最新编译时间：2018-09-20 11:40:54   */

#define _BuiltDate __DATE__
#define _BuiltTime __TIME__

const unsigned char BuiltDate[] = _BuiltDate;
const unsigned char BuiltTime[] = _BuiltTime;
const unsigned char MCUName[] = {"STM32F407ZGT6"};
const unsigned char MCUKernel[] = {"Cortex-M4"};
const unsigned char RunFOSC[] = {"168MHz"};
const unsigned char Ver[] = {"1.06.78"};
const unsigned char VerSet[] = {"1"};
const unsigned char ProjectName[] = {"门禁考勤系统-客户端"};
const unsigned char Author[] = {"祝世叶"};
const unsigned char OSName[] = {"rt-thread 3.0.4"};


#endif

