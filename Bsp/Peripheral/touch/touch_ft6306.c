/**
 ******************************************************************************
 * @file        touch_ft6306.c
 * @author      �Ž�����С��
 * @version     V1.0.3
 * @date        2018-01-15
 * @brief       �ļ��ڰ�����FT6306�Ĳ�����ʽ���ڴ��ļ��ڿ��Ի�ȡ�����������������
 * @History
 * Date           Author    version    		    Notes
 * 2017-11-02     ZSY       V1.0.0          first version.
 * 2018-01-09     ZSY       V1.0.1          �Ű��ʽ���������޸��ļ��ͷ�������.
 * 2018-01-11     ZSY       V1.0.2          ����5�㴥����֧�֣��������ƴ����������
 * 2018-01-15     ZSY       V1.0.3          �Ż�2�㴥����֧��
 */
	
/* Includes ------------------------------------------------------------------*/
#include "touch_ft6306.h"
#include "iic.h"
#include "delay.h"

/* Private macro Definition --------------------------------------------------*/
/* reg map */								
#define DEVICE_MODE                 (0x00)      //�˼Ĵ������豸ģʽ�Ĵ���, ��������ȷ��оƬ�ĵ�ǰģʽ��[6:4] 000�ǹ���ģʽ
#define GEST_ID                     (0x01)      //���Ĵ�����������Ч����������
#define TD_STATUS                   (0x02)      //��⵽���ٵ㣬[3:0]��Ч
#define P1_XH                       (0x03)      //�˼Ĵ��������� n ������� X �������Ӧ���¼���־�� MSB [7:6]�¼���־��[3:0] X Position[11:8]
#define P1_XL                       (0x04)      //����Ĵ��������˵� n ������� X ����� LSB
#define P1_YH                       (0x05)      //�˼Ĵ��������� n ������� Y ����� MSB ����Ӧ�Ĵ��� ID [7:4]���� ID, [3:0]Y Position[11:8]
#define P1_YL                       (0x06)      //�˼Ĵ��������� n ������� Y ����� LSB
#define P2_XH                       (0x09)        
#define P2_XL                       (0x0A)        
#define P2_YH                       (0x0B)        
#define P2_YL                       (0x0C)        
#define ID_G_THGROUP                (0x80)      //������
#define ID_G_PERIODACTIVE           (0x88)      //ɨ��Ƶ�ʣ����Ǽǲ������˻״̬���ڼ�, ����Ӧ���� 12, ���14
#define ID_G_AUTO_CLB_MODE          (0xA0)      //�˼Ĵ��������Զ�У׼ģʽ
#define G_MODE                      (0xA4)      
#define ID_G_STATE                  (0xA7)      //�˼Ĵ����������� TPM ������ģʽ��
#define ID_G_FTID                   (0xA8)      //оƬID
#define ID_G_ERR                    (0xA9)      //�˼Ĵ����� TPM ����ʱ�����������

/* ���� */
#define GUSTURE_MOVE_UP             (0x10)
#define GUSTURE_MOVE_RIGHT          (0x14)
#define GUSTURE_MOVE_DOWN           (0x18)
#define GUSTURE_MOVE_LEFT           (0x1C)
#define GUSTURE_MOVE_ZOOM_IN        (0x48)
#define GUSTURE_MOVE_ZOOM_OUT       (0x49)
#define GUSTURE_NONE                (0x00)

