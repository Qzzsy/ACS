/**
 ******************************************************************************
 * @file        rfid_rc522.c
 * @author      �Ž�����С��
 * @version     V1.0.0
 * @date        2018-01-01
 * @brief       ����ļ���rfid�ķ��ʷ�����ʵ����rfid�Ļ��������������ṩ��ȫ��
                ���ʣ�֧�ִ��豸���ر��豸�������豸����д�豸����ʼ���豸��
 * @note        ʹ��ǰ��Ҫ����rfid_init_config();  �����޷�ʹ�ô˷���
 * @History
 * Date             Author      version                 Notes
 * 2018-01-01        ZSY         V1.0.0             first version.
 */
	
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <rtdevice.h>

#include "rfid_rc522.h"
#include "rtthread.h"

#include "spi_hard.h"

/* Private macro Definition --------------------------------------------------*/

/* MF522�Ĵ�������*/
/* PAGE 0 */
#define     RFU00                  0x00    
#define     COMMAND_REG            0x01    
#define     COMM_IEN_REG           0x02    
#define     DIVL_EN_REG            0x03    
#define     COMM_IRQ_REG           0x04    
#define     DIV_IRQ_REG            0x05
#define     ERROR_REG              0x06    
#define     STATUS1_REG            0x07    
#define     STATUS2_REG            0x08    
#define     FIFO_DATA_REG          0x09
#define     FIFO_LEVEL_REG         0x0A
#define     Water_LEVEL_REG        0x0B
#define     CONTROL_REG            0x0C
#define     BIT_FRAMING_REG        0x0D
#define     COLL_REG               0x0E
#define     RFU0F                  0x0F
/* PAGE 1*/     
#define     RFU10                  0x10
#define     MODE_REG               0x11
#define     TX_MODE_REG            0x12
#define     RX_MODE_REG            0x13
#define     TX_CONTROL_REG         0x14
#define     TX_AUTO_REG            0x15
#define     TX_SEL_REG             0x16
#define     RX_SEL_REG             0x17
#define     RX_THRESHOLD_REG       0x18
#define     DEMOD_REG              0x19
#define     RFU1A                  0x1A
#define     RFU1B                  0x1B
#define     MIFARE_REG             0x1C
#define     RFU1D                  0x1D
#define     RFU1E                  0x1E
#define     SERIAL_SPEED_REG       0x1F
/* PAGE 2*/ 
#define     RFU20                  0x20  
#define     CRC_RESULT_REG_M       0x21
#define     CRC_RESULT_REG_L       0x22
#define     RFU23                  0x23
#define     MOD_WIDTH_REG          0x24
#define     RFU25                  0x25
#define     RF_CFG_REG             0x26
#define     GS_N_REG               0x27
#define     CW_GS_CFG_REG          0x28
#define     MOD_GS_CFG_REG         0x29
#define     T_MODE_REG             0x2A
#define     T_PRESCALER_REG        0x2B
#define     T_RELOAD_REG_H         0x2C
#define     T_RELOAD_REG_L         0x2D
#define     T_COUNTER_VALUE_REG_H  0x2E
#define     T_COUNTER_VALUE_REG_L  0x2F
/* PAGE 3*/      
#define     RFU30                  0x30
#define     TEST_SEL1_REG          0x31
#define     TEST_SEL2_REG          0x32
#define     TEST_PIN_EN_REG        0x33
#define     TEST_PIN_VALUE_REG     0x34
#define     TEST_BUS_REG           0x35
#define     AUTO_TEST_REG          0x36
#define     VERSION_REG            0x37
#define     ANALOG_TEST_REG        0x38
#define     TEST_DAC1_REG          0x39  
#define     TEST_DAC2_REG          0x3A   
#define     TEST_ADC_REG           0x3B   
#define     RFU3C                  0x3C   
#define     RFU3D                  0x3D   
#define     RFU3E                  0x3E   
#define     RFU3F		           0x3F
/* End private macro Definition ----------------------------------------------*/

/* global variable Declaration -----------------------------------------------*/
static struct spi_rfid_device spi_rfid_device;

/* User function Declaration -------------------------------------------------*/

/* User functions ------------------------------------------------------------*/
/**
 * @func	rfid_lock
 * @brief 	��rfid�����ϻ���������ֹ�����̷߳���
 * @param	rfid_device �豸���������
 * @note  	
 * @retval	��
 */
