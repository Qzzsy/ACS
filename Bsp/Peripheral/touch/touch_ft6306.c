/**
 ******************************************************************************
 * @file        touch_ft6306.c
 * @author      门禁开发小组
 * @version     V1.0.3
 * @date        2018-01-15
 * @brief       文件内包含着FT6306的操作方式，在此文件内可以获取到触摸的坐标和手势
 * @History
 * Date           Author    version    		    Notes
 * 2017-11-02     ZSY       V1.0.0          first version.
 * 2018-01-09     ZSY       V1.0.1          排版格式化操作，修改文件和方法名称.
 * 2018-01-11     ZSY       V1.0.2          增加5点触摸的支持，继续完善触摸处理过程
 * 2018-01-15     ZSY       V1.0.3          优化2点触摸的支持
 */
	
/* Includes ------------------------------------------------------------------*/
#include "touch_ft6306.h"
#include "iic.h"
#include "delay.h"

/* Private macro Definition --------------------------------------------------*/
/* reg map */								
#define DEVICE_MODE                 (0x00)      //此寄存器是设备模式寄存器, 配置它以确定芯片的当前模式，[6:4] 000是工作模式
#define GEST_ID                     (0x01)      //本寄存器描述了有效触摸的手势
#define TD_STATUS                   (0x02)      //检测到多少点，[3:0]有效
#define P1_XH                       (0x03)      //此寄存器描述第 n 个触点的 X 坐标和相应的事件标志的 MSB [7:6]事件标志，[3:0] X Position[11:8]
#define P1_XL                       (0x04)      //这个寄存器描述了第 n 个触点的 X 坐标的 LSB
#define P1_YH                       (0x05)      //此寄存器描述第 n 个触点的 Y 坐标的 MSB 和相应的触摸 ID [7:4]触摸 ID, [3:0]Y Position[11:8]
#define P1_YL                       (0x06)      //此寄存器描述第 n 个触点的 Y 坐标的 LSB
#define P2_XH                       (0x09)        
#define P2_XL                       (0x0A)        
#define P2_YH                       (0x0B)        
#define P2_YL                       (0x0C)        
#define ID_G_THGROUP                (0x80)      //灵敏度
#define ID_G_PERIODACTIVE           (0x88)      //扫描频率，本登记册描述了活动状态的期间, 它不应少于 12, 最大14
#define ID_G_AUTO_CLB_MODE          (0xA0)      //此寄存器描述自动校准模式
#define G_MODE                      (0xA4)      
#define ID_G_STATE                  (0xA7)      //此寄存器用于配置 TPM 的运行模式。
#define ID_G_FTID                   (0xA8)      //芯片ID
#define ID_G_ERR                    (0xA9)      //此寄存器在 TPM 运行时描述错误代码

/* 手势 */
#define GUSTURE_MOVE_UP             (0x10)
#define GUSTURE_MOVE_RIGHT          (0x14)
#define GUSTURE_MOVE_DOWN           (0x18)
#define GUSTURE_MOVE_LEFT           (0x1C)
#define GUSTURE_MOVE_ZOOM_IN        (0x48)
#define GUSTURE_MOVE_ZOOM_OUT       (0x49)
#define GUSTURE_NONE                (0x00)

#define CT_READ_START               (GEST_ID)               //每次检测到触摸屏中断信号时从哪个寄存器开始读取数据
#if (CT_IC == 0X6336)
#define CT_READ_LENGTH              (31)                    //每次检测到触摸屏中断信号时读取多少个数据
#else
#define CT_READ_LENGTH              (14)                    //每次检测到触摸屏中断信号时读取多少个数据
#endif
#define CT_READ_NO_GEST             (0)                     //手势在读出数据的位置
#define CT_READ_NO_TD_STATUS        (CT_READ_NO_GEST + 1)     //有效点数
#define CT_READ_NO_P1               (CT_READ_NO_TD_STATUS + 1)
#define CT_READ_NO_P2               (CT_READ_NO_P1 + 6)
#if (CT_IC == 0X6336)
#define CT_READ_NO_P3               (CT_READ_NO_P2 + 6)
#define CT_READ_NO_P4               (CT_READ_NO_P3 + 6)
#define CT_READ_NO_P5               (CT_READ_NO_P4 + 6)
#endif
#define CT_DEF_VALID_POINT_ERROR    (10)
#define CT_WAITING_ACK_TIMEOUT      (100)

#define CT_RESET_GPIO_RCC           RCC_AHB1Periph_GPIOE
#define CT_RESET_PORT               GPIOE
#define CT_RESET_PIN                GPIO_Pin_3
/* End private macro Definition ----------------------------------------------*/

