/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        queue.h
 * @author          �Ž�����С��
 * @version         V1.0.1
 * @date            2018-02-28
 * @Description     queueͷ�ļ����ڴ��ļ���ʵ�ֶ��ж��⿪�ŵ�API�ӿڡ�
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-04     ZSY       V1.0.0          first version.
 * 2018-02-28     ZSY       V1.0.1          ������Ƕ�뵽ϵͳ����.
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _Queue_H_
#define _Queue_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rtthread.h"

/* Public macro Definition ---------------------------------------------------*/
#define MAXQUEUE 80
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
typedef struct _queue_node
{
	void * qitem;
	struct _queue_node * next;
}queue_node_t;

typedef struct _queue
{
	queue_node_t * front;
	queue_node_t * rear;
	int count;
}queue_t;

/* Member method APIs --------------------------------------------------------*/
/*��ʼ������*/
rt_err_t queue_init(queue_t * pq);
/*�ж϶����Ƿ�Ϊ��*/
rt_err_t queue_is_full(const queue_t * pq);
/*�ж϶����Ƿ�Ϊ��*/
rt_err_t queue_is_empty(const queue_t * pq);
/*��ȡ���е�Ԫ�صĸ���*/
rt_uint16_t queue_item_count(const queue_t * pq);
/*����*/
rt_err_t enqueue(queue_t * pq, void * qitem);
/*����*/
void * dequeue(queue_t * pq);
/* End Member Method APIs ---------------------------------------------------*/
/* UserCode end -------------------------------------------------------------*/
	
#endif