#define CT_READ_START               (GEST_ID)               //ÿ�μ�⵽�������ж��ź�ʱ���ĸ��Ĵ�����ʼ��ȡ����
#if (CT_IC == 0X6336)
#define CT_READ_LENGTH              (31)                    //ÿ�μ�⵽�������ж��ź�ʱ��ȡ���ٸ�����
#else
#define CT_READ_LENGTH              (14)                    //ÿ�μ�⵽�������ж��ź�ʱ��ȡ���ٸ�����
#endif
#define CT_READ_NO_GEST             (0)                     //�����ڶ������ݵ�λ��
#define CT_READ_NO_TD_STATUS        (CT_READ_NO_GEST + 1)     //��Ч����
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
 * @brief   ����������оƬָ���ĵ�ַд��һ���ֽڵ�����
 * @param   sla_addr �豸�ĵ�ַ
 * @param   reg_index	д��ĵ�ַ
 * @param   data ��Ҫд�������
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
 * @brief   ����������оƬָ���ĵ�ַд��ָ�����ȵ�����
 * @param   sla_addr �豸�ĵ�ַ
 * @param   start_addr	��ʼд��ĵ�ַ
 * @param   *pbuf ��Ҫд������ݵĻ���
 * @param   len ָ��д��ĳ���
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
    
    /* ѭ����ȡָ�����ȵ����� */
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
 * @brief   �����Ӵ���оƬָ���ĵ�ַ��ȡһ���ֽڵ�����
 * @param   sla_addr �豸�ĵ�ַ
 * @param   reg_index	��ʼ��ȡ�ĵ�ַ
 * @retval  receive ��ȡ��������
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
 * @brief   �����Ӵ���оƬָ���ĵ�ַ��ȡָ�����ȵ�����
 * @param   sla_addr �豸�ĵ�ַ
 * @param   start_addr	��ʼ��ȡ�ĵ�ַ
 * @param   *pbuf ��Ҫ��ȡ�����ݵĻ���
 * @param   len ָ��д��ĳ���
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
        pbuf[i] = iic_read_byte(IIC_NEED_ACK);	//��ȡ1���ֽ�
    }
    pbuf[i] = iic_read_byte(IIC_NEEDNT_ACK);	//Read the last Byte
    
    iic_stop();	 
    return CT_COM_OK;	 
}

/**
 * @func    touch_config
 * @brief   ����touch������
 * @retval  ��
 */
static void touch_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* ʹ��CT_RESET_GPIO��ʱ�� */
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
 * @brief   ��λ����оƬ
 * @retval  ��
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
 * @brief   ��ʼ��touchоƬ
 * @retval  ��
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
 * @brief   �ӽ�����ȡ��������
 * @param   str_tmp_data ��Ҫ����������
 * @param   num �����ڼ��������������
 * @param   str_tmp_data ��Ҫ����������
 * @param   offset ����ƫ��
 * @retval  ��
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
 * @brief   ��ctоƬ��ȡ�������ݲ�����
 * @retval  �Ƿ�����Ҫ����Ĵ���״̬
 */
static uint8_t ct_get_touch_status(void)
{
    static uint8_t i;
    uint8_t str_tmp_data[CT_READ_LENGTH], error = ERROR_UNKNOW;
    
    error = ct_read_reg(CT_ADDR, CT_READ_START, str_tmp_data, CT_READ_LENGTH);
    
    if (error == CT_COM_OK)
    {
        error = ERROR_OK;
        
        ct_touch_status.gesture = str_tmp_data[CT_READ_NO_GEST]; //����
        ct_touch_status.n_point = str_tmp_data[CT_READ_NO_TD_STATUS]; //�ж��ٸ�����Ч
        
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
        else			//û�а�������
        {
            error = ERROR_UNKNOW;
        }
    }
    return error;
}

/**
 * @func    CT_Check_Point_Valid
 * @brief   �����ȡ�������ݵ��Ƿ��ڹ涨��������
 * @param   p_valid_point ���Աȵ�ǰ��Ч��������
 * @retval  �Ƿ�����Ҫ����Ĵ���״̬
 */
static uint8_t CT_Check_Point_Valid(ct_valid_point_typedef *p_valid_point)
{
    uint8_t Stauts = ERROR_TOUCH_POINT_INVALID, n;
    uint16_t ValidStart, ValidEnd; 
            
    for(n = 0; n < ct_touch_status.n_point; n++)
    {
        //���X,Y�����Ƿ�����Ч������
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
 * @brief   ��ct��ȡ������xy����
 * @retval  true ���� false û�д���
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








