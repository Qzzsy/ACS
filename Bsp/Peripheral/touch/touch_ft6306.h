/**
 ******************************************************************************
 * @Copyright     (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename      touch_ft6306.h
 * @author        �Ž�����С��
 * @version       V1.0.3
 * @date          2017-01-15
 * @Description   touch_ft6306�ļ����ڴ��ļ��ڶ�����ft6306���õ����ft6306��Ӧ��
                  ���Խṹ���Լ����⿪�ŵ�API�ӿ�
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-02     ZSY       V1.0.0        first version.
 * 2017-01-09     ZSY       V1.0.1        �Ű��ʽ���������޸��ļ��ͷ�������.
 * 2018-01-11     ZSY       V1.0.2        ����5�㴥����֧�֣��������ƴ����������
 * 2018-01-15     ZSY       V1.0.3        �Ż�2�㴥����֧��
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __TOUCH_FT6306_H__
#define  __TOUCH_FT6306_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Public macro Definition ---------------------------------------------------*/

//#define TOUCH_GPIO_PortSource		 	GPIO_PortSourceGPIOF
//#define TOUCH_GPIO_PinSource		 	GPIO_PinSource12
//#define TOUCH_EXTI_Line				EXTI_Line12
//#define TOUCH_EXTI_IRQn				EXTI15_10_IRQn
	
/*	��غ궨��		*/
#define TOUCH_NO_POINT              (~0)
      
/*	�������		*/      
#define CT_COM_OK                   (0)
#define CT_ACK_FAIL                 (1)

#define ERROR_UNKNOW                (0)
#define ERROR_OK                    (1)
	
#define ERROR_TOUCH_POINT_INVALID   (0)
#define ERROR_TOUCH_POINT_VALID     (1)

#define FT6X06_ID                   (0x6306)
#define CT_IC                       (FT6X06_ID)	//ʹ�õĿ���IC
#define CT_ADDR                     (0x70)      //������ַ
#define CT_WRITE_MASK               (0x00)		
#define CT_READ_MASK                (0x01)		

/*********************��ؽṹ��*************************/
#if (CT_IC == 0X6336)
#define MAX_TOUCH_POINT             (5)         //����������
#elif (CT_IC == 0X6306)
#define MAX_TOUCH_POINT             (2)         //����������
#endif
#ifndef MAX_TOUCH_POINT
#define MAX_TOUCH_POINT             (1)
#endif

/* �������¼� */
#define EVENT_PRESS_DOWN            (0) 
#define EVENT_LIFT_UP               (1)
#define EVENT_CONTACT               (2)
#define EVENT_NONE                  (3)
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
/* ����һЩ��صĽṹ�� */
typedef struct
{
	uint8_t event_flag;             //�������¼�
	uint16_t x_pos;
	uint16_t y_pos;
	uint16_t pre_x_pos;             //��һ�εĵ�λ��
	uint16_t pre_y_pos;
}ct_point_typedef;

typedef struct
{
	uint8_t n_point;                //��ǰ��Ч�����������,����FT6X06�������
	uint8_t gesture;                //����
	uint8_t flag_ct_int;            //�����жϱ�־
	uint8_t flag_point_to_deal;     //�Ƿ��е���Ч��־			
	ct_point_typedef *st_ct_point[MAX_TOUCH_POINT]; 
}ct_touch_status_typedef;

typedef struct
{
	uint16_t valid_start_x_pos;     //X��Ч��ʼλ��
	uint16_t valid_end_x_pos;       //X��Ч����λ��
	uint16_t valid_start_y_pos;     //Y��Ч��ʼλ��
	uint16_t valid_end_y_pos;       //Y��Ч����λ��
	uint8_t valid_err_range;        //����ƫ��
}ct_valid_point_typedef;

/* Member method APIs --------------------------------------------------------*/
extern ct_touch_status_typedef ct_touch_status;	

//uint8_t FT6206_Read_Reg(uint8_t *pbuf,uint32_t len);

//int GUI_TOUCH_X_MeasureX(void); 
//int GUI_TOUCH_X_MeasureY(void);

//void TP_GetAdXY(unsigned int *x,unsigned int *y);
void touch_reset(void);
void touch_init(void);
uint8_t touch_get_xy(void);
/* End Member Method APIs ---------------------------------------------------*/

#endif     







