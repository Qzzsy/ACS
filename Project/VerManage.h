/**
******************************************************************************
* @filename			vermanage.h
* @author				ף��Ҷ
* @Description	vermanage�ļ����ڴ��ļ�����Ҫ�ǵ�ǰϵͳ�ı���汾�š�ϵͳ��
                 Ӳ����Ϣ�Լ����ߺ�ʹ�õ�OS��
*/

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _VerManage_H_  //ͷ�ļ�
#define _VerManage_H_ 

/*     FirstBuiltDate��2017-10-31 18:06:00      */
/*    �������±���ʱ�䣺2018-09-20 11:40:54   */

#define _BuiltDate __DATE__
#define _BuiltTime __TIME__

const unsigned char BuiltDate[] = _BuiltDate;
const unsigned char BuiltTime[] = _BuiltTime;
const unsigned char MCUName[] = {"STM32F407ZGT6"};
const unsigned char MCUKernel[] = {"Cortex-M4"};
const unsigned char RunFOSC[] = {"168MHz"};
const unsigned char Ver[] = {"1.06.78"};
const unsigned char VerSet[] = {"1"};
const unsigned char ProjectName[] = {"�Ž�����ϵͳ-�ͻ���"};
const unsigned char Author[] = {"ף��Ҷ"};
const unsigned char OSName[] = {"rt-thread 3.0.4"};


#endif

