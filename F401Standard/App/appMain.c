#include "appMain.h"
#include "pid.h"
#include "fifo.h"
#include "usart.h"
#include "bluetooth.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "bsp_usart.h"
#include "bsp_lcd_init.h"
#include "bsp_lcd.h"
#include "pic.h"
#include "spi.h"
#include "bsp_24l01.h"
#include "menu.h"
#include "appMenu.h"
#include "Oscilloscope.h"
#include "rollingball.h"
#include "steeringengine.h"


void appInit(void)
{
	usart1_init();	//初始化串口1
	NRF24L01_Init();			//初始化NRF24L01
	__HAL_SPI_ENABLE(&hspi1);	//使能SPI
	
	rollingballInit();//板球系统初始化
	
	
	SET_BIT(hspi1.Instance->CR2, SPI_CR2_TXDMAEN);	//开启spi1的DMA

	//LCD_Init();			//LCD初始化
	User_Delay(100);
	//LCD_ShowString(COLUMN0,LINE0,"GUI DEMO - DOEE",GREEN,BLACK,16,0);	//显示接收到的数据
	//LCD_Fill(0,0,LCD_W,LCD_H,BLACK);	//初始颜色

	SPI1_NRF24_ReInit(SPI_BAUDRATEPRESCALER_16); //SPI1重新初始化
//	while(NRF24L01_Check() == 1)	//检查NRF24L01
//	{
//		HAL_Delay(100);
//		User_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);	//如果未检测到：led快闪
//	}

//	menuInit(&GUI,menu1);	//初始化菜单
}


uint32_t ledTick;
uint32_t nrfTick = 0;

uint8_t nrfBuf[32];

void appMain(void)
{
	rollingballRun();//板球系统运行
	
	//menuRefresh(&GUI);			//刷新GUI:菜单在appMenu.c
	bt_DataProcess(&u1_Fifo);	//蓝牙数据处理
	if(User_GetTick() - ledTick >= 500)
	{
		ledTick = User_GetTick();
		//HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);	//LED闪烁
	}
	
//	SPI1_NRF24_ReInit(SPI_BAUDRATEPRESCALER_16); //SPI1重新初始化
//	if(User_GetTick() - nrfTick >= 10)
//	{
//		nrfTick = User_GetTick();
//		if(NRF24L01_RxPacket(nrfBuf)==0)//NRF接收到信息
//		{
//			SPI1_LCD_ReInit(SPI_BAUDRATEPRESCALER_2);	//重新初始化SPI1
//			LCD_ShowString(10,20,nrfBuf,RED,WHITE,16,0);	//显示接收到的数据
//		}
//	}

}
