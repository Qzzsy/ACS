/**
 ******************************************************************************
 * @file        w25qxx.c
 * @author      门禁开发小组
 * @version     V1.0.4
 * @date        2018-05-20
 * @brief       这个文件是w25qxx的访问方法，实现了w25qxx的基本操作，并且提供安全
                的访问，支持打开设备，关闭设备，控制设备，读写设备，初始化设备
 * @note        使用前需要调用w25qxx_init();  否则无法使用此方法
 * @History
 * Date           	Author    	version    		Notes
 * 2017-12-19        ZSY         V1.0.0      first version.
 * 2017-12-20        ZSY         V1.0.1      修复写入任意地址和长度数据的数据错误的BUG,提高执行速率
 * 2017-12-21        ZSY         V1.0.2      添加私有宏定义，添加对4字节对齐的支持
 * 2018-01-09        ZSY         V1.0.3      增强可视化，修复在不同缩进情况下注释显示错乱的问题
 * 2018-05-20        ZSY         V1.0.4      修复写入错误的BUG
 */
	
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <rtdevice.h>

#include "spi_flash.h"
#include "spi_hard.h"
#include "w25qxx.h"
#include "delay.h"
#include "board.h"

#ifdef STM32_EXT_SDRAM
#include "sdram_fmc.h"
#endif

/* Private macro Definition --------------------------------------------------*/

//#define FLASH_DEBUG

#ifdef FLASH_DEBUG
#define FLASH_TRACE         rt_kprintf
#else
#define FLASH_TRACE(...)
#endif /* #ifdef FLASH_DEBUG */

#define FLASH_PAGE_SIZE             256
#define FLASH_SECTOR_SIZE           4096

/* JEDEC Manufacturer??s ID */
#define MF_ID           (0xEF)

#define USE_MCU_CCM

#define FLASH_BUFF_SIZE			4096

/* JEDEC Device ID: Memory type and Capacity */
#define MTC_W25Q80_BV         (0x4014) /* W25Q80BV */
#define MTC_W25Q16_BV_CL_CV   (0x4015) /* W25Q16BV W25Q16CL W25Q16CV  */
#define MTC_W25Q16_DW         (0x6015) /* W25Q16DW  */
#define MTC_W25Q32_BV         (0x4016) /* W25Q32BV */
#define MTC_W25Q32_DW         (0x6016) /* W25Q32DW */
#define MTC_W25Q64_BV_CV      (0x4017) /* W25Q64BV W25Q64CV */
#define MTC_W25Q64_DW         (0x4017) /* W25Q64DW */
#define MTC_W25Q128_BV        (0x4018) /* W25Q128BV */
#define MTC_W25Q256_FV        (0x4019) /* W25Q256FV */

/* global variable Declaration -----------------------------------------------*/
#ifdef STM32_EXT_SDRAM
#define ADDRESS_OFFSET			2 * 854 * 480 * 4 + 4096
#endif

#if defined (USE_MCU_CCM) || !defined (SPI_USE_DMA)
#pragma pack (4)
uint8_t flash_buff[FLASH_BUFF_SIZE] __attribute__((at(0x10000000))) = {0};	/* 使用内部TCM，提高速度 */
#elif defined (STM32_EXT_SDRAM)
#pragma pack (4)
uint8_t flash_buff[FLASH_BUFF_SIZE] __attribute__((at(SDRAM_BANK_ADDR + ADDRESS_OFFSET))) = {0};	/* 可以放置于外部SDRAM中 */
#else
#pragma pack (4)
uint8_t flash_buff[FLASH_BUFF_SIZE] = {0}; /* 使用内部RAM */
#endif
#pragma pack()


/* 定义flash对象操作方法 */
static struct spi_flash_device  spi_flash_device;

/* User function Declaration --------------------------------------------------*/

/* User functions -------------------------------------------------------------*/

/**
 * @func    flash_lock
 * @brief   给flash总线上互斥锁，禁止其他线程访问
 * @param   * flash_device 设备对象的属性
 * @note  	
 * @retval  无
 */
static void flash_lock(struct spi_flash_device * flash_device)
{
    rt_mutex_take(&flash_device->lock, RT_WAITING_FOREVER);
}

/**
 * @func    flash_unlock
 * @brief   给flash总线释放互斥锁，允许其他线程访问
 * @param   * flash_device 设备对象的属性
 * @note  	
 * @retval  无
 */