static void rfid_lock(rt_spi_rfid_device_t rfid_device)
{
    rt_mutex_take(&rfid_device->lock, RT_WAITING_FOREVER);
}

/**
 * @func	rfid_unlock
 * @brief 	��rfid�����ͷŻ����������������̷߳���
 * @param	rfid_device �豸���������
 * @note  	
 * @retval	��
 */
static void rfid_unlock(rt_spi_rfid_device_t rfid_device)
{
    rt_mutex_release(&rfid_device->lock);
}

/**
 * @func    read_raw_rc
 * @brief   rfid������
 * @param   address Ҫ�����ݵĵ�ַ
 * @param   buffer ���ݻ�����
 * @note    ��
 * @retval  ��
 */
static void read_raw_rc(rt_uint8_t address, rt_uint8_t * buffer)
{
	rt_uint8_t send_buff[1] = {0};
	
	send_buff[0] = ((address << 1) & 0x7E) | 0x80;
	
	/* ������ */
	rt_spi_send_then_recv(spi_rfid_device.rt_spi_device, send_buff, 1, buffer, 1);
}

/**
 * @func    write_raw_rc
 * @brief   rfidд����
 * @param   address Ҫд���ݵĵ�ַ
 * @param   buffer ���ݻ�����
 * @note    ��
 * @retval  ��
 */
static void write_raw_rc(rt_uint8_t address, const rt_uint8_t * buffer)
{
	rt_uint8_t send_buff[1] = {0};
	
	send_buff[0] = (address << 1) & 0x7E;
	
	/* д���� */
	rt_spi_send_then_send(spi_rfid_device.rt_spi_device, send_buff, 1, buffer, 1);
}

/**
 * @func    clear_bit_mask
 * @brief   �������
 * @param   reg �Ĵ�����ַ
 * @param   mask ����ֵ
 * @note    ��
 * @retval  RT_EOK
 */
static rt_err_t clear_bit_mask(rt_uint8_t reg, rt_uint8_t mask)
{
	rt_uint8_t tmp = 0x00;
	
	read_raw_rc(reg, &tmp);
	
	tmp = tmp & (~mask);
	
	write_raw_rc(reg, &tmp);
	
	return RT_EOK;
}

/**
 * @func    set_bit_mask
 * @brief   ��������
 * @param   reg �Ĵ�����ַ
 * @param   mask ����ֵ
 * @note    ��
 * @retval  RT_EOK
 */
static rt_err_t set_bit_mask(rt_uint8_t reg, rt_uint8_t mask)
{
	rt_uint8_t tmp = 0x00;
	
	read_raw_rc(reg, &tmp);
	
	tmp = tmp | mask;
	
	write_raw_rc(reg, &tmp);
	
	return RT_EOK;
}

/**
 * @func    calulate_crc
 * @brief   ����RFID�������CRC
 * @param   p_in_data ���뻺��
 * @param   len �������ݵĳ���
 * @param   p_out_data �������
 * @note    ��
 * @retval  RT_EOK
 */
static rt_err_t calulate_crc(const rt_uint8_t * p_in_data, rt_uint8_t len, rt_uint8_t * p_out_data)
{
	rt_uint8_t i, n, cmd;
	
	clear_bit_mask(DIV_IRQ_REG, 0x04);
	
	cmd = PCD_IDLE;
	write_raw_rc(COMMAND_REG, &cmd);
	
	set_bit_mask(FIFO_LEVEL_REG, 0x80);
	
	for (i = 0; i < len; i++)
	{
		write_raw_rc(FIFO_DATA_REG, &p_in_data[i]);
	}
	
	cmd = PCD_CALCCRC;
	write_raw_rc(COMMAND_REG, &cmd);
	
	i = 0xff;
	
	do
	{
		read_raw_rc(DIV_IRQ_REG, &n);
		i--;
	} while ((i != 0) && !(n & 0x04));
	
	read_raw_rc(CRC_RESULT_REG_L, &p_out_data[0]);
	read_raw_rc(CRC_RESULT_REG_M, &p_out_data[1]);
	
	return RT_EOK;
}

