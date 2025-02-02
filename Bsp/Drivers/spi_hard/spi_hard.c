#include "spi_hard.h"
#include "board.h"
#include <rtdevice.h>

/* SPI1 GPIO define. */
#define SPI1_GPIO_CLK       	GPIO_Pin_3
#define SPI1_CLK_PIN_SOURCE 	GPIO_PinSource3
#define SPI1_GPIO_MISO       	GPIO_Pin_4
#define SPI1_MISO_PIN_SOURCE 	GPIO_PinSource4
#define SPI1_GPIO_MOSI       	GPIO_Pin_5
#define SPI1_MOSI_PIN_SOURCE 	GPIO_PinSource5
#define SPI1_GPIO_PORT          GPIOB
#define SPI1_GPIO_RCC      		RCC_AHB1Periph_GPIOB
#define RCC_APBPeriph_SPI1 		RCC_APB2Periph_SPI1

/*	SPI2 GPIO define	*/
#define SPI2_GPIO_CLK       	GPIO_Pin_13
#define SPI2_CLK_PIN_SOURCE 	GPIO_PinSource13
#define SPI2_GPIO_MISO       	GPIO_Pin_14
#define SPI2_MISO_PIN_SOURCE 	GPIO_PinSource14
#define SPI2_GPIO_MOSI       	GPIO_Pin_15
#define SPI2_MOSI_PIN_SOURCE 	GPIO_PinSource15
#define SPI2_GPIO_PORT          GPIOB
#define SPI2_GPIO_RCC      		RCC_AHB1Periph_GPIOB
#define RCC_APBPeriph_SPI2 		RCC_APB1Periph_SPI2

/*	SPI3 GPIO define	*/
#define SPI3_GPIO_CLK       	GPIO_Pin_3
#define SPI3_CLK_PIN_SOURCE 	GPIO_PinSource3
#define SPI3_GPIO_MISO       	GPIO_Pin_4
#define SPI3_MISO_PIN_SOURCE 	GPIO_PinSource4
#define SPI3_GPIO_MOSI       	GPIO_Pin_5
#define SPI3_MOSI_PIN_SOURCE 	GPIO_PinSource5
#define SPI3_GPIO_PORT          GPIOB
#define SPI3_GPIO_RCC      		RCC_AHB1Periph_GPIOB
#define RCC_APBPeriph_SPI3 		RCC_APB1Periph_SPI3

/*	SPI3 GPIO define	*/
#define SPI5_GPIO_CLK       	GPIO_Pin_7
#define SPI5_CLK_PIN_SOURCE 	GPIO_PinSource7
#define SPI5_GPIO_MISO       	GPIO_Pin_8
#define SPI5_MISO_PIN_SOURCE 	GPIO_PinSource8
#define SPI5_GPIO_MOSI       	GPIO_Pin_9
#define SPI5_MOSI_PIN_SOURCE 	GPIO_PinSource9
#define SPI5_GPIO_PORT          GPIOF
#define SPI5_GPIO_RCC      		RCC_AHB1Periph_GPIOF
#define RCC_APBPeriph_SPI5 		RCC_APB2Periph_SPI5

#define W25QXX_GPIO_CS			GPIO_Pin_12
#define W25QXX_CS_PORT			GPIOB
#define W25QXX_GPIO_RCC      	RCC_AHB1Periph_GPIOB

#define RC522_GPIO_CS			GPIO_Pin_15
#define RC522_CS_PORT			GPIOG
#define RC522_GPIO_RCC      	RCC_AHB1Periph_GPIOG

static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration);
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message);

static struct rt_spi_ops stm32_spi_ops =
{
    configure,
    xfer
};

/*				USE DMA				*/
#ifdef SPI_USE_DMA
static uint8_t dummy = 0xFF;
#endif /*		SPI_USE_DMA		*/