static void flash_unlock(struct spi_flash_device * flash_device)
{
    rt_mutex_release(&flash_device->lock);
}

/**
 * @func    w25qxx_read_status
 * @brief   读取w25qxx的状态
 * @note  	
 * @retval  返回w25qxx的状态
 */
static uint8_t w25qxx_read_status(void)
{
    return rt_spi_sendrecv8(spi_flash_device.rt_spi_device, FLASH_CMD_RDSR1);
}

/**
 * @func    w25qxx_wait_busy
 * @brief   等待flash忙过去
 * @note  	
 * @retval  无
 */
static void w25qxx_wait_busy(void)
{
    while( w25qxx_read_status() & (0x01));
}

#ifdef FLASH_W25Q256
/**
 * @func    w25qxx_enter_4_byte_mode
 * @brief   设置w25q256的访问地址大小，对于w25q256来说，可以使用4-byte地址访问，
            当使用3-byte地址访问时只能访问16M大小
 * @note  	
 * @retval  无
 */
static void w25qxx_enter_4_byte_mode(void)
{
    uint8_t send_buffer[1];
    
    /* 写使能 */
    send_buffer[0] = FLASH_CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    /* wait operation done. */
    w25qxx_wait_busy();
    
    /* 设置模式 */
    send_buffer[0] = FLASH_CMD_ENTER_4_BYTE_MODE;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    /* wait operation done. */
    w25qxx_wait_busy();
}

/**
 * @func    w25qxx_exit_4_byte_mode
 * @brief   退出w25q256地址的4-byte访问模式
 * @note  	
 * @retval  无
 */
static void w25qxx_exit_4_byte_mode(void)
{
    uint8_t send_buffer[1];
    
    /* 写使能 */
    send_buffer[0] = FLASH_CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    w25qxx_wait_busy();
    
    /* 写命令 */
    send_buffer[0] = FLASH_CMD_EXIT_4_BYTE_MODE;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    w25qxx_wait_busy();
}
#endif

/**
 * @func    w25qxx_power_down
 * @brief   设置w25qxx进入睡眠模式
 * @note  	
 * @retval  无
 */
static void w25qxx_power_down(void)
{
    uint8_t send_buffer[1];

    /* 写命令 */
    send_buffer[0] = FLASH_CMD_POWER_DOWN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
	
    delay_us(3);
}

/**
 * @func    w25qxx_power_release
 * @brief   w25qxx退出睡眠模式
 * @note  	
 * @retval  无
 */
static void w25qxx_power_release(void)
{
    uint8_t send_buffer[1];

    /* 写命令 */
    send_buffer[0] = FLASH_CMD_POWER_RELEASE;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
	
    delay_us(3);
}

/**
 * @func    w25qxx_ERASE
 * @brief   w25qxx擦除指定的大小
 * @param   cmd 擦除命令 
 * @param   offset 擦除的地址
 * @note  	
 * @retval  无
 */
static void w25qxx_ERASE(uint8_t cmd, uint32_t offset)
{
#ifdef FLASH_W25Q256
    uint8_t send_buffer[5];
#else
    uint8_t send_buffer[4];
#endif
    /* 判断是否是256字节对齐 */
    RT_ASSERT((offset & 0x0FFF) == 0); /* page addr must align to 256byte. */

#ifdef RT_DEBUG
    rt_kprintf("w25q256 is erasing...\r\n");
#endif
    
    /* 写使能 */
    send_buffer[0] = FLASH_CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    w25qxx_wait_busy();
    
    /* 判断擦除命令 */
    if (cmd != FLASH_CMD_ERASE_FULL) /* 非整片擦除 */
    {
#ifdef FLASH_W25Q256
        send_buffer[0] = cmd;
        send_buffer[1] = (uint8_t)(offset>>24);
        send_buffer[2] = (uint8_t)(offset>>16);
        send_buffer[3] = (uint8_t)(offset>>8);
        send_buffer[4] = (uint8_t)(offset);
        rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 5);
#else
        send_buffer[0] = cmd;
        send_buffer[1] = (uint8_t)(offset>>16);
        send_buffer[2] = (uint8_t)(offset>>8);
        send_buffer[3] = (uint8_t)(offset);
        rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 4);
