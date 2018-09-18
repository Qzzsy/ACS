/**
******************************************************************************
 * @Copyright		(C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename		rfid_rc522.h
 * @author			�Ž�����С��
 * @version			V1.0.0
 * @date			2018-01-26
 * @Description		rfid_rc522�ļ���������rfid_rc522������һЩ���������
 * @Others
 * @History
 * Date           	Author    	version    				Notes
 * 2018-01-26    	ZSY       	V1.0.0				first version.
 * @verbatim  
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _RFID_RC522_H_
#define _RFID_RC522_H_

/* Includes ------------------------------------------------------------------*/
#include "rtthread.h"
#include "stm32f4xx.h"

/* Public macro Definition ---------------------------------------------------*/
/* ��MF522ͨѶʱ���صĴ������궨��*/
#define MI_OK                          0
#define MI_NOTAGERR                    1 //(-1)
#define MI_ERR                         2 //(-2)

#define MAXRLEN                       18

/* MF522������*/
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ
#define PCD_RECEIVE           0x08               //��������
#define PCD_TRANSMIT          0x04               //��������
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λ
#define PCD_CALCCRC           0x03               //CRC����

/* Mifare_One��Ƭ������*/
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����


/* MF522 FIFO���ȶ���*/
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

#define RC522_ANTENNA_ON_CMD             0x01
#define RC522_ANTENNA_OFF_CMD            0x02
#define RC522_RESET_CMD                  0x03
#define RC522_REQUEST_CMD                0x04
#define RC522_AUTH_STATE_CMD             0x05
#define RC522_SELECT_CMD                 0x06
#define RC522_ANTICOLL_CMD               0x07
#define RC522_HALT_CMD                   0X08

/* End public macro Definition -----------------------------------------------*/
/* UserCode start ------------------------------------------------------------*/

struct spi_rfid_device
{
    struct rt_device                rfid_device;
    struct rt_spi_device *          rt_spi_device;
    struct rt_mutex                 lock;
    void *                          user_data;
};

typedef struct spi_rfid_device *rt_spi_rfid_device_t;

/* Member method APIs --------------------------------------------------------*/

rt_err_t rc522_init_config(const char * rfid_device_name, const char * spi_device_name);

/* End Member Method APIs ----------------------------------------------------*/

#endif