#ifdef SPI_USE_DMA
static void DMA_Configuration(struct stm32_spi_bus * stm32_spi_bus, const void * send_addr, void * recv_addr, rt_size_t size)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	if(!stm32_spi_bus->dma)
	{
		return ;
	}
	
	DMA_ClearFlag(stm32_spi_bus->dma->priv_data->DMA_Channel_RX_FLAG_TC
                  | stm32_spi_bus->dma->priv_data->DMA_Channel_RX_FLAG_TE
                  | stm32_spi_bus->dma->priv_data->DMA_Channel_TX_FLAG_TC
                  | stm32_spi_bus->dma->priv_data->DMA_Channel_TX_FLAG_TE);
	    /* RX channel configuration */
	DMA_Cmd(stm32_spi_bus->dma->priv_data->DMA_Channel_RX, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_BufferSize = size;

	if(recv_addr != RT_NULL)
	{
			DMA_InitStructure.DMA_MemoryBaseAddr = (u32) recv_addr;
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	}
	else
	{
			DMA_InitStructure.DMA_MemoryBaseAddr = (u32) (&dummy);
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	}

	DMA_Init(stm32_spi_bus->dma->priv_data->DMA_Channel_RX, &DMA_InitStructure);

	DMA_ITConfig(stm32_spi_bus->dma->priv_data->DMA_Channel_RX, DMA_IT_TC, ENABLE);
	DMA_Cmd(stm32_spi_bus->dma->priv_data->DMA_Channel_RX, ENABLE);

	/* TX channel configuration */
	DMA_Cmd(stm32_spi_bus->dma->priv_data->DMA_Channel_TX, DISABLE);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_BufferSize = size;

	if(send_addr != RT_NULL)
	{
			DMA_InitStructure.DMA_MemoryBaseAddr = (u32)send_addr;
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	}
	else
	{
			DMA_InitStructure.DMA_MemoryBaseAddr = (u32)(&dummy);;
			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	}

	DMA_Init(stm32_spi_bus->dma->priv_data->DMA_Channel_TX, &DMA_InitStructure);

	DMA_ITConfig(stm32_spi_bus->dma->priv_data->DMA_Channel_TX, DMA_IT_TC, ENABLE);
	DMA_Cmd(stm32_spi_bus->dma->priv_data->DMA_Channel_TX, ENABLE);
}

#ifdef SPI1_USING_DMA
static const struct stm32_spi_dma_private dma1_pric = 
{
	DMA1_Channel3,
	DMA1_Channel2,
	DMA1_FLAG_TC3,
	DMA1_FLAG_TE3,
	DMA1_FLAG_TC2,
	DMA1_FLAG_TE2,
	DMA1_Channel3_IRQn,
	DMA1_Channel2_IRQn,
	DMA1_FLAG_GL3,
	DMA1_FLAG_GL2,
};

static struct stm32_spi_dma dmal = 
{
	&dma1_priv
};

void DMA1_Channel2_IRQHandler(void) 
{
	/* enter interrupt */
	rt_interrupt_enter();
	rt_event_send(&dma1.event, SPI_DMA_TX_DONE);
	DMA_ClearFlag(dma1.priv_data->tx_gl_flag);
	/* leave interrupt */
	rt_interrupt_leave();
}
void DMA1_Channel3_IRQHandler(void) 
{
	/* enter interrupt */
	rt_interrupt_enter();
	rt_event_send(&dma1.event, SPI_DMA_RX_DONE);
	DMA_ClearFlag(dma1.priv_data->rx_gl_flag);
	/* leave interrupt */
	rt_interrupt_leave();
}
#endif	/*	SPI1_USING_DMA	*/

#ifdef SPI2_USEING_DMA

