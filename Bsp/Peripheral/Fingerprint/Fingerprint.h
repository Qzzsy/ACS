#ifndef _FINGERPRINT_H_
#define _FINGERPRINT_H_

#include "stm32f4xx.h"
#include "rtthread.h"

#define FINGERPRINT_USING_UART       "uart1"

rt_err_t fingerprint_send_data(const char * data, rt_size_t size);
rt_size_t fingerprint_get_rev_status(void);
rt_err_t fingerprint_rev_data(uint8_t * data_buf, rt_size_t size);
rt_err_t fingerprint_open(void);
rt_err_t fingerprint_close(void);
rt_err_t fingerprint_control(void);

#endif