/* global variable Declaration -----------------------------------------------*/
#if (CT_IC == 0X6306) || (CT_IC == 0X6336)
ct_point_typedef ct_point1;
ct_point_typedef ct_point2;
#if (CT_IC == 0X6336)
ct_point_typedef ct_point3;
ct_point_typedef ct_point4;
ct_point_typedef ct_point5;
#endif
#endif
ct_touch_status_typedef ct_touch_status;
ct_valid_point_typedef ct_valid_point;

/* User function Declaration -------------------------------------------------*/


/* User functions ------------------------------------------------------------*/

/**
 * @func    ct_write_reg_one
 * @brief   主机往触摸芯片指定的地址写入一个字节的数据
 * @param   sla_addr 设备的地址
 * @param   reg_index	写入的地址
 * @param   data 将要写入的数据
 * @retval  Operation successfully returned CT_COM_OK, otherwise CT_ACK_FAIL returned
 */
static uint8_t ct_write_reg_one(uint8_t sla_addr, uint8_t reg_index, uint8_t data)
{
    iic_start();
	
    /* Send Device Address */
    iic_send_byte(sla_addr | CT_WRITE_MASK);
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    iic_send_byte(reg_index);
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    iic_send_byte(data);
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    iic_stop();	 
    return CT_COM_OK;
}

/**
 * @func    ct_write_reg
 * @brief   主机往触摸芯片指定的地址写入指定长度的数据
 * @param   sla_addr 设备的地址
 * @param   start_addr	开始写入的地址
 * @param   *pbuf 将要写入的数据的缓存
 * @param   len 指定写入的长度
 * @retval  Operation successfully returned CT_COM_OK, otherwise CT_ACK_FAIL returned
 */
static uint8_t ct_write_reg(uint8_t sla_addr, uint8_t start_addr, uint8_t *pbuf, uint32_t len)
{
    __IO uint16_t i = 0;
    
    iic_start();
    
    /* Send Device Address */
    iic_send_byte(sla_addr | CT_WRITE_MASK);
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)	
    {
        return CT_ACK_FAIL;
    }					
    
    /* Send Sub Address */
    iic_send_byte(start_addr);	
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    /* 循环读取指定长度的数据 */
    for (i = 0; i < len; i++)
    {
        iic_send_byte(pbuf[i]);
        
        /* Read Ack */
        if (iic_wait_ack() == IIC_OPER_FAILT)
        {
            return CT_ACK_FAIL;
        }
    }
    
    iic_stop();
    return CT_COM_OK;
}

/**
 * @func    ct_read_reg_one
 * @brief   主机从触摸芯片指定的地址读取一个字节的数据
 * @param   sla_addr 设备的地址
 * @param   reg_index	开始读取的地址
 * @retval  receive 读取到的数据
 */
static uint8_t ct_read_reg_one(uint8_t sla_addr, uint8_t reg_index)
{
    unsigned char receive = 0;
    
    iic_start();
    iic_send_byte(sla_addr | CT_WRITE_MASK);
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    iic_send_byte(reg_index);
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    iic_start();
    iic_send_byte(sla_addr | CT_READ_MASK);
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    receive = iic_read_byte(IIC_NEEDNT_ACK);
    iic_stop();	 
    return receive;
}

/**
 * @func    ct_read_reg
 * @brief   主机从触摸芯片指定的地址读取指定长度的数据
 * @param   sla_addr 设备的地址
 * @param   start_addr	开始读取的地址
 * @param   *pbuf 将要读取的数据的缓存
 * @param   len 指定写入的长度
 * @retval  Operation successfully returned CT_COM_OK, otherwise CT_ACK_FAIL returned
 */
static uint8_t ct_read_reg(uint8_t sla_addr, uint8_t start_addr, uint8_t *pbuf, uint32_t len)
{
    __IO uint16_t i = 0;
    
    iic_start();
    
    /* Send Slave Address for Write */
    iic_send_byte(sla_addr | CT_WRITE_MASK);	
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    /* Send Slave Address for Read */
    iic_send_byte(start_addr);	
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    iic_stop();
    
    /* start transport data */
    iic_start();
    
    /* Send Slave Address for Read */
    iic_send_byte(sla_addr | CT_READ_MASK);	
    
    /* Read Ack */
    if (iic_wait_ack() == IIC_OPER_FAILT)
    {
        return CT_ACK_FAIL;
    }
    
    /* readlen-1 Data */
    for(i = 0; i < len - 1; i++)
    {
        pbuf[i] = iic_read_byte(IIC_NEED_ACK);	//读取1个字节
    }
    pbuf[i] = iic_read_byte(IIC_NEEDNT_ACK);	//Read the last Byte
    
    iic_stop();	 
    return CT_COM_OK;	 
}