#endif
    }
    else /* 整片擦除 */
    {
        send_buffer[0] = cmd;
        rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    }
    
    w25qxx_wait_busy();

#ifdef RT_DEBUG    
    rt_kprintf("w25q256 is erase ok!\r\n");
#endif
}

/**
 * @func    w25qxx_read
 * @brief   在w25qxx设备中指定的地址读取指定大小的数据
 * @param   offset 读取的地址
 * @param   * buffer 读取数据的缓存区
 * @param   size 读取的大小
 * @note  	
 * @retval  返回操作的大小
 */
static uint32_t w25qxx_read(uint32_t offset, uint8_t * buffer, uint32_t size)
{
#ifdef FLASH_W25Q256
    uint8_t send_buffer[5];
    
    /* 读数据 */
    send_buffer[0] = FLASH_CMD_WRDI;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    /* 指定地址 */
    send_buffer[0] = FLASH_CMD_READ;
    send_buffer[1] = (uint8_t)(offset>>24);
    send_buffer[2] = (uint8_t)(offset>>16);
    send_buffer[3] = (uint8_t)(offset>>8);
    send_buffer[4] = (uint8_t)(offset);
    
    /* 读取数据 */
    rt_spi_send_then_recv(spi_flash_device.rt_spi_device,
                          send_buffer, 5,
                          buffer, size);
#else /* !FLASH_W25Q256 */
    uint8_t send_buffer[4];
    
    send_buffer[0] = FLASH_CMD_WRDI;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
    
    send_buffer[0] = FLASH_CMD_READ;
    send_buffer[1] = (uint8_t)(offset>>16);
    send_buffer[2] = (uint8_t)(offset>>8);
    send_buffer[3] = (uint8_t)(offset);

    rt_spi_send_then_recv(spi_flash_device.rt_spi_device,
                          send_buffer, 4,
                          buffer, size);
#endif /* FLASH_W25Q256 */

    return size;
}

/**
 * @func    w25qxx_write
 * @brief   在w25qxx设备中指定的地址写入指定大小的数据
 * @param   block_addr 写入的扇区的地址
 * @param   offset 扇区的偏移值
 * @param   * buffer 写入数据的缓存区
 * @param   size 写入的大小
 * @note  	
 * @retval  返回操作的大小
 */
static uint32_t w25qxx_write(uint32_t block_addr, uint16_t offset, const uint8_t * buffer, uint16_t size)
{
    uint16_t i;
    uint32_t index;;
    uint8_t * write_point = RT_NULL, page = 0;
#ifdef FLASH_W25Q256
    uint8_t send_buffer[5];
#else
    uint8_t send_buffer[4];
#endif
    /* 读取即将要写入数据的区域 */
    w25qxx_read(block_addr, flash_buff, FLASH_SECTOR_SIZE);
	
    /* 判断是否为空，不为空则执行擦除操作 */
    for (i = offset; i < (offset + size); i++)
    {
        if (flash_buff[i] != 0xff)
        {
            /* 执行擦除操作 */
            send_buffer[0] = FLASH_CMD_WREN;
            rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
            send_buffer[0] = FLASH_CMD_ERASE_4K;
            
#ifdef FLASH_W25Q256         
            send_buffer[1] = (block_addr >> 24);
            send_buffer[2] = (block_addr >> 16);
            send_buffer[3] = (block_addr >> 8);
            send_buffer[4] = (block_addr);
            rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 5);
#else 
            send_buffer[1] = (block_addr >> 16);
            send_buffer[2] = (block_addr >> 8);
            send_buffer[3] = (block_addr);
            rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 4);
#endif            
            w25qxx_wait_busy(); // wait erase done.
            break;
        }
    }
	
    /* 将要写入的数据载入写入缓存区，等待写入 */
    for (i = offset; i < (offset + size); i++)
    {
        flash_buff[i] = buffer[i - offset];
    }
	
    /* 计算写入的页的大小，按页操作 */
    if ((size & 0XFF) == 0)
    {
        page = size >> 8;
    }
    else
    {
        page = size >> 8;
        page += 1;
    }
	
    /* 计算写入的地址 */
    block_addr += (offset & 0xFF00);
	
    /* 计算开始写入的缓存指针，即要写入的数据所在缓存中的位置 */
    write_point = flash_buff + (offset & 0xFF00);
	
    /* 循环写入 */
    for(index = 0; index < page; index++)
    {
        send_buffer[0] = FLASH_CMD_WREN;
        rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);
        
        send_buffer[0] = FLASH_CMD_PP;
