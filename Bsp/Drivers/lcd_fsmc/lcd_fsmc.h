/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        lcd_fsmc.h
 * @author          门禁开发小组
 * @version         V1.0.3
 * @date            2018-01-26
 * @Description     bsp_lcd头文件，在此文件内定义了一些lcd常用的颜色，以及驱动液晶的
                    结构体原型，指出了lcd挂载在FSMC总线的地址，此外声明了外部调用的方
                    法接口。
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

/** 使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11 A6作为数据命令区分线
 * @note:设置时STM32内部会右移一位对其! 111 1110=0X7E	
 */    
#define LCD_BASE_ADDRESS        ((uint32_t)(0x6C000000 | 0x007FFFFE))
#define LCD_OPERATION         	((lcd_typedef *) LCD_BASE_ADDRESS)

/* 定义LCD的大小 */
#define LCD_HEIGHT          480
#define LCD_WDITH           320

/* 宏定义LCD的扫描方向 */
#define DIR_HORIZONTAL_NORMAL           0x01
#define DIR_HORIZONTAL_REVERSE          0x02
#define DIR_VERTICAL_NORMAL             0x03
#define DIR_VERTICAL_REVERSE            0x04

/* 定义LCD背光操作的宏定义 */
#define LCD_LIGHT_GPIO          GPIO_Pin_11
#define LCD_LIGHT_PORT          GPIOA
#define LCD_LIGHT_PIN_SOURCE 	GPIO_PinSource11
#define LCD_LIGHT_RCC           RCC_AHB1Periph_GPIOA
#define LCD_LIGHT_ON GPIO_SetBits(LCD_LIGHT_PORT, LCD_LIGHT_GPIO)
#define LCD_LIGHT_OFF GPIO_ResetBits(LCD_LIGHT_PORT, LCD_LIGHT_GPIO)

/* 定义是否使用手动输入LCD_ID */
#define USING_LCD_ID
#ifdef USING_LCD_ID
#define LCD_ID      0x61529
#endif
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/

/* LCD地址结构体 */    
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


/* LCD重要参数集 */
typedef struct
{					    
	uint16_t p_width;               //LCD 宽度，不随屏幕方向改变
	uint16_t p_height;              //LCD 高度，不随屏幕方向改变				    
	uint16_t width;                 //LCD 宽度
	uint16_t height;                //LCD 高度
	uint32_t id;                    //LCD ID
	uint8_t  dir;                   //横屏还是竖屏控制：0，竖屏；1，横屏。	
	uint16_t wramcmd;               //开始写gram指令
	uint16_t setxcmd;               //设置x坐标指令
	uint16_t setycmd;               //设置y坐标指令 
	uint16_t memory_acc_cont_cmd;		
	uint16_t dir_hor_normal_data;
	uint16_t dir_hor_reverse_data;
	uint16_t dir_ver_normal_data;
	uint16_t dir_ver_reverse_data;
	uint16_t disp_on_cmd;
	uint16_t disp_off_cmd;
}lcd_dev_typedef; 

/* Member method APIs --------------------------------------------------------*/
/* 访问LCD的方法成员 */
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
	 
	 



