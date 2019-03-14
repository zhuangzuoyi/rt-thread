#include <rtdevice.h>
#include "drv_spi.h"
#include "drv_ssd1306.h"

#ifdef  BSP_USING_SSD1306

#ifdef BSP_USING_SDCARD
#include <dfs_posix.h>
#endif

#define DBG_ENABLE
#define DBG_SECTION_NAME  "SSD1306"
#define DBG_COLOR

#define DBG_LEVEL DBG_INFO

#include <rtdbg.h>


#define SSD1306_DC_PIN  GET_PIN(A, 15)
#define SSD1306_RES_PIN GET_PIN(A, 2)


uint8_t SSD1306_GRAM[128][8];	 


static struct rt_spi_device *spi_dev_lcd;




static rt_err_t ssd1306_write_cmd(const rt_uint8_t cmd)
{
    rt_size_t len;

    rt_pin_write(SSD1306_DC_PIN, PIN_LOW);

    len = rt_spi_send(spi_dev_lcd, &cmd, 1);

    if (len != 1)
    {
        LOG_I("ssd1306_write_cmd error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}

static rt_err_t ssd1306_write_data(const rt_uint8_t data)
{
    rt_size_t len;

    rt_pin_write(SSD1306_DC_PIN, PIN_HIGH);

    len = rt_spi_send(spi_dev_lcd, &data, 1);

    if (len != 1)
    {
        LOG_I("ssd1306_write_data error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}


static rt_err_t ssd1306_write_datas(const rt_uint8_t *data,rt_uint8_t length)
{
    rt_size_t len;

    rt_pin_write(SSD1306_DC_PIN, PIN_HIGH);

    len = rt_spi_send(spi_dev_lcd, data, length);

    if (len != length)
    {
        LOG_I("ssd1306_write_data error. %d", len);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}


void ssd1306_fille(uint8_t *dat)
{
	uint8_t i,n;	

	for(i=0;i<8;i++)  
	{  
		ssd1306_write_cmd (0xb0+i);   
		ssd1306_write_cmd (0x00);      
		ssd1306_write_cmd (0x10);      
        ssd1306_write_datas(dat,128);  
        dat +=  128;
	}   
}



void ssd1306_refresh_gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		ssd1306_write_cmd (0xb0+i);   
		ssd1306_write_cmd (0x00);      
		ssd1306_write_cmd (0x10);      
		for(n=0;n<128;n++)
            ssd1306_write_data(SSD1306_GRAM[n][i]); 
	}   
}

void ssd1306_clear(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)SSD1306_GRAM[n][i]=0X00;  
	    ssd1306_refresh_gram();
}
FINSH_FUNCTION_EXPORT(ssd1306_clear, clear ssd136 screan);
MSH_CMD_EXPORT(ssd1306_clear,  clear ssd136 screan);

void ssd1306_fill(void)  
{  
	uint8_t i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)SSD1306_GRAM[n][i]=0Xff;  
	    ssd1306_refresh_gram();
}
FINSH_FUNCTION_EXPORT(ssd1306_fill, fill ssd136 screan);
MSH_CMD_EXPORT(ssd1306_fill, fill ssd136 screan);

static int rt_hw_ssd1306_config(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    rt_hw_spi_device_attach("spi1", "spi10", GPIOB, GPIO_PIN_2);

    spi_dev_lcd = (struct rt_spi_device *)rt_device_find("spi10");

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
        cfg.max_hz = 42 * 1000 * 1000; /* 42M,SPI max 42MHz,lcd 4-wire spi */

        rt_spi_configure(spi_dev_lcd, &cfg);
    }

    rt_pin_mode(SSD1306_DC_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(SSD1306_RES_PIN, PIN_MODE_OUTPUT);

    rt_pin_write(SSD1306_RES_PIN, PIN_HIGH);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    rt_pin_write(SSD1306_RES_PIN, PIN_LOW);
    rt_thread_delay(RT_TICK_PER_SECOND / 10);
    rt_pin_write(SSD1306_RES_PIN, PIN_HIGH);

    return RT_EOK;
}





static int rt_hw_ssd1306_init(void)
{
    
    rt_hw_ssd1306_config();


	ssd1306_write_cmd(0xAE); 
	ssd1306_write_cmd(0xD5); 
	ssd1306_write_cmd(80);  
	ssd1306_write_cmd(0xA8);
	ssd1306_write_cmd(0X3F);
	ssd1306_write_cmd(0xD3); 
	ssd1306_write_cmd(0X00);

	ssd1306_write_cmd(0x40); 
													    
	ssd1306_write_cmd(0x8D); 
	ssd1306_write_cmd(0x14); 
	ssd1306_write_cmd(0x20); 
	ssd1306_write_cmd(0x02); 
	ssd1306_write_cmd(0xA1); 
	ssd1306_write_cmd(0xC0);
	ssd1306_write_cmd(0xDA); 
	ssd1306_write_cmd(0x12); 
		 
	ssd1306_write_cmd(0x81); 
	ssd1306_write_cmd(0xEF);
	ssd1306_write_cmd(0xD9); 
	ssd1306_write_cmd(0xf1); 
	ssd1306_write_cmd(0xDB); 
	ssd1306_write_cmd(0x30); 

	ssd1306_write_cmd(0xA4); 
	ssd1306_write_cmd(0xA6); 
	ssd1306_write_cmd(0xAF); 

    ssd1306_fill();

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_ssd1306_init);


#ifdef BSP_USING_SDCARD

void bad_apple(void)
{
    int fd, size,i,j=0;
    char buffer[1024];


    /* 以只读模式打开 /text.txt 文件 */
    fd = open("/badapple.bin", O_RDONLY);
    if (fd >= 0)
    {
        for(;;)
        {
            size = read(fd, buffer, sizeof(buffer));
            if(size <0 ||size ==0)
                break;
            ssd1306_fille(buffer);
            rt_thread_delay(40);
        }
        
        close(fd);
}
}
FINSH_FUNCTION_EXPORT(bad_apple, bad apple player);
MSH_CMD_EXPORT(bad_apple, bad apple player);

#endif



#endif