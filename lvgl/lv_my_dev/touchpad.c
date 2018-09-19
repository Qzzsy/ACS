/**
 * @file indev.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
 
#include "usart.h"
#include "touchpad.h"
#include "lv_hal/lv_hal.h"
#include "touch_ft6306.h"
#include "lcd_fsmc.h"

/*********************
 *      DEFINES
 *********************/
#define TOUCH_AD_TOP		160  	//���´������Ķ�����д�� Y ��ģ������ֵ��
#define TOUCH_AD_BOTTOM		3990 	//���´������ĵײ���д�� Y ��ģ������ֵ��
#define TOUCH_AD_LEFT 		160		//���´���������࣬д�� X ��ģ������ֵ��
#define TOUCH_AD_RIGHT		3990	//���´��������Ҳ࣬д�� X ��ģ������ֵ��
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool touchpad_read(lv_indev_data_t *data);

/**********************
 *  STATIC VARIABLES
 **********************/
//static TS_StateTypeDef  TS_State;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize your input devices here
 */
void touchpad_init(void)
{

  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.read = touchpad_read;
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read an input device
 * @param indev_id id of the input device to read
 * @param x put the x coordinate here
 * @param y put the y coordinate here
 * @return true: the device is pressed, false: released
 */
static bool touchpad_read(lv_indev_data_t *data)
{
	static int16_t last_x = 0;
	static int16_t last_y = 0;
	if(touch_get_xy())//�а�������ʱ
	{

			data->point.x = ct_touch_status.st_ct_point[0]->y_pos;
			data->point.y = 320-ct_touch_status.st_ct_point[0]->x_pos;
			data->state = LV_INDEV_STATE_PR;
			last_x = data->point.x;
			last_y = data->point.y;


	}
	else
    {
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_REL;
	}

	return false;
}
