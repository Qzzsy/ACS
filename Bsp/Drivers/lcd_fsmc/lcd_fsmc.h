/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        lcd_fsmc.h
 * @author          �Ž�����С��
 * @version         V1.0.3
 * @date            2018-01-26
 * @Description     bsp_lcdͷ�ļ����ڴ��ļ��ڶ�����һЩlcd���õ���ɫ���Լ�����Һ����
                    �ṹ��ԭ�ͣ�ָ����lcd������FSMC���ߵĵ�ַ�������������ⲿ���õķ�
                    ���ӿڡ�
 * @Others
 * @History
 * Date             Author      version         Notes
 * 2018-8-8         Zehn        V1.0.0          first version.
 
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LCD_FSMC_H_
#define _LCD_FSMC_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"	 

/* Public macro Definition ---------------------------------------------------*/

/** ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A6��Ϊ��������������
 * @note:����ʱSTM32�ڲ�������һλ����! 111 1110=0X7E	
 */    
#define LCD_BASE_ADDRESS        ((uint32_t)(0x6C000000 | 0x007FFFFE))
#define LCD_OPERATION         	((lcd_typedef *) LCD_BASE_ADDRESS)

/* ����LCD�Ĵ�С */
#define LCD_HEIGHT          480
#define LCD_WDITH           320

/* �궨��LCD��ɨ�跽�� */
#define DIR_HORIZONTAL_NORMAL           0x01
#define DIR_HORIZONTAL_REVERSE          0x02
#define DIR_VERTICAL_NORMAL             0x03
#define DIR_VERTICAL_REVERSE            0x04

/* ����LCD��������ĺ궨�� */
#define LCD_LIGHT_GPIO          GPIO_Pin_11
#define LCD_LIGHT_PORT          GPIOA
#define LCD_LIGHT_PIN_SOURCE 	GPIO_PinSource11
#define LCD_LIGHT_RCC           RCC_AHB1Periph_GPIOA
#define LCD_LIGHT_ON GPIO_SetBits(LCD_LIGHT_PORT, LCD_LIGHT_GPIO)
#define LCD_LIGHT_OFF GPIO_ResetBits(LCD_LIGHT_PORT, LCD_LIGHT_GPIO)

/* �����Ƿ�ʹ���ֶ�����LCD_ID */
#define USING_LCD_ID
#ifdef USING_LCD_ID
#define LCD_ID      0x61529
#endif
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/

/* LCD��ַ�ṹ�� */    
typedef struct
{
    __IO uint16_t REG;
    __IO uint16_t RAM;
}lcd_typedef;

typedef union
{
    __IO uint16_t pos;
    struct
    {
        __IO uint8_t l_bit;
        __IO uint8_t h_bit;
    }Pos;
}pos_typedef;


/* LCD��Ҫ������ */
typedef struct
{					    
	uint16_t p_width;               //LCD ��ȣ�������Ļ����ı�
	uint16_t p_height;              //LCD �߶ȣ�������Ļ����ı�				    
	uint16_t width;                 //LCD ���
	uint16_t height;                //LCD �߶�
	uint32_t id;                    //LCD ID
	uint8_t  dir;                   //���������������ƣ�0��������1��������	
	uint16_t wramcmd;               //��ʼдgramָ��
	uint16_t setxcmd;               //����x����ָ��
	uint16_t setycmd;               //����y����ָ�� 
	uint16_t memory_acc_cont_cmd;		
	uint16_t dir_hor_normal_data;
	uint16_t dir_hor_reverse_data;
	uint16_t dir_ver_normal_data;
	uint16_t dir_ver_reverse_data;
	uint16_t disp_on_cmd;
	uint16_t disp_off_cmd;
}lcd_dev_typedef; 

/* Member method APIs --------------------------------------------------------*/
/* ����LCD�ķ�����Ա */
typedef struct
{
    void (*init)(void);
    
    void (*lcd_wr_ram_prepare)(void);
    
    void (*set_disp_dir)(uint8_t dir);
    
    void (*set_disp_win)(uint16_t cur_x, uint16_t cur_y, 
                        uint16_t w_height, uint16_t w_width);
    
    void (*set_disp_cur)(uint16_t x_pos, uint16_t y_pos);
    
    void (*draw_colorbox)(uint16_t cur_x, uint16_t cur_y, 
                        uint16_t x_size, uint16_t y_size, 
                        uint16_t p_color);
    
    void (*refresh_colorbox)(uint16_t x_start, uint16_t y_start, 
                            uint16_t x_end, uint16_t y_end, 
                            uint16_t p_color);
    
    uint16_t (*gbr_to_rgb)(uint16_t c);
    
    void (*disp_on)(void);
    
    void (*disp_off)(void);
    
    uint16_t (*get_x_size)(void);
    
    uint16_t (*get_y_size)(void);
    
    void (*clr_scr)(uint16_t p_color);
    
    void (*put_pixel)(uint16_t cur_x, uint16_t cur_y, 
                    uint16_t p_color);
    
    uint16_t (*get_pixel)(uint16_t cur_x, uint16_t cur_y);
    
    void (*draw_line)(uint16_t x_start, uint16_t y_start, 
                    uint16_t x_end, uint16_t y_end, 
                    uint16_t p_color);
    
    void (*draw_hor_line)(uint16_t x_start, uint16_t y_start, 
                        uint16_t x_end, uint16_t p_color);
    
    void (*draw_ver_line)(uint16_t x_start, uint16_t y_start, 
                        uint16_t y_end, uint16_t p_color);
    
    void (*draw_hor_color_line)(uint16_t cur_x , uint16_t cur_y,
                                uint16_t d_width, const uint16_t *p_color);
    
    void (*draw_hor_trans_line)(uint16_t cur_x, uint16_t cur_y, 
                                uint16_t d_width, const uint16_t *p_color);
    
    void (*draw_rect)(uint16_t cur_x, uint16_t cur_y, 
                    uint16_t d_width, uint16_t d_height, 
                    uint16_t p_olor);
    
    void (*fill_rect)(uint16_t cur_x, uint16_t cur_y, 
                    uint16_t d_width, uint16_t d_height, 
                    uint16_t p_olor);
    
    void (*draw_circle)(uint16_t cur_x, uint16_t cur_y, 
                        uint16_t r_radius, uint16_t p_color);
    
    void (*draw_bmp)(uint16_t cur_x, uint16_t cur_y, 
                    uint16_t d_width, uint16_t d_height, 
                    uint16_t *ptr);
    
}lcd_func_typedef;

extern lcd_dev_typedef lcd_dev;
extern lcd_func_typedef lcd;
void lcd_func_init(void);	
/* End Member Method APIs ---------------------------------------------------*/
/* UserCode end -------------------------------------------------------------*/

#endif  
	 
	 



