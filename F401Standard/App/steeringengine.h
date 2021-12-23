#ifndef __STEERINGENINE_H__
#define __STEERINGENINE_H__

#include "main.h"

typedef struct
{
	float coordinate;
	volatile int32_t  PWM;
}M1TypeDef,M2TypeDef;

void MotorMove(int32_t pwm1,int32_t pwm2);
void User_PWM_Set1(int32_t v);
void User_PWM_Set2(int32_t v);


#endif
