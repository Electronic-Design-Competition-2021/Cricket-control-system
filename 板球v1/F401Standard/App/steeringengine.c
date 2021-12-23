#include "steeringengine.h"
#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "bsp_pwm.h"

void User_PWM_Set1(int32_t v)
{
	if(v >= PWM_MAX_1) v = PWM_MAX_1;
	if(v <= PWM_MIN_1) v = PWM_MIN_1;
	
	TIM4->CCR1 = v;
}

void User_PWM_Set2(int32_t v)
{
	if(v >= PWM_MAX_1) v = PWM_MAX_1;
	if(v <= PWM_MIN_1) v = PWM_MIN_1;
	
	TIM4->CCR2 = v;
}

void MotorMove(int32_t pwm1,int32_t pwm2)//¶æ»úµ×²ãÇý¶¯
{	
		User_PWM_Set1(1200+pwm1);
	
		User_PWM_Set2(1200-pwm2);
}