/**
 * @func    pcd_com_mf522
 * @brief   ͨ��RC522��ISO14443��ͨѶ
 * @param   command RC522������
 * @param   p_in_data ͨ��RC522���͵���Ƭ������
 * @param   in_len_byte �������ݵ��ֽڳ���
 * @param   p_out_data ���յ��Ŀ�Ƭ��������
 * @param   p_out_len_byte �������ݵ�λ����
 * @note    ��
 * @retval  status ����״ֵ̬
 */
static rt_uint8_t pcd_com_mf522(rt_uint8_t command,
						        const rt_uint8_t * p_in_data,
						        rt_uint8_t in_len_byte,
						        rt_uint8_t * p_out_data,
						        rt_uint16_t * p_out_len_byte)
{
	char status = MI_ERR;
	rt_uint8_t irq_en = 0x00;
	rt_uint8_t wait_for = 0x00;
	rt_uint8_t last_bits;
	rt_uint8_t n = 0, cmd, tmp = 0;
	rt_uint16_t i;
	
	switch (command)
	{
		case PCD_AUTHENT :
		{
			irq_en = 0x12;
			wait_for = 0x10;
		}break;
		case PCD_TRANSCEIVE :
		{
			irq_en = 0x77;
			wait_for = 0x30;
		}break;
		default :
			break;
	}
	
	cmd = irq_en | 0x80;
	write_raw_rc(COMM_IEN_REG, &cmd);
	
	clear_bit_mask(COMM_IRQ_REG, 0x80);
	
	cmd = PCD_IDLE;
	write_raw_rc(COMMAND_REG, &cmd);
	
	set_bit_mask(FIFO_LEVEL_REG, 0x80);
	
	for (i = 0; i < in_len_byte; i++)
	{
		write_raw_rc(FIFO_DATA_REG, &p_in_data[i]);
	}
	
	write_raw_rc(COMMAND_REG, &command);
	
	if (command == PCD_TRANSCEIVE)
	{
		set_bit_mask(BIT_FRAMING_REG, 0x80);
	}
	
	i = 600;
	do
	{
		read_raw_rc(COMM_IRQ_REG, &n);
		i--;
	} while ((i != 0) && (!(n & 0x01)) && (!(n & wait_for)));
	
	clear_bit_mask(BIT_FRAMING_REG, 0x80);
	
	if (i != 0)
	{
		read_raw_rc(ERROR_REG, &tmp);
		if (!(tmp & 0x1B))
		{
			status = MI_OK;
			if (n & irq_en & 0x01)
			{
				status = MI_NOTAGERR;
			}
			if (command == PCD_TRANSCEIVE)
			{
				read_raw_rc(FIFO_LEVEL_REG, &n);
				read_raw_rc(CONTROL_REG, &tmp);
				last_bits = tmp & 0x07;
				
				if (last_bits)
				{
					*p_out_len_byte = (n - 1) * 8 + last_bits;
				}
				else
				{
					*p_out_len_byte = n * 8;
				}
				
				if (n == 0)
				{
					n = 1;
				}
				
				if (n > MAXRLEN)
				{
					n = MAXRLEN;
				}
				
				for (i = 0; i < n; i++)
				{
					read_raw_rc(FIFO_DATA_REG, &p_out_data[i]);
				}
				
			}
		}
		else
		{
			status = MI_ERR;
		}
	}
	
	set_bit_mask(CONTROL_REG, 0x80);
	
	cmd = PCD_IDLE;
	write_raw_rc(COMMAND_REG, &cmd);
	
	return status;
}

/**
 * @func    pcd_read
 * @brief   ��ȡM1��һ������
 * @param   addr ���ַ
 * @param   p_data ���������ݣ�16�ֽ�				
 * @retval	�ɹ�����MI_OK�����򷵻�MI_ERR
 */
static char pcd_read(rt_uint8_t addr,rt_uint8_t * p_data, rt_uint8_t size)
{
    char status;
    rt_uint16_t  len;
    rt_uint8_t i, com_mf522_buf[MAXRLEN]; 

    com_mf522_buf[0] = PICC_READ;
    com_mf522_buf[1] = addr;
    calulate_crc(com_mf522_buf, 2, &com_mf522_buf[2]);
   
    status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, 4, com_mf522_buf, &len);
	
    if ((status == MI_OK) && (len == 0x90))
    {
        for (i = 0; i < size; i++)
        {    
			*(p_data + i) = com_mf522_buf[i];
		}
    }
    else
    {   
		status = MI_ERR;
	}
    
    return status;
}