static const struct stm32_spi_dma_private dma2_priv =
{
    DMA1_Channel5,
    DMA1_Channel5,
    DMA1_FLAG_TC5,
    DMA1_FLAG_TE5,
    DMA1_FLAG_TC4,
    DMA1_FLAG_TE4,
    DMA1_Channel5_IRQn,
    DMA1_Channel4_IRQn,
    DMA1_FLAG_GL5,
    DMA1_FLAG_GL4,
};
static struct stm32_spi_dma dma2 =
{
    &dma2_priv,
};
void DMA1_Channel4_IRQHandler(void) {
    /* enter interrupt */
    rt_interrupt_enter();
    rt_event_send(&dma2.event, SPI_DMA_TX_DONE);
    DMA_ClearFlag(dma2.priv_data->tx_gl_flag);
    /* leave interrupt */
    rt_interrupt_leave();
}
void DMA1_Channel5_IRQHandler(void) {
    /* enter interrupt */
    rt_interrupt_enter();
    rt_event_send(&dma2.event, SPI_DMA_RX_DONE);
    DMA_ClearFlag(dma2.priv_data->rx_gl_flag);
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /*SPI2_USING_DMA*/
#ifdef SPI3_USING_DMA
static const struct stm32_spi_dma_private dma3_priv =
{
    DMA2_Channel2,
    DMA2_Channel1,
    DMA2_FLAG_TC2,
    DMA2_FLAG_TE2,
    DMA2_FLAG_TC1,
    DMA2_FLAG_TE1,
    DMA2_Channel2_IRQn,
    DMA2_Channel1_IRQn,
    DMA2_FLAG_GL2,
    DMA2_FLAG_GL1,
};
static struct stm32_spi_dma dma3 =
{
    &dma3_priv,
};
void DMA2_Channel1_IRQHandler(void) 
{
    /* enter interrupt */
    rt_interrupt_enter();
    rt_event_send(&dma3.event, SPI_DMA_TX_DONE);
    DMA_ClearFlag(dma3.priv_data->tx_gl_flag);
    /* leave interrupt */
    rt_interrupt_leave();
}
void DMA2_Channel2_IRQHandler(void) 
{
    /* enter interrupt */
    rt_interrupt_enter();
    rt_event_send(&dma3.event, SPI_DMA_RX_DONE);
    DMA_ClearFlag(dma3.priv_data->rx_gl_flag);
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /*SPI3_USING_DMA*/
#endif /*SPI_USE_DMA*/

rt_inline uint16_t get_spi_BaudRatePrescaler(rt_uint32_t max_hz)
{
    uint16_t SPI_BaudRatePrescaler;

    /* STM32F40x SPI MAX 42Mhz  SPI1 max 84MHz*/
    if(max_hz >= SystemCoreClock/2 && SystemCoreClock/2 <= 36000000)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    }
    else if(max_hz >= SystemCoreClock/4)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }
    else if(max_hz >= SystemCoreClock/8)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
    else if(max_hz >= SystemCoreClock/16)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
    else if(max_hz >= SystemCoreClock/32)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    }
    else if(max_hz >= SystemCoreClock/64)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    }
    else if(max_hz >= SystemCoreClock/128)
    {
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
    }
    else
    {
        /* min prescaler 256 */
        SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }

    return SPI_BaudRatePrescaler;
}


static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    SPI_InitTypeDef SPI_InitStructure;

    SPI_StructInit(&SPI_InitStructure);

    /* data_width */
    if(configuration->data_width <= 8)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    }
    else if(configuration->data_width <= 16)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        return RT_EIO;
    }
    /* baudrate */
    SPI_InitStructure.SPI_BaudRatePrescaler = get_spi_BaudRatePrescaler(configuration->max_hz);
    /* CPOL */
    if(configuration->mode & RT_SPI_CPOL)
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    }
    else
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    }
    /* CPHA */
    if(configuration->mode & RT_SPI_CPHA)
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    }
    else
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    }
    /* MSB or LSB */
    if(configuration->mode & RT_SPI_MSB)
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    }
    else
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;

    /* init SPI */
    SPI_I2S_DeInit(stm32_spi_bus->SPI);
    SPI_Init(stm32_spi_bus->SPI, &SPI_InitStructure);
    /* Enable SPI_MASTER */
    SPI_Cmd(stm32_spi_bus->SPI, ENABLE);
    SPI_CalculateCRC(stm32_spi_bus->SPI, DISABLE);

    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    struct rt_spi_configuration * config = &device->config;
    SPI_TypeDef * SPI = stm32_spi_bus->SPI;
    struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;
    rt_uint32_t size = message->length;

    /* take CS */
    if(message->cs_take && stm32_spi_cs)
    {
        GPIO_ResetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }
