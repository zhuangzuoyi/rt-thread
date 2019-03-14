#include <rtthread.h>

#ifdef BSP_USING_SDCARD

#include <dfs_elm.h>
#include <dfs_fs.h>
#include <dfs_posix.h>
#include "drv_spi.h"
#include "spi_msd.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME  "SDCard"
#define DBG_COLOR

#define DBG_LEVEL DBG_INFO


#include <rtdbg.h>


void mount_sdcard_sd0(void *parameter)
{
    uint8_t i=0;
    while (1)
    {
        
        if(rt_device_find("sd0") != RT_NULL)
        {
            if (dfs_mount("sd0", "/", "elm", 0, 0) == RT_EOK)
            {
                LOG_I("sd card mount to '/'");
                break;
            }
            else
            {
                LOG_W("sd card mount to '/' failed!");
            }
        }
        i++;
        if(i>5)
        {
            LOG_W("sd card mount to '/' failed!,exit task");
        }
        rt_thread_mdelay(500);
    }
}

FINSH_FUNCTION_EXPORT(mount_sdcard_sd0, mount sdcard sd0);
MSH_CMD_EXPORT(mount_sdcard_sd0, mount sdcard sd0);

static int rt_hw_spi1_tfcard(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    rt_hw_spi_device_attach("spi1", "spi11", GPIOB, GPIO_PIN_8);

    return msd_init("sd0", "spi11");
}
INIT_DEVICE_EXPORT(rt_hw_spi1_tfcard);

#endif /* BSP_USING_SDCARD */