/**
 * @func    pcd_write
 * @brief   д���ݵ�M1��һ��
 * @param   addr ���ַ
 * @param   p_data ���������ݣ�16�ֽ�				
 * @retval	�ɹ�����MI_OK�����򷵻�MI_ERR
 */            
static char pcd_write(rt_uint8_t addr,const rt_uint8_t * p_data, rt_uint8_t size)
{
    char status;
    rt_uint16_t  len;
    rt_uint8_t i, com_mf522_buf[MAXRLEN]; 
	
    com_mf522_buf[0] = PICC_WRITE;
    com_mf522_buf[1] = addr;
    calulate_crc(com_mf522_buf, 2, &com_mf522_buf[2]);
 
    status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, 4, com_mf522_buf, &len);

    if ((status != MI_OK) || (len != 4) || ((com_mf522_buf[0] & 0x0F) != 0x0A))
    {   
		status = MI_ERR;
	}
        
    if (status == MI_OK)
    {
        for (i = 0; i < size; i++)
        {    
			com_mf522_buf[i] = *(p_data + i);
		}
		
        calulate_crc(com_mf522_buf, size, &com_mf522_buf[size]);

        status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, size + 2, com_mf522_buf,&len);
        if ((status != MI_OK) || (len != 4) || ((com_mf522_buf[0] & 0x0F) != 0x0A))
        {   
			status = MI_ERR;
		}
    }
    
    return status;
}

/**
 * @func    pcd_antenna_on
 * @brief   ��������  
 * @note    ÿ��������ر����߷���֮��Ӧ������1ms�ļ��               
 * @retval  ��
 */
static void pcd_antenna_on(void)
{
	rt_uint8_t tmp = 0x00;
	
	read_raw_rc(TX_CONTROL_REG, &tmp);
	if (!(tmp & 0x03))
	{
		set_bit_mask(TX_CONTROL_REG, 0x03);
	}
}

/**
 * @func    pcd_antenna_off
 * @brief   �ر�����
 * @note    ÿ��������ر����߷���֮��Ӧ������1ms�ļ��               
 * @retval  ��
 */
static void pcd_antenna_off(void)
{
	clear_bit_mask(TX_CONTROL_REG, 0x03);
}

/**
 * @func    rc522_gpio_config
 * @brief   ����rc522����
 * @note    ��              
 * @retval  ��
 */
static void rc522_gpio_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;//PD0,1,4,5,8,9,10,14,15 AF OUT
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;//�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
}

/**
 * @func    pcd_halt
 * @brief   ���Ƭ��������״̬
 * @note    ��
 * @retval  RT_EOK
 */
static rt_uint8_t pcd_halt(void)
{
	rt_uint16_t len;
	
	rt_uint8_t com_mf522_buf[MAXRLEN];
	
	com_mf522_buf[0] = PICC_HALT;
	com_mf522_buf[1] = 0;
	
	calulate_crc(com_mf522_buf, 2, &com_mf522_buf[2]);
	
	pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, 4, com_mf522_buf, &len);
	
	return RT_EOK;
}

/**
 * @func    pcd_request
 * @brief   Ѱ��
 * @param   reg_code Ѱ����ʽ
              0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
              0x26 = Ѱδ��������״̬�Ŀ�
 * @param	p_tag_type ��Ƭ���ʹ���
              0x4400 = Mifare_UltraLight
              0x0400 = Mifare_One(S50)
              0x0200 = Mifare_One(S70)
              0x0800 = Mifare_Pro(X)
              0x4403 = Mifare_DESFire
 * @retval	�ɹ�����MI_OK�����򷵻�MI_ERR
 */
static char pcd_request(rt_uint8_t reg_code, rt_uint8_t * p_tag_type)
{
	char status;
	rt_uint16_t len;
	rt_uint8_t com_mf522_buff[MAXRLEN];
	rt_uint8_t cmd;
	
	clear_bit_mask(STATUS2_REG, 0x08);
	
	cmd = 0x07;
	write_raw_rc(BIT_FRAMING_REG, &cmd);
	
	set_bit_mask(TX_CONTROL_REG, 0x03);
	
	com_mf522_buff[0] = reg_code;
	
	status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buff, 1, com_mf522_buff, &len);
	
	if ((status == MI_OK) && (len == 0x10))
	{
		*p_tag_type       = com_mf522_buff[0];
		*(p_tag_type + 1) = com_mf522_buff[1];
	}
	else
	{
		status = MI_ERR;
	}
	
	return status;
}