/**
 * @func    touch_config
 * @brief   配置touch的引脚
 * @retval  无
 */
static void touch_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能CT_RESET_GPIO的时钟 */
    RCC_AHB1PeriphClockCmd(CT_RESET_GPIO_RCC, ENABLE);
        
    /**	config PIN Status	
     * alternate function IO output
     * push-Pull output
     * speed is 100MHz
     * pull output
    */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
    
    /* config iic scl line */
    GPIO_InitStructure.GPIO_Pin = CT_RESET_PIN;  		//PC5  
    GPIO_Init(CT_RESET_PORT, &GPIO_InitStructure);
    
    iic_gpio_config();
}

/**
 * @func    touch_reset
 * @brief   复位触摸芯片
 * @retval  无
 */
void touch_reset(void)
{
    delay_ms(5);
    
    //The reset signal from host to CTPM, active low, and the low pulse width should be more than or equal to 1ms
    GPIO_SetBits(CT_RESET_PORT,   CT_RESET_PIN);
    delay_ms(1);
    GPIO_ResetBits(CT_RESET_PORT, CT_RESET_PIN);
    delay_ms(5);
    GPIO_SetBits(CT_RESET_PORT,   CT_RESET_PIN);
    delay_ms(1);
}

/**
 * @func    touch_init
 * @brief   初始化touch芯片
 * @retval  无
 */
void touch_init(void)
{
    touch_config();
    touch_reset();
    
    ct_touch_status.flag_ct_int        = RESET;
    ct_touch_status.flag_point_to_deal = RESET;
    
    ct_touch_status.gesture = GUSTURE_NONE;
    ct_touch_status.n_point = 0;
#if (CT_IC == 0x6306) || (CT_IC == 0x6336)
    ct_point1.event_flag = EVENT_NONE;
    ct_point2.event_flag = EVENT_NONE;
    ct_touch_status.st_ct_point[0] = &ct_point1;
    ct_touch_status.st_ct_point[1] = &ct_point2;
#if (CT_IC == 0X6336)
    ct_point3.event_flag = EVENT_NONE;
    ct_point4.event_flag = EVENT_NONE;
    ct_point5.event_flag = EVENT_NONE;
    ct_touch_status.st_ct_point[2] = &ct_point3;
    ct_touch_status.st_ct_point[3] = &ct_point4;
    ct_touch_status.st_ct_point[4] = &ct_point5;
#endif
    ct_valid_point.valid_start_x_pos = 0;
    ct_valid_point.valid_start_y_pos = 0;
#if (CT_IC == 0x6336)
    ct_valid_point.valid_end_x_pos = 854;
    ct_valid_point.valid_end_y_pos = 480;
#elif (CT_IC == 0x6306)
    ct_valid_point.valid_end_x_pos = 320;
    ct_valid_point.valid_end_y_pos = 480;
#endif
    ct_valid_point.valid_err_range = 10;

#endif
}

/**
 * @func    get_touch_data
 * @brief   从解析获取触摸数据
 * @param   str_tmp_data 需要解析的数据
 * @param   num 解析第几个触摸点的数据
 * @param   str_tmp_data 需要解析的数据
 * @param   offset 数据偏移
 * @retval  无
 */
static void get_touch_data(uint8_t * str_tmp_data, uint8_t num, uint16_t offset)
{
    uint16_t Temp;
    //Event Flag
    ct_touch_status.st_ct_point[num]->event_flag = str_tmp_data[offset] >> 6;

    //X1 Position
    Temp = str_tmp_data[offset] & 0x0F;
    Temp = Temp << 8;
    Temp += str_tmp_data[offset + 1];
    ct_touch_status.st_ct_point[num]->x_pos = Temp; 

    //Y1 Position
    Temp = str_tmp_data[offset + 2] & 0x0F;
    Temp = Temp << 8;
    Temp += str_tmp_data[offset + 3];
    ct_touch_status.st_ct_point[num]->y_pos = Temp;
}

/**
 * @func    ct_get_touch_status
 * @brief   从ct芯片获取触摸数据并解析
 * @retval  是否有需要处理的触摸状态
 */