#ifdef SPI_USE_DMA
    if(
       (stm32_spi_bus->parent.parent.flag & (RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX)) &&
        stm32_spi_bus->dma &&
        message->length > 32)
    {
        if(config->data_width <= 8)
        {
            rt_uint32_t ev = 0;
            DMA_Configuration(stm32_spi_bus, message->send_buf, message->recv_buf, message->length);
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
            rt_event_recv(&stm32_spi_bus->dma->event, SPI_DMA_COMPLETE,
                    RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &ev);
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);
            DMA_ITConfig(stm32_spi_bus->dma->priv_data->DMA_Channel_TX, DMA_IT_TC, DISABLE);
            DMA_ITConfig(stm32_spi_bus->dma->priv_data->DMA_Channel_RX, DMA_IT_TC, DISABLE);
        }
    }
    else
#endif /*SPI_USE_DMA*/
    {
        if(config->data_width <= 8)
        {
            const rt_uint8_t * send_ptr = message->send_buf;
            rt_uint8_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint8_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }

                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);

                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
        else if(config->data_width <= 16)
        {
            const rt_uint16_t * send_ptr = message->send_buf;
            rt_uint16_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint16_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }

                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);

                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
    }

    /* release CS */
    if(message->cs_release && stm32_spi_cs)
    {
        GPIO_SetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }

    return message->length;
};

/** \brief init and register stm32 spi bus.
 *
 * \param SPI: STM32 SPI, e.g: SPI1,SPI2,SPI3.
 * \param stm32_spi: stm32 spi bus struct.
 * \param spi_bus_name: spi bus name, e.g: "spi1"
 * \return
 *
 */
rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
                            struct stm32_spi_bus * stm32_spi,
                            const char * spi_bus_name)
{
    rt_err_t res = RT_EOK;
#ifdef SPI_USE_DMA
    NVIC_InitTypeDef NVIC_InitStructure;
#endif
    rt_uint32_t flags = 0;
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    if(SPI == SPI1)
    {
    	stm32_spi->SPI = SPI1;
			
		/*	开启SPI1的时钟	max 84MHz*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
#ifdef SPI_USE_DMA
#ifdef SPI1_USING_DMA
        {
            rt_event_init(&dma1.event, "spi1ev", RT_IPC_FLAG_FIFO);
            stm32_spi->dma = &dma1;
            /* rx dma interrupt config */
            NVIC_InitStructure.NVIC_IRQChannel = dma1.priv_data->tx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            NVIC_InitStructure.NVIC_IRQChannel = dma1.priv_data->rx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            /* Enable the DMA1 Clock */
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
            flags |= RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
        }
#else /*!SPI1_USING_DMA*/
        stm32_spi->dma = RT_NULL;
#endif /*SPI1_USING_DMA*/
#endif /*SPI_USE_DMA*/
    }
    else if(SPI == SPI2)
    {
			stm32_spi->SPI = SPI2;
			
			/*	开启SPI2时钟	max 42MHz	*/
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#ifdef SPI_USE_DMA
#ifdef SPI2_USING_DMA
        {
            rt_event_init(&dma2.event, "spi2ev", RT_IPC_FLAG_FIFO);
            stm32_spi->dma = &dma2;
            /* rx dma interrupt config */
            NVIC_InitStructure.NVIC_IRQChannel = dma2.priv_data->tx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            NVIC_InitStructure.NVIC_IRQChannel = dma2.priv_data->rx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            /* Enable the DMA1 Clock */
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
            flags |= RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
        }
#else /*!SPI2_USING_DMA*/
        stm32_spi->dma = RT_NULL;
#endif /*SPI2_USING_DMA*/
#endif /*SPI_USE_DMA*/
    }
    else if(SPI == SPI3)
    {
    	stm32_spi->SPI = SPI3;
			
			/*	开启SPI3时钟	max 42MHz*/
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
#ifdef SPI_USE_DMA
#ifdef SPI3_USING_DMA
        {
            rt_event_init(&dma3.event, "spi3ev", RT_IPC_FLAG_FIFO);
            stm32_spi->dma = &dma3;
            /* rx dma interrupt config */
            NVIC_InitStructure.NVIC_IRQChannel = dma3.priv_data->tx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            NVIC_InitStructure.NVIC_IRQChannel = dma3.priv_data->rx_irq_ch;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            /* Enable the DMA1 Clock */
            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
            flags |= RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
        }
#else /*!SPI3_USING_DMA*/
        stm32_spi->dma = RT_NULL;
#endif /*SPI3_USING_DMA*/
#endif /*SPI_USE_DMA*/
    }
	else if(SPI == SPI5)
	{
		stm32_spi->SPI = SPI5;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);