/**
 * @func    pcd_anticoll
 * @brief   ����ײ
 * @param   p_snr ��Ƭ���кţ�4�ֽ�
 * @retval  �ɹ�����MI_OK
 */
static char pcd_anticoll(rt_uint8_t * p_snr)
{
    char status;
    rt_uint8_t i, snr_check = 0;
    rt_uint16_t  len;
    rt_uint8_t com_mf522_buf[MAXRLEN]; 
    rt_uint8_t cmd;

    clear_bit_mask(STATUS2_REG, 0x08);
	
	cmd = 0x00;
    write_raw_rc(BIT_FRAMING_REG, &cmd);
	
    clear_bit_mask(COLL_REG, 0x80);
 
    com_mf522_buf[0] = PICC_ANTICOLL1;
    com_mf522_buf[1] = 0x20;

    status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, 2, com_mf522_buf, &len);

    if (status == MI_OK)
    {
    	 for (i = 0; i < 4; i++)
         {   
             *(p_snr + i)  = com_mf522_buf[i];
             snr_check ^= com_mf522_buf[i];
         }
         if (snr_check != com_mf522_buf[i])
         {   
			status = MI_ERR;
		 }
    }
    
    set_bit_mask(COLL_REG, 0x80);
	
    return status;
}

/**
 * @func    pcd_select
 * @brief   ѡ����Ƭ
 * @param   p_snr ��Ƭ���кţ�4�ֽ�
 * @retval  status �ɹ�����MI_OK�����򷵻�MI_ERR
 */
static char pcd_select(const rt_uint8_t * p_snr)
{
    char status;
    rt_uint8_t i;
    rt_uint16_t  len;
    rt_uint8_t com_mf522_buf[MAXRLEN]; 
    
    com_mf522_buf[0] = PICC_ANTICOLL1;
    com_mf522_buf[1] = 0x70;
    com_mf522_buf[6] = 0;
    for (i = 0; i < 4; i++)
    {
    	com_mf522_buf[i+2] = *(p_snr + i);
    	com_mf522_buf[6]  ^= *(p_snr + i);
    }
    calulate_crc(com_mf522_buf, 7, &com_mf522_buf[7]);
  
    clear_bit_mask(STATUS2_REG, 0x08);

    status = pcd_com_mf522(PCD_TRANSCEIVE, com_mf522_buf, 9, com_mf522_buf, &len);
    
    if ((status == MI_OK) && (len == 0x18))
    {   
		status = MI_OK;
	}
    else
    {
		status = MI_ERR;
    }

    return status;
}

/**
 * @func    pcd_auth_state
 * @brief 	��֤��Ƭ����
 * @param	auth_mode ������֤ģʽ
            0x60 = ��֤A��Կ
            0x61 = ��֤B��Կ 
 * @param	addr ���ַ
 * @param	p_key ����
 * @param	p_snr ��Ƭ���кţ�4�ֽ�
 * @retval	�ɹ�����MI_OK�����򷵻�MI_ERR
 */
static char pcd_auth_state(rt_uint8_t auth_mode, 
	                       rt_uint8_t addr, 
                           const rt_uint8_t * p_key,
                           const rt_uint8_t * p_snr)
{
    char status;
    rt_uint16_t  len;
    rt_uint8_t i, com_mf522_buf[MAXRLEN]; 
	rt_uint8_t tmp;
    com_mf522_buf[0] = auth_mode;
    com_mf522_buf[1] = addr;
	
	/* װ������ */
    for (i = 0; i < 6; i++)
    {    
		com_mf522_buf[i + 2] = *(p_key + i);
	}
    
	/* װ�ؿ���ID */
    for (i=0; i<6; i++)
    {    
		com_mf522_buf[i + 8] = *(p_snr + i);
	}
	
	/* ����ͨ��У������ */
    status = pcd_com_mf522(PCD_AUTHENT, com_mf522_buf, 12, com_mf522_buf, &len);
	
	read_raw_rc(STATUS2_REG, &tmp);
	
    if ((status != MI_OK) || (!(tmp & 0x08)))
    {   
		status = MI_ERR;
	}
		
    return status;
}

/**
 * @func    rc522_reset
 * @brief   ��λRC522
 * @retval  �ɹ�����MI_OK
 */
static rt_err_t rc522_reset(void)
{
	rt_uint8_t cmd;
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	rt_thread_delay(10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	rt_thread_delay(10);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	
    /* ����֮ǰ�ϻ����� */
	rfid_lock(&spi_rfid_device);
	
    /* д��ʼ������ */
	cmd = PCD_RESETPHASE;
	write_raw_rc(COMMAND_REG, &cmd);
	
	rt_thread_delay(1);
	
	cmd = 0x3D;
	write_raw_rc(MODE_REG, &cmd);
	
	cmd = 30;
	write_raw_rc(T_RELOAD_REG_L, &cmd);
	
	cmd = 0;
	write_raw_rc(T_RELOAD_REG_H, &cmd);
	
	cmd = 0x8D;
	write_raw_rc(T_MODE_REG, &cmd);
	
	cmd = 0x3E;
	write_raw_rc(T_PRESCALER_REG, &cmd);
	
	cmd = 0x40;
	write_raw_rc(TX_AUTO_REG, &cmd);
	
	set_bit_mask(TEST_PIN_EN_REG, 0x80);
	
	cmd = 0x40;
	write_raw_rc(TX_AUTO_REG, &cmd);
	
	pcd_antenna_off();

	rfid_unlock(&spi_rfid_device);
	
	rt_thread_delay(200);
	
	rfid_lock(&spi_rfid_device);
	
	pcd_antenna_on();

    /* ������ɺ�⿪������ */
	rfid_unlock(&spi_rfid_device);
	
	return RT_EOK;
}

/**
 * @func    rc522_write
 * @brief   д���ݵ�M1��һ��
 * @param   dev �豸������
 * @param   address ���ַ
 * @param   buffer д�������
 * @param   size Ҫд������ݵĴ�С
 * @retval	size ���ݵĴ�С
 */ 
static rt_size_t rc522_write(rt_device_t dev,
                             rt_off_t address,
                             const void * buffer,
                             rt_size_t size)
{
	const uint8_t * ptr = buffer;
	rt_uint8_t err;
	rfid_lock((struct spi_rfid_device *)dev);
	
	err = pcd_write(address, ptr, size);
	if (err != MI_OK)
	{
		return False;
	}
	
	rfid_unlock((struct spi_rfid_device *)dev);
	
	return size;
}

/**
 * @func    rc522_read
 * @brief   ��ȡM1��һ������
 * @param   dev �豸������
 * @param   address ���ַ
 * @param   buffer ����������
 * @param   size ���ݵĴ�С			
 * @retval	size ���ݵĴ�С
 */
static rt_size_t rc522_read(rt_device_t dev,
                            rt_off_t address,
                            void * buffer,
                            rt_size_t size)
{
	rt_uint8_t err;
	rfid_lock((struct spi_rfid_device *)dev);
	
	err = pcd_read(address, buffer, size);
	
	if (err != MI_OK)
	{
		return False;
	}
	
	rfid_unlock((struct spi_rfid_device *)dev);
	
	return size;
}

/**
 * @func    rc522_init
 * @brief   ��ʼ��rc522
 * @param   dev �豸������		
 * @retval  ��
 */
static rt_err_t rc522_init(rt_device_t dev)
{
	if (rc522_reset() == RT_EOK)
	{
#ifdef RC522_DEBUG
		rt_kprintf("RC522��ʼ����ɣ�\r\n");
#endif
	}
    
	return RT_EOK;
}

/**
 * @func    rc522_open
 * @brief   ��rc522�豸
 * @param   dev �����Ķ��������
 * @param   oflag �����ı�־
 * @note    ��
 * @retval  ������� RT_EOK �����ɹ�
 */
static rt_err_t rc522_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

/**
 * @func    rc522_close
 * @brief   �ر�rc522�豸
 * @param   dev �����Ķ��������
 * @note    ��
 * @retval  ������� RT_EOK �����ɹ�
 */
static rt_err_t rc522_close(rt_device_t dev)
{
	return RT_EOK;
}

/**
 * @func    rc522_control
 * @brief   ����rc522�豸
 * @param   dev �����Ķ��������
 * @param   cmd ���Ʋ���������
 * @param   args �������û�����
 * @note    ��
 * @retval  ������� RT_EOK �����ɹ����������ʧ��
 */
static rt_err_t rc522_control(rt_device_t dev, int cmd, void * args)
{
	rt_uint8_t status = MI_ERR, *tmp_point = RT_NULL; 
	rt_err_t err = -RT_ERROR;
	
    RT_ASSERT(dev != RT_NULL);
	
	rfid_lock((struct spi_rfid_device *)dev);
	
	if (cmd == RC522_ANTENNA_ON_CMD)/* ������ */
	{
		pcd_antenna_on();
		err = RT_EOK;
	}
	else if (cmd == RC522_ANTENNA_OFF_CMD) /* ������ */
	{
		pcd_antenna_off();
		err = RT_EOK;
	}
	else if (cmd == RC522_REQUEST_CMD)  /*Ѱ�� */
	{
		tmp_point = (rt_uint8_t *)args;
		status = pcd_request(tmp_point[0], args);
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	else if (cmd == RC522_ANTICOLL_CMD) /* ����ײ */
	{
		status = pcd_anticoll(args);
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	else if (cmd == RC522_SELECT_CMD)   /* ѡ����Ƭ */
	{
		status = pcd_select(args);
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	else if (cmd == RC522_RESET_CMD)    /* ��λ���� */
	{
		status = rc522_reset();
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	else if (cmd == RC522_AUTH_STATE_CMD)   /* ��֤���� */
	{
		tmp_point = (rt_uint8_t *)args;
		status = pcd_auth_state(tmp_point[0], tmp_point[1], tmp_point + 2, tmp_point + 8);
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	else if (cmd == RC522_HALT_CMD) /* ����������� */
	{
		status = pcd_halt();
		if (status == MI_OK)
		{
			err = RT_EOK;
		}
	}
	
	rfid_unlock((struct spi_rfid_device *)dev);
	
	return err;
}

/**
 * @func	rc522_init_config
 * @brief 	rc522��ʼ������
 * @param	rfid_device_name �豸������
 * @param	spi_device_name SPI�����豸���������
			�ڴ�rc522�ǹ�����spi1����
 * @note  	��
 * @retval	��
 */
rt_err_t rc522_init_config(const char * rfid_device_name, const char * spi_device_name)
{
    struct rt_spi_device * rt_spi_device;
	
    /* ��ʼ�������ź��� */
    if (rt_mutex_init(&spi_rfid_device.lock, spi_device_name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
#ifdef RC522_DEBUG
        rt_kprintf("init rfid lock mutex failed\n");
#endif
        return -RT_ENOSYS;
    }

	/* Ѱ��ָ�������ƵĶ��� */
    rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(rt_spi_device == RT_NULL)
    {
#ifdef RC522_DEBUG
        rt_kprintf("spi device %s not found!\r\n", spi_device_name);
#endif
        return -RT_ENOSYS;
    }
	
    spi_rfid_device.rt_spi_device = rt_spi_device;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8; /*���ݿ����8bit */
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 10 * 1000 * 1000; /* 10M */
        rt_spi_configure(spi_rfid_device.rt_spi_device, &cfg);
    }

    /* init rfid */
    {
		rc522_gpio_config();
		rc522_reset();
    }

    /* ע���豸�������� */
    spi_rfid_device.rfid_device.type    = RT_Device_Class_SPIDevice;
    spi_rfid_device.rfid_device.init    = rc522_init;
    spi_rfid_device.rfid_device.open    = rc522_open;
    spi_rfid_device.rfid_device.close   = rc522_close;
    spi_rfid_device.rfid_device.read    = rc522_read;
    spi_rfid_device.rfid_device.write   = rc522_write;
    spi_rfid_device.rfid_device.control = rc522_control;
	
    /* û��˽������ */
    spi_rfid_device.rfid_device.user_data = RT_NULL;

	/* ע���豸 */
    rt_device_register(&spi_rfid_device.rfid_device, rfid_device_name,
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

#ifdef RC522_DEBUG
    rt_kprintf("rc522 device in %s bus init successful!\r\n", spi_device_name);    
#endif
    return RT_EOK;
}







