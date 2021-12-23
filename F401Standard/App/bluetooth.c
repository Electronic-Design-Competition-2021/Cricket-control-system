#include "bluetooth.h"
#include "string.h"
#include "stdio.h"
#include "fifo.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"
#include "bsp_lcd_init.h"
#include "bsp_lcd.h"
#include "string.h"
#include "menu.h"
#include "appMenu.h"
#include "steeringengine.h"
#include "rollingball.h"
#include "pid.h"

extern pid_type_def M1PID;
extern pid_type_def M2PID;
extern float x_ratio,y_ratio;
extern float d_LIMIT;

uint8_t bt_tx_buf[BT_TX_LENGTH];
uint8_t btProcBuf[BT_RX_LENGTH];
btDataType btData;
//������λ�����ݷ���,����32λ����
void bt_SendData(float data1 ,float data2 ,float data3 ,float data4)
{
	uint8_t i = 0;
	memset(bt_tx_buf,0,BT_TX_LENGTH);
	bt_tx_buf[0] = 0xA5;
	for( i = 0;i < 4; i ++ )
	{
		bt_tx_buf[i+1] = *((uint8_t *)(&data1) + i);
	}
	for( i = 0;i < 4; i ++ )
	{
		bt_tx_buf[i+5] = *((uint8_t *)(&data2) + i);
	}
	for( i = 0;i < 4; i ++ )
	{
		bt_tx_buf[i+9] = *((uint8_t *)(&data3) + i);
	}
	for( i = 0;i < 4; i ++ )
	{
		bt_tx_buf[i+13] = *((uint8_t *)(&data4) + i);
	}
	for(i = 1;i<BT_TX_LENGTH-2;i++)
	{
		bt_tx_buf[BT_TX_LENGTH-2] += bt_tx_buf[i];
	}
	
	bt_tx_buf[BT_TX_LENGTH-1] = 0x5A;
	usart1_SendData(bt_tx_buf,BT_TX_LENGTH);	//�ĳ����⴮�ڼ���
}
void bt_DataUnPack(uint8_t *data)
{
	float temp_float1 = 0;
	float temp_float2 = 0;
	float temp_float3 = 0;
	float temp_float4 = 0;
	float temp_float5 = 0;
	float temp_float6 = 0;
	float temp_float7 = 0;
	float temp_float8 = 0;
	float temp_float9 = 0;
	
	
	memcpy(&temp_float1,data,4);	//floatλ�ã���2��3��4��5�ֽ�
	memcpy(&temp_float2,data+4,4);
	memcpy(&temp_float3,data+8,4);
	memcpy(&temp_float4,data+12,4);
	memcpy(&temp_float5,data+16,4);
	memcpy(&temp_float6,data+20,4);
	memcpy(&temp_float7,data+24,4);
	memcpy(&temp_float8,data+28,4);
	memcpy(&temp_float9,data+32,4);
	
	M1PID.Kp = temp_float1;
	M1PID.Ki = temp_float2;
	M1PID.Kd = temp_float3;
	M2PID.Kp = temp_float4;
	M2PID.Ki = temp_float5;
	M2PID.Kd = temp_float6;
	x_ratio = temp_float7;
	y_ratio = temp_float8;
	d_LIMIT = temp_float9;
	//menuRefreshFlagSet(&GUI);		//GUIˢ�±�־λ��λ

}

void bt_DataSend(btDataType_tx * data)
{
	uint8_t tx_buf[BT_TX_LENGTH];
	tx_buf[0] = 0xA5;
	memcpy(tx_buf+1,&data->float1,4);	//float���Ƶ�������
	
//	memcpy(tx_buf+1,&data->float1,4);	//float���Ƶ�������
}
uint8_t buf[4];
uint8_t openmv[2];
void uartIRQ()
{
	uint8_t cnt = 0,dat = 0;
	if(cnt == 3 && dat == '\n')
	{
		openmv[0] = buf[0];
		openmv[1] = buf[1];
		cnt = 0;
		return;
	}
	buf[cnt++] = dat;
}
void bt_DataProcess(fifo_s_t * p_fifo)
{
	static uint8_t proc_step = 0;
	uint8_t head = 0,end = 0;
	uint8_t i = 0,sum = 0,adj_sum = 0;
	
	if(fifo_s_used(p_fifo))
	{
		if(proc_step == 0)	
		{
			//step1����λ֡ͷ
			while(head != 0xA5)
			{
				if(fifo_s_gets(p_fifo,(char*)&head,1) == -1)	//����֡ͷ
					return;	//��ȫ��ȡ��,δ����֡ͷ��ֱ�ӷ���
			}
			proc_step = 1;	//�ҵ�֡ͷ������step2
		}
		if(proc_step == 1)
		{
			//step2����������
			if(fifo_s_used(p_fifo) >= (BT_RX_LENGTH - 1))	//�ж�fifo�е����ݳ���
			{
				fifo_s_gets(p_fifo,(char*)&btProcBuf,BT_RX_LENGTH - 3);	//��������
				proc_step = 2;	//����������ɣ�����step3
			}
			else
				return;	//���ݻ�δ�����꣬�˳�
		}
		if(proc_step == 2)
		{
			//step3������У��� & �ж�֡β
			if(fifo_s_used(p_fifo) >= 2)
			{
				fifo_s_gets(p_fifo,(char*)&adj_sum,1);	//����У���
				i = BT_RX_LENGTH - 3;	// 2021.7.7ע�ͣ����ģ�
				while(i -- )
					sum += btProcBuf[i];	//����У���
				fifo_s_gets(p_fifo,(char*)&end,1);	//����֡β
				if(sum == adj_sum && end == 0x5A)
					proc_step = 3;	//У���&֡β��ȷ������step4
				else
				{
					proc_step = 0;	//���󣬷���
					return;
				}
			}
			else
				return;
		}
		if(proc_step == 3)
		{
			proc_step = 0;	//������ɣ����½���
			bt_DataUnPack(btProcBuf);//��������
		}
	}
}


