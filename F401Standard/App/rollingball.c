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

int32_t appcount;//rollingballrun���б�־λ
int32_t ball_lea_count;//���뿪��ʱ��
int32_t GetTick_temp; //������һ��ʶ�����ϵͳʱ��

int32_t	loc_account;//����ֹͣλ�ñ�־λ
_Bool ball_leave = 0;
extern unsigned char RXOVER;
extern unsigned char RXBUF[4];

pid_type_def M1PID;//PID�ṹ��
pid_type_def M2PID;

M1TypeDef M1;//����ṹ�壨X�������꣬���ֵ��
M2TypeDef M2;//��Y�������꣬���ֵ��

fp32 pid[3];//������ʼ��

int time = 20;//�������ڱ���

void USER_Mode(uint32_t modenum);

//��ʱ��Ƶ��=(TIM_Prescaler /84M)*TIM_Period 
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
	if(User_GetTick() - appcount >= time)//���������������
	{	
		appcount = User_GetTick();
		
		if(RXBUF[0] == 255 && RXBUF[1] == 255)//openmvδʶ����ʱ����255
		{
			if(User_GetTick() - GetTick_temp >= 2000)//���뿪���������ƽ
			{
				ball_leave = 1;
			}
		}
		else
		{
			GetTick_temp = User_GetTick();//������һ��ʶ��С���ʱ��
			
			ball_leave = 0;
		}
		USER_Mode(1);//ѡ��ģʽ
		bt_SendData(M1.coordinate,M2.coordinate,M1PID.out,M2PID.out);//���������͵�ǰ����������ֵ
		
	}
}




uint32_t numb = 0;
void USER_Mode(uint32_t modenum)//ģʽѡ��
{
	if(modenum == 0)
	{
			
	}
	
	if(modenum == 1)//����Ϊ120*160,���Ϊ��Ļ���Ͻǣ����ţ�
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
			loca_x = RXBUF[0] * x_ratio + loca_x * (1-x_ratio);//��ͨ�˲�
			loca_y = RXBUF[1] * y_ratio + loca_y * (1-y_ratio);
				
			M1.coordinate = loca_x - X_Center;//��������ԭ��Ϊ������
			M2.coordinate = loca_y - Y_Center;
		}
		
		M1.PWM = PID_calc1(&M1PID,-M1.coordinate,0);	//X����PID����
		M2.PWM = PID_calc1(&M2PID,-M2.coordinate,0);  //Y����PID����
		
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
			loca_x = RXBUF[0] * x_ratio + loca_x * (1-x_ratio);//��ͨ�˲�
			loca_y = RXBUF[1] * y_ratio + loca_y * (1-y_ratio);
				
			M1.coordinate = loca_x - X_Center;//��������ԭ��Ϊ������
			M2.coordinate = loca_y - Y_Center;
		}
		
		
		M1.PWM = PID_calc(&M1PID,-M1.coordinate,set_x);	//X����PID����
		M2.PWM = PID_calc(&M2PID,-M2.coordinate,set_y);  //Y����PID����
		
		MotorMove(M1.PWM,M2.PWM);
	}
	
	
}