static uint8_t ct_get_touch_status(void)
{
    static uint8_t i;
    uint8_t str_tmp_data[CT_READ_LENGTH], error = ERROR_UNKNOW;
    
    error = ct_read_reg(CT_ADDR, CT_READ_START, str_tmp_data, CT_READ_LENGTH);
    
    if (error == CT_COM_OK)
    {
        error = ERROR_OK;
        
        ct_touch_status.gesture = str_tmp_data[CT_READ_NO_GEST]; //手势
        ct_touch_status.n_point = str_tmp_data[CT_READ_NO_TD_STATUS]; //有多少个点有效
        
        for (i = 0; i < MAX_TOUCH_POINT; i++)
        {
            ct_touch_status.st_ct_point[i]->event_flag = EVENT_NONE;
            ct_touch_status.st_ct_point[i]->pre_x_pos = ct_touch_status.st_ct_point[i]->x_pos;
            ct_touch_status.st_ct_point[i]->x_pos = TOUCH_NO_POINT; 
            ct_touch_status.st_ct_point[i]->pre_y_pos = ct_touch_status.st_ct_point[i]->y_pos;
            ct_touch_status.st_ct_point[i]->y_pos = TOUCH_NO_POINT;
        }
        
        if (ct_touch_status.n_point > 0 && ct_touch_status.n_point <= MAX_TOUCH_POINT)
        {
            for(i = 0; i < ct_touch_status.n_point; i++)
            {
                switch (i)
                {
                    case 0:get_touch_data(str_tmp_data, i, CT_READ_NO_P1);break;
                    case 1:get_touch_data(str_tmp_data, i, CT_READ_NO_P2);break;
#if (CT_IC == 0X6336)
                    case 2:get_touch_data(str_tmp_data, i, CT_READ_NO_P3);break;
                    case 3:get_touch_data(str_tmp_data, i, CT_READ_NO_P4);break;
                    case 4:get_touch_data(str_tmp_data, i, CT_READ_NO_P5);break;
#endif
                    default :break;
                }
            }
        }
        else			//没有按键按下
        {
            error = ERROR_UNKNOW;
        }
    }
    return error;
}

/**
 * @func    CT_Check_Point_Valid
 * @brief   检验获取到的数据的是否在规定的区域内
 * @param   p_valid_point 待对比当前有效触摸区域
 * @retval  是否有需要处理的触摸状态
 */
static uint8_t CT_Check_Point_Valid(ct_valid_point_typedef *p_valid_point)
{
    uint8_t Stauts = ERROR_TOUCH_POINT_INVALID, n;
    uint16_t ValidStart, ValidEnd; 
            
    for(n = 0; n < ct_touch_status.n_point; n++)
    {
        //检测X,Y坐标是否都在有效区域内
        if((p_valid_point->valid_start_x_pos + p_valid_point->valid_err_range) > 
            p_valid_point->valid_err_range)
        {
            ValidStart = p_valid_point->valid_start_x_pos - p_valid_point->valid_err_range;
        }
        else
        {
            ValidStart = p_valid_point->valid_start_x_pos;
        }
        ValidEnd = p_valid_point->valid_end_x_pos + p_valid_point->valid_err_range;
                    
        if((ct_touch_status.st_ct_point[n]->x_pos >= ValidStart) && (ct_touch_status.st_ct_point[n]->x_pos <= ValidEnd))
        {
            if((p_valid_point->valid_start_y_pos + p_valid_point->valid_err_range) > 
                p_valid_point->valid_err_range)
            {
                ValidStart = p_valid_point->valid_start_y_pos - p_valid_point->valid_err_range;
            }
            else
            {
                ValidStart = p_valid_point->valid_start_y_pos;
            }
            ValidEnd = p_valid_point->valid_end_y_pos + p_valid_point->valid_err_range;
                        
            if((ct_touch_status.st_ct_point[n]->y_pos >= ValidStart) && (ct_touch_status.st_ct_point[n]->y_pos <= ValidEnd))
            {
                Stauts = ERROR_TOUCH_POINT_VALID;
            }
        }
    }
    return Stauts;
}

/**
 * @func    get_touch_xy
 * @brief   从ct获取触摸的xy坐标
 * @retval  true 触摸 false 没有触摸
 */
uint8_t touch_get_xy(void)
{
    ct_touch_status.flag_point_to_deal = False;
    if (ct_get_touch_status() == ERROR_OK)
    {
        if (CT_Check_Point_Valid(&ct_valid_point) == ERROR_TOUCH_POINT_VALID)
        {
            ct_touch_status.flag_point_to_deal = True;
            return True;
        }
    }
    return False;
}








