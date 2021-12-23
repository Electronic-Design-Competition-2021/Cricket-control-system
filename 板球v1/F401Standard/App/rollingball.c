#include "rollingball.h"
#include "main.h"
#include "tim.h"
#include "pid.h"
#include "tim.h"
#include "bsp_delay.h"
#include "bsp_pwm.h"
#include "steeringengine.h"
#include "openmv.h"
#include "usart.h"
#include "bluetooth.h"

int32_t appcount;//rollingballrun运行标志位
int32_t ball_lea_count;//球离开板时间
int32_t GetTick_temp; //存放最后一次识别到球的系统时间

int32_t	loc_account;//更换停止位置标志位
_Bool ball_leave = 0;
extern unsigned char RXOVER;
extern unsigned char RXBUF[4];

pid_type_def M1PID;//PID结构体
pid_type_def M2PID;

M1TypeDef M1;//舵机结构体（X方向坐标，输出值）
M2TypeDef M2;//（Y方向坐标，输出值）

fp32 pid[3];//参数初始化

int time = 20;//运算周期变量

void USER_Mode(uint32_t modenum);

//定时器频率=(TIM_Prescaler /84M)*TIM_Period 
void rollingballInit(void)
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	
	pid[0] = 0;
	pid[1] = 0;
	pid[2] = 0;
	
	PID_init(&M1PID,PID_POSITION,pid,10000,10000);
	PID_init(&M2PID,PID_POSITION,pid,10000,10000);
}

float x_tem= 0,y_tem = 0;
float loca_x = 0,loca_y = 0;
float x_ratio = 0.2f,y_ratio = 0.2f;
void rollingballRun(void)
{
	if(User_GetTick() - appcount >= time)//设置输出计算周期
	{	
		appcount = User_GetTick();
		
		if(RXBUF[0] == 255 && RXBUF[1] == 255)//openmv未识别到球时发送255
		{
			if(User_GetTick() - GetTick_temp >= 2000)//球离开板后两秒板调平
			{
				ball_leave = 1;
			}
		}
		else
		{
			GetTick_temp = User_GetTick();//标记最后一次识别到小球的时间
			
			ball_leave = 0;
		}
		USER_Mode(1);//选择模式
		bt_SendData(M1.coordinate,M2.coordinate,M1PID.out,M2PID.out);//向蓝牙发送当前球坐标和输出值
		
	}
}




uint32_t numb = 0;
void USER_Mode(uint32_t modenum)//模式选择
{
	if(modenum == 0)
	{
			
	}
	
	if(modenum == 1)//像素为120*160,零点为屏幕左上角（竖着）
	{
		
		if(ball_leave == 1)
		{
			loca_x = X_Center;
			loca_y = Y_Center;

			M1.coordinate = loca_x - X_Center;
			M2.coordinate = loca_y - Y_Center;
			}
		else if(ball_leave == 0&&RXBUF[0] != 255 && RXBUF[1] != 255)
		{
			loca_x = RXBUF[0] * x_ratio + loca_x * (1-x_ratio);//低通滤波
			loca_y = RXBUF[1] * y_ratio + loca_y * (1-y_ratio);
				
			M1.coordinate = loca_x - X_Center;//设置坐标原点为板中心
			M2.coordinate = loca_y - Y_Center;
		}
		
		M1.PWM = PID_calc1(&M1PID,-M1.coordinate,0);	//X方向PID计算
		M2.PWM = PID_calc1(&M2PID,-M2.coordinate,0);  //Y方向PID计算
		
		MotorMove(M1.PWM,M2.PWM);
	}
	
	if(modenum == 2)
	{
		int32_t coordinatesum[9][2] = {{45,37},{45,72},{45,116},{69,38},{69,79},{69,118},{93,40},{93,78},{92,117}};
		
		int32_t set_x;
		int32_t set_y;
		
		if(User_GetTick() - loc_account >= 3000)
		{
			loc_account = User_GetTick();
			numb++;
			if(numb == 8)
			{
				numb = 0;
			}
		}
		
		set_x = coordinatesum[numb][0] - X_Center;
		set_y = coordinatesum[numb][1] - Y_Center;
		
		
		if(ball_leave == 1)
		{
			loca_x = coordinatesum[numb][0];
			loca_y = coordinatesum[numb][1];

			M1.coordinate = loca_x - X_Center;
			M2.coordinate = loca_y - Y_Center;
			}
		else if(ball_leave == 0&&RXBUF[0] != 255 && RXBUF[1] != 255)
		{
			loca_x = RXBUF[0] * x_ratio + loca_x * (1-x_ratio);//低通滤波
			loca_y = RXBUF[1] * y_ratio + loca_y * (1-y_ratio);
				
			M1.coordinate = loca_x - X_Center;//设置坐标原点为板中心
			M2.coordinate = loca_y - Y_Center;
		}
		
		
		M1.PWM = PID_calc(&M1PID,-M1.coordinate,set_x);	//X方向PID计算
		M2.PWM = PID_calc(&M2PID,-M2.coordinate,set_y);  //Y方向PID计算
		
		MotorMove(M1.PWM,M2.PWM);
	}
	
	
}