#ifdef FLASH_W25Q256
        send_buffer[1] = (uint8_t)(block_addr >> 24);
        send_buffer[2] = (uint8_t)(block_addr >> 16);
        send_buffer[3] = (uint8_t)(block_addr >> 8);
        send_buffer[4] = (uint8_t)(block_addr);
        
        rt_spi_send_then_send(spi_flash_device.rt_spi_device, send_buffer, 5, write_point, 256);             
#else
        send_buffer[1] = (uint8_t)(block_addr >> 16);
        send_buffer[2] = (uint8_t)(block_addr >> 8);
        send_buffer[3] = (uint8_t)(block_addr);
        
        rt_spi_send_then_send(spi_flash_device.rt_spi_device, send_buffer, 4, write_point, 256);
#endif
        write_point += 256;
        block_addr += 256;
        w25qxx_wait_busy();
    }

    send_buffer[0] = FLASH_CMD_WRDI;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    return size;
}

/**
 * @func    w25qxx_flash_read
 * @brief   在w25qxx设备中指定的地址读取指定大小的数据
 * @param   dev 设备对象的属性
 * @param   address 读取的地址
 * @param   * buffer 读取数据的缓存区
 * @param   size 读取的大小
 * @note  	
 * @retval  返回操作的大小
 */
static rt_size_t w25qxx_flash_read(rt_device_t dev,
                                   rt_off_t address,
                                   void * buffer,
                                   rt_size_t size)
{
    /* 上互斥信号量 */
    flash_lock((struct spi_flash_device *)dev);
    
    w25qxx_read(address, buffer, size);
    
    flash_unlock((struct spi_flash_device *)dev);
    
    return size;
}

/**
 * @func    w25qxx_flash_write
 * @brief   在w25qxx设备中指定的地址写入指定大小的数据
 * @param   dev 操作的对象的属性
 * @param   address 写入的地址
 * @param   * buffer 写入数据的缓存区
 * @param   size 写入的大小
 * @note  	
 * @retval  返回操作的大小
 */
static rt_size_t w25qxx_flash_write(rt_device_t dev,
                                    rt_off_t address,
                                    const void * buffer,
                                    rt_size_t size)
{
    rt_size_t i = 0;
    rt_size_t sector_addr = (rt_size_t)(address / FLASH_SECTOR_SIZE);
    rt_size_t block = (rt_size_t)(size / FLASH_SECTOR_SIZE);
    rt_size_t write_address = address;
    rt_size_t write_size, tmp_size = size;
	
    const uint8_t * ptr = buffer;

	/* 上互斥信号量 */
    flash_lock((struct spi_flash_device *)dev);
	
	/* 按块循环写入 */
    do
    {
        if (tmp_size >= 4096)
        {
            write_size = 4096;
        }
        else
        {
            write_size = tmp_size;
        }
		
        w25qxx_write((sector_addr + i) * spi_flash_device.geometry.bytes_per_sector, 
                     write_address % 4096, 
                     ptr, 
                     write_size);

        ptr += write_size;
        write_address += write_size;
        tmp_size -= write_size;
        i++;  
    }while(block--);
	
    flash_unlock((struct spi_flash_device *)dev);

    return size;
}

/**
 * @func    w25qxx_flash_open
 * @brief   打开w25qxx设备
 * @param   dev 操作的对象的属性
 * @param   oflag 操作的标志
 * @note  	
 * @retval  操作结果 RT_EOK 操作成功
 */
static rt_err_t w25qxx_flash_open(rt_device_t dev, rt_uint16_t oflag)
{
    uint8_t send_buffer[3];

    flash_lock((struct spi_flash_device *)dev);

    send_buffer[0] = FLASH_CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    send_buffer[0] = FLASH_CMD_WRSR;
    send_buffer[1] = 0;
    send_buffer[2] = 0;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 3);

    w25qxx_wait_busy();

    flash_unlock((struct spi_flash_device *)dev);

    return RT_EOK;
}

/**
 * @func    w25qxx_flash_close
 * @brief   关闭w25qxx设备
 * @param   dev 操作的对象的属性
 * @note  	
 * @retval  操作结果 RT_EOK 操作成功
 */
