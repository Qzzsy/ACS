/**
 ******************************************************************************
 * @Copyright     (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename      touch_ft6306.h
 * @author        门禁开发小组
 * @version       V1.0.3
 * @date          2017-01-15
 * @Description   touch_ft6306文件，在此文件内定义了ft6306常用的命令，ft6306对应的
                  属性结构体以及对外开放的API接口
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-02     ZSY       V1.0.0        first version.
 * 2017-01-09     ZSY       V1.0.1        排版格式化操作，修改文件和方法名称.
 * 2018-01-11     ZSY       V1.0.2        增加5点触摸的支持，继续完善触摸处理过程
 * 2018-01-15     ZSY       V1.0.3        优化2点触摸的支持
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
	
/*	相关宏定义		*/
#define TOUCH_NO_POINT              (~0)
      
/*	错误代码		*/      
#define CT_COM_OK                   (0)
#define CT_ACK_FAIL                 (1)

#define ERROR_UNKNOW                (0)
#define ERROR_OK                    (1)
	
#define ERROR_TOUCH_POINT_INVALID   (0)
#define ERROR_TOUCH_POINT_VALID     (1)

#define FT6X06_ID                   (0x6306)
#define CT_IC                       (FT6X06_ID)	//使用的控制IC
#define CT_ADDR                     (0x70)      //器件地址
#define CT_WRITE_MASK               (0x00)		
#define CT_READ_MASK                (0x01)		

/*********************相关结构体*************************/
#if (CT_IC == 0X6336)
#define MAX_TOUCH_POINT             (5)         //最大触摸点个数
#elif (CT_IC == 0X6306)
#define MAX_TOUCH_POINT             (2)         //最大触摸点个数
#endif
#ifndef MAX_TOUCH_POINT
#define MAX_TOUCH_POINT             (1)
#endif

/* 触摸点事件 */
#define EVENT_PRESS_DOWN            (0) 
#define EVENT_LIFT_UP               (1)
#define EVENT_CONTACT               (2)
#define EVENT_NONE                  (3)
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
/* 定义一些相关的结构体 */
typedef struct
{
	uint8_t event_flag;             //触摸点事件
	uint16_t x_pos;
	uint16_t y_pos;
	uint16_t pre_x_pos;             //上一次的点位置
	uint16_t pre_y_pos;
}ct_point_typedef;

typedef struct
{
	uint8_t n_point;                //当前有效，触摸点个数,对于FT6X06最多两个
	uint8_t gesture;                //手势
	uint8_t flag_ct_int;            //触摸中断标志
	uint8_t flag_point_to_deal;     //是否有点有效标志			
	ct_point_typedef *st_ct_point[MAX_TOUCH_POINT]; 
}ct_touch_status_typedef;

typedef struct
{
	uint16_t valid_start_x_pos;     //X有效起始位置
	uint16_t valid_end_x_pos;       //X有效结束位置
	uint16_t valid_start_y_pos;     //Y有效起始位置
	uint16_t valid_end_y_pos;       //Y有效结束位置
	uint8_t valid_err_range;        //容许偏差
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







