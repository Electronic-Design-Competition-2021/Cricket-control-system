/**
  ****************************(C) COPYRIGHT 2021 DOEE****************************
  * @file      	appMenu.c/h
  * @brief     	多级菜单（用户层）
  * @note      	每个菜单界面以一个独立函数的形式被调用，新建菜单仅需新建一个函数，
				通过调用menuSkip实现菜单跳转，相关函数见menu.c/h
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     July-1-2021     doee            1. 完成
  * @blog		https://blog.csdn.net/weixin_44578655?spm=1001.2101.3001.5343
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2021 DOEE****************************
  */
  
#include "appMenu.h"
#include "main.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcd_init.h"
#include "bluetooth.h"
#include "menu.h"
#include "Oscilloscope.h"

menu_s_t GUI;	//定义多级菜单

void menu1(void)
{
	SPI1_LCD_ReInit(SPI_BAUDRATEPRESCALER_2);	//重新初始化SPI1
	if(GUI.init_flag)
	{
		//发生界面切换
		GUI.init_flag = 0;
		LCD_Fill(0,0,LCD_W,LCD_H,BLACK);	//初始颜色
	}
	LCD_ShowString(COLUMN0,LINE0,"GUI DEMO - DOEE",GREEN,BLACK,16,0);	//显示接收到的数据
	LCD_ShowString(COLUMN0,LINE1,"FLOAT:",GREEN,BLACK,16,0);	//显示接收到的数据
	LCD_ShowFloatNum1(COLUMN1,LINE1,btData.float1,5,GREEN,BLACK,16);	//显示小数

	if(btData.buttonRes == 2)
	{
		menuSkip(&GUI,menu2);	//跳转至menu2
	}
	else if(btData.buttonRes == 3)
	{
		menuSkip(&GUI,menu3);	//跳转至menu3
	}
	else if(btData.buttonRes == 4)
	{
		menuReturn(&GUI);		//返回界面
	}
	btData.buttonRes = 0;
}

//定义示波器窗口
osc_window osc_win;
uint32_t osc_x = 0;
void menu2(void)
{
	SPI1_LCD_ReInit(SPI_BAUDRATEPRESCALER_2);	//重新初始化SPI1
	
	if(GUI.init_flag)
	{
		//发生界面切换
		GUI.init_flag = 0;
		LCD_Fill(0,0,LCD_W,LCD_H,BLACK);	//初始颜色
		osc_init(&osc_win,0,0,LCD_W,LCD_H,1);		//示波器初始化,模式0
		osc_y_ratio_bias_set(&osc_win,(osc_win.width/255.0f),-128);
//		osc_init(&osc_win,uint16_t loca_x_set,uint16_t loca_y_set,uint16_t length_set,uint16_t width_set);
	}

	//LCD_ShowString(10,20,"menu2",RED,WHITE,16,0);	//显示接收到的数据
	osc_refresh(&osc_win,osc_x++,btData.float1);	//刷新示波器串口（以屏幕像素点为单位）
	if(btData.buttonRes == 1)
	{
		menuSkip(&GUI,menu1);	//跳转至menu1
	}
	else if(btData.buttonRes == 3)
	{
		menuSkip(&GUI,menu3);	//跳转至menu3		
	}
	else if(btData.buttonRes == 4)
	{
		menuReturn(&GUI);		//返回界面
	}
	btData.buttonRes = 0;
}
void menu3(void)
{
	SPI1_LCD_ReInit(SPI_BAUDRATEPRESCALER_2);	//重新初始化SPI1
	if(GUI.init_flag)
	{
		//发生界面切换
		GUI.init_flag = 0;
		LCD_Fill(0,0,LCD_W,LCD_H,GREEN);	//初始颜色
	}
	LCD_ShowString(10,20,"menu3",RED,WHITE,16,0);	//显示接收到的数据
	if(btData.buttonRes == 1)
	{
		menuSkip(&GUI,menu1);	//跳转至menu1
	}
	else if(btData.buttonRes == 2)
	{
		menuSkip(&GUI,menu2);	//跳转至menu2	
	}
	else if(btData.buttonRes == 4)
	{
		menuReturn(&GUI);		//返回界面
	}
	btData.buttonRes = 0;
}
void menu4(void)
{
	SPI1_LCD_ReInit(SPI_BAUDRATEPRESCALER_2);	//重新初始化SPI1
	if(GUI.init_flag)
	{
		//发生界面切换
		GUI.init_flag = 0;
		LCD_Fill(0,0,LCD_W,LCD_H,YELLOW);	//初始颜色
	}
	LCD_ShowString(10,20,"menu4",RED,WHITE,16,0);	//显示接收到的数据
}