static rt_err_t w25qxx_flash_close(rt_device_t dev)
{
    return RT_EOK;
}

/**
 * @func    w25qxx_flash_control
 * @brief   控制w25qxx设备
 * @param   dev 操作的对象的属性
 * @param   cmd 控制操作的命令
 * @param   args 操作的用户数据
 * @note  	
 * @retval  操作结果 RT_EOK 操作成功
 */
static rt_err_t w25qxx_flash_control(rt_device_t dev, int cmd, void * args)
{
    RT_ASSERT(dev != RT_NULL);

    flash_lock((struct spi_flash_device *)dev);
  
    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        
        if (geometry == RT_NULL) 
        {
            return -RT_ERROR;
        }

        geometry->bytes_per_sector = spi_flash_device.geometry.bytes_per_sector;
        geometry->sector_count = spi_flash_device.geometry.sector_count;
        geometry->block_size = spi_flash_device.geometry.block_size;
    }
    else if (cmd == FLASH_CMD_ERASE_4K)
    {
        uint32_t address = (uint32_t)args;
        w25qxx_ERASE(FLASH_CMD_ERASE_4K, address);
    }
    else if (cmd == FLASH_CMD_ERASE_32K)
    {
        uint32_t address = (uint32_t)args;
        w25qxx_ERASE(FLASH_CMD_ERASE_32K, address);
    }
    else if (cmd == FLASH_CMD_ERASE_64K)
    {
        uint32_t address = (uint32_t)args;
        w25qxx_ERASE(FLASH_CMD_ERASE_64K, address);
    }
    else if (cmd == FLASH_CMD_ERASE_FULL)
    {
        uint32_t address = (uint32_t)args;
        w25qxx_ERASE(FLASH_CMD_ERASE_FULL, address);
    }
    else if (cmd == FLASH_CMD_POWER_DOWN)
    {
        w25qxx_power_down();
    }
    else if (cmd == FLASH_CMD_POWER_RELEASE)
    {
        w25qxx_power_release();
    }
#ifdef FLASH_W25Q256
    else if (cmd == FLASH_CMD_ENTER_4_BYTE_MODE)
    {
        w25qxx_enter_4_byte_mode();
    }
    else if (cmd == FLASH_CMD_EXIT_4_BYTE_MODE)
    {
        w25qxx_exit_4_byte_mode();
    }
#endif /* FLASH_W25Q256 */

    flash_unlock((struct spi_flash_device *)dev);
	
    return RT_EOK;
}

/**
 * @func    w25qxx_flash_init
 * @brief   初始化w25qxx设备
 * @param   dev 操作的对象的属性
 * @note  	
 * @retval  操作结果 RT_EOK 操作成功
 */
static rt_err_t w25qxx_flash_init(rt_device_t dev)
{
#ifdef FLASH_W25Q256
    w25qxx_flash_control(dev, FLASH_CMD_ENTER_4_BYTE_MODE, RT_NULL);
#endif
    return RT_EOK;
}

/**
 * @func    w25qxx_init
 * @brief   w25qxx初始化方法
 * @param   flash_device_name 设备的名称
 * @param   spi_device_name SPI总线设备对象的名称
                在此flash是挂载在spi2上面
 * @note  	无
 * @retval	无
 */
rt_err_t w25qxx_init(const char * flash_device_name, const char * spi_device_name)
{
    struct rt_spi_device * rt_spi_device;
	
    /* 初始化互斥信号量 */
    if (rt_mutex_init(&spi_flash_device.lock, spi_device_name, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        rt_kprintf("init flash lock mutex failed\n");
        return -RT_ENOSYS;
    }

    /* 寻找指定的名称的对象 */
    rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(rt_spi_device == RT_NULL)
    {
        FLASH_TRACE("spi device %s not found!\r\n", spi_device_name);
        return -RT_ENOSYS;
    }
	
    spi_flash_device.rt_spi_device = rt_spi_device;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8; /*数据宽度是8bit */
        cfg.mode = RT_SPI_MODE_3 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 80 * 1000 * 1000; /* 10M */
        rt_spi_configure(spi_flash_device.rt_spi_device, &cfg);
    }

    /* init flash */
    {
        rt_uint8_t cmd;
        rt_uint8_t id_recv[3];
        uint16_t memory_type_capacity;

        /* 操作过程需要上互斥信号锁 */
        flash_lock(&spi_flash_device);

        cmd = 0xFF; /* reset SPI FLASH, cancel all cmd in processing. */
        rt_spi_send(spi_flash_device.rt_spi_device, &cmd, 1);

        cmd = FLASH_CMD_WRDI;
        rt_spi_send(spi_flash_device.rt_spi_device, &cmd, 1);

        /* read flash id */
        cmd = FLASH_CMD_JEDEC_ID;
        rt_spi_send_then_recv(spi_flash_device.rt_spi_device, &cmd, 1, id_recv, 3);

        flash_unlock(&spi_flash_device);

        if(id_recv[0] != MF_ID)
        {
            FLASH_TRACE("Manufacturers ID error!\r\n");
            FLASH_TRACE("JEDEC Read-ID Data : %02X %02X %02X\r\n", id_recv[0], id_recv[1], id_recv[2]);
            return -RT_ENOSYS;
        }

        spi_flash_device.geometry.bytes_per_sector = FLASH_SECTOR_SIZE; /* 扇区大小 */
        spi_flash_device.geometry.block_size = FLASH_SECTOR_SIZE; /* 捈除块的大小 4k */

        /* get memory type and capacity */
        memory_type_capacity = id_recv[1];
        memory_type_capacity = (memory_type_capacity << 8) | id_recv[2];

        if (memory_type_capacity == MTC_W25Q256_FV)
        {
            FLASH_TRACE("W25Q256FV detection\r\n");
            spi_flash_device.geometry.sector_count = 8192;
        }
        else if(memory_type_capacity == MTC_W25Q128_BV)
        {
            FLASH_TRACE("W25Q128BV detection\r\n");
            spi_flash_device.geometry.sector_count = 4096;
        }
        else if(memory_type_capacity == MTC_W25Q64_BV_CV)
        {
            FLASH_TRACE("W25Q64BV or W25Q64CV detection\r\n");
            spi_flash_device.geometry.sector_count = 2048;
        }
        else if(memory_type_capacity == MTC_W25Q64_DW)
        {
            FLASH_TRACE("W25Q64DW detection\r\n");
            spi_flash_device.geometry.sector_count = 2048;
        }
        else if(memory_type_capacity == MTC_W25Q32_BV)
        {
            FLASH_TRACE("W25Q32BV detection\r\n");
            spi_flash_device.geometry.sector_count = 1024;
        }
        else if(memory_type_capacity == MTC_W25Q32_DW)
        {
            FLASH_TRACE("W25Q32DW detection\r\n");
            spi_flash_device.geometry.sector_count = 1024;
        }
        else if(memory_type_capacity == MTC_W25Q16_BV_CL_CV)
        {
            FLASH_TRACE("W25Q16BV or W25Q16CL or W25Q16CV detection\r\n");
            spi_flash_device.geometry.sector_count = 512;
        }
        else if(memory_type_capacity == MTC_W25Q16_DW)
        {
            FLASH_TRACE("W25Q16DW detection\r\n");
            spi_flash_device.geometry.sector_count = 512;
        }
        else if(memory_type_capacity == MTC_W25Q80_BV)
        {
            FLASH_TRACE("W25Q80BV detection\r\n");
            spi_flash_device.geometry.sector_count = 256;
        }
        else
        {
            FLASH_TRACE("Memory Capacity error!\r\n");
            return -RT_ENOSYS;
        }
    }

    /* 注册设备操作函数 */
    spi_flash_device.flash_device.type    = RT_Device_Class_MTD;
    spi_flash_device.flash_device.init    = w25qxx_flash_init;
    spi_flash_device.flash_device.open    = w25qxx_flash_open;
    spi_flash_device.flash_device.close   = w25qxx_flash_close;
    spi_flash_device.flash_device.read    = w25qxx_flash_read;
    spi_flash_device.flash_device.write   = w25qxx_flash_write;
    spi_flash_device.flash_device.control = w25qxx_flash_control;
	
    /* 没有私有数据 */
    spi_flash_device.flash_device.user_data = RT_NULL;

    /* 注册设备 */
    rt_device_register(&spi_flash_device.flash_device, flash_device_name,
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

    return RT_EOK;
}