#ifdef SPI_USE_DMA
#ifdef SPI5_USING_DMA
		{
			rt_event_init(&dma5.event, "spi5ev", RT_IPC_FLAG_FIFO);
			stm32_spi->dma = &dma5;
			/* rx dma interrupt config */
			NVIC_InitStructure.NVIC_IRQChannel = dma5.priv_data->tx_irq_ch;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			NVIC_InitStructure.NVIC_IRQChannel = dma5.priv_data->rx_irq_ch;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			/* Enable the DMA1 Clock */
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
			flags |= RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX;
		}
#else	/*!SPI5_USING_DMA*/
		stm32_spi->DMA1 = RT_NULL;
#endif	/*SPI5_USING_DMA*/
#endif	/*SPI_USE_DMA*/
	}
    else
    {
        return RT_ENOSYS;
    }
    res = rt_spi_bus_register(&stm32_spi->parent, spi_bus_name, &stm32_spi_ops);
    stm32_spi->parent.parent.flag |= flags;

    return res;
}

int rt_hw_spi_init(void)
{
#ifdef RT_USING_SPI1
    /* register SPI bus */
    /* SPI1 configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO Periph clock */
        RCC_AHB1PeriphClockCmd(SPI1_GPIO_RCC , ENABLE);
        
        GPIO_PinAFConfig(SPI1_GPIO_PORT, SPI1_CLK_PIN_SOURCE, GPIO_AF_SPI1);
        GPIO_PinAFConfig(SPI1_GPIO_PORT, SPI1_MISO_PIN_SOURCE, GPIO_AF_SPI1);
        GPIO_PinAFConfig(SPI1_GPIO_PORT, SPI1_MOSI_PIN_SOURCE, GPIO_AF_SPI1);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 

        /* Configure SPI1 pins */
        GPIO_InitStructure.GPIO_Pin = SPI1_GPIO_CLK;
        GPIO_Init(SPI1_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI1_GPIO_MISO;
        GPIO_Init(SPI1_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI1_GPIO_MOSI;
        GPIO_Init(SPI1_GPIO_PORT, &GPIO_InitStructure);
    } /* SPI1 configuration */

    /* register SPI1 to stm32_spi_bus */
    {
        static struct stm32_spi_bus stm32_spi1;             //it must be add static
        
        stm32_spi_register(SPI1, &stm32_spi1, "spi1"); 
    }
	
	/* attach spi10 */
    {
        static struct rt_spi_device spi_device;    //it must be add static
        static struct stm32_spi_cs  spi_cs;     //it must be add static
        
        spi_cs.GPIOx    = RC522_CS_PORT;
        spi_cs.GPIO_Pin = RC522_GPIO_CS;
        
        RCC_AHB1PeriphClockCmd(RC522_GPIO_RCC, ENABLE);

		GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Pin   = spi_cs.GPIO_Pin;
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        
        rt_spi_bus_attach_device(&spi_device, "RFID", "spi1", (void*)&spi_cs);//set spi_device->bus
    } /* attach spi10 */   
#endif /* RT_USING_SPI1 */

#ifdef RT_USING_SPI2
    /* register SPI bus */
    /* SPI2 configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO Periph clock */
        RCC_AHB1PeriphClockCmd(SPI2_GPIO_RCC , ENABLE);
        
        GPIO_PinAFConfig(SPI2_GPIO_PORT, SPI2_CLK_PIN_SOURCE, GPIO_AF_SPI2);
        GPIO_PinAFConfig(SPI2_GPIO_PORT, SPI2_MISO_PIN_SOURCE, GPIO_AF_SPI2);
        GPIO_PinAFConfig(SPI2_GPIO_PORT, SPI2_MOSI_PIN_SOURCE, GPIO_AF_SPI2);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 

        /* Configure SPI2 pins */
        GPIO_InitStructure.GPIO_Pin = SPI2_GPIO_CLK;
        GPIO_Init(SPI2_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI2_GPIO_MISO;
        GPIO_Init(SPI2_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI2_GPIO_MOSI;
        GPIO_Init(SPI2_GPIO_PORT, &GPIO_InitStructure);
    } /* SPI2 configuration */

    /* register SPI2 to stm32_spi_bus */
    {
        static struct stm32_spi_bus stm32_spi2;             //it must be add static
        
        /* register SPI2 to stm32_spi_bus */
        stm32_spi_register(SPI2, &stm32_spi2, "spi2"); 
    }
	
	/* attach spi20 */
    {
        static struct rt_spi_device spi_device;    //it must be add static
        static struct stm32_spi_cs  spi_cs;     //it must be add static
        
        spi_cs.GPIOx    = W25QXX_CS_PORT;
        spi_cs.GPIO_Pin = W25QXX_GPIO_CS;
        
        RCC_AHB1PeriphClockCmd(W25QXX_GPIO_RCC, ENABLE);

		GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Pin   = W25QXX_GPIO_CS;
        GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStructure);
        GPIO_SetBits(W25QXX_CS_PORT, W25QXX_GPIO_CS);
        
        rt_spi_bus_attach_device(&spi_device, "FLASH", "spi2", (void*)&spi_cs);//set spi_device->bus
    } /* attach spi10 */   
#endif /* RT_USING_SPI2 */
	
#ifdef RT_USING_SPI5
    /* SPI1 configure */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO Periph clock */
        RCC_AHB1PeriphClockCmd(SPI5_GPIO_RCC , ENABLE);
        
        GPIO_PinAFConfig(SPI5_GPIO_PORT, SPI5_CLK_PIN_SOURCE, GPIO_AF_SPI5);
        GPIO_PinAFConfig(SPI5_GPIO_PORT, SPI5_MISO_PIN_SOURCE, GPIO_AF_SPI5);
        GPIO_PinAFConfig(SPI5_GPIO_PORT, SPI5_MOSI_PIN_SOURCE, GPIO_AF_SPI5);

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 

        /* Configure SPI1 pins */
        GPIO_InitStructure.GPIO_Pin = SPI5_GPIO_CLK;
        GPIO_Init(SPI5_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI5_GPIO_MISO;
        GPIO_Init(SPI5_GPIO_PORT, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = SPI5_GPIO_MOSI;
        GPIO_Init(SPI5_GPIO_PORT, &GPIO_InitStructure);
    } /* SPI1 configuration */

    {
        static struct stm32_spi_bus stm32_spi5;             //it must be add static
        
        /* register SPI1 to stm32_spi_bus */
        stm32_spi_register(SPI5, &stm32_spi5, "spi5"); 
    }
	
	/* attach spi10 */
    {
        static struct rt_spi_device spi_device;    //it must be add static
        static struct stm32_spi_cs  spi_cs;     //it must be add static
        
        spi_cs.GPIOx    = W25QXX_CS_PORT;
        spi_cs.GPIO_Pin = W25QXX_GPIO_CS;
        
        RCC_AHB1PeriphClockCmd(W25QXX_GPIO_RCC, ENABLE);

		GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_InitStructure.GPIO_Pin   = W25QXX_GPIO_CS;
        GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStructure);
        GPIO_SetBits(W25QXX_CS_PORT, W25QXX_GPIO_CS);
        
		spi_device.bus = &stm32_spi.parent;
        rt_spi_bus_attach_device(&spi_device, "spi51", "spi5", (void*)&spi_cs);//set spi_device->bus
    } /* attach spi10 */   
#endif	/*	RT_USING_SPI5	*/
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_spi_init);//rt_hw_spi1_init will be called in rt_components_board_init()





