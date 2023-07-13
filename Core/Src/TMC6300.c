/*
 * TMC6300.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Justin
 */

#include <math.h>

#include "TMC6300.h"
#include "TMC6300_commutation_table.h"

// init
void TMC6300_Init(TMC6300_t *me,
		TIM_HandleTypeDef *commutation_timer,
		uint16_t commutation_channel_u,
		uint16_t commutation_channel_v,
		uint16_t commutation_channel_w,
		GPIO_TypeDef *standby_gpio,
		uint16_t standby_pin,
		GPIO_TypeDef *diag_gpio,
		uint16_t diag_pin)
{
	// sine commutation
	me->commutation_timer = commutation_timer;
	me->commutation_channel_u = commutation_channel_u;
	me->commutation_channel_v = commutation_channel_v;
	me->commutation_channel_w = commutation_channel_w;
	me->commutation_compare_u = 0;
	me->commutation_compare_v = 0;
	me->commutation_compare_w = 0;
	me->commutation_arcseconds = 0;

	// io control
	me->standby_gpio = standby_gpio;
	me->diag_gpio = diag_gpio;
	me->standby_pin = standby_pin;
	me->diag_pin = diag_pin;

	// driver state
	me->enabled = 0;
}

// change state
void TMC6300_Enable(TMC6300_t *me)
{
	// update state
	HAL_GPIO_WritePin(me->standby_gpio, me->standby_pin, GPIO_PIN_SET);

	// enable pwm channels
	me->commutation_timer->Instance->CR2 &= ~(TIM_CR2_CCPC);
	HAL_TIM_Base_Start(me->commutation_timer);
//	HAL_TIM_PWM_Start(me->commutation_timer, me->commutation_channel_u);
//	HAL_TIMEx_PWMN_Start(me->commutation_timer, me->commutation_channel_u);
//	HAL_TIM_PWM_Start(me->commutation_timer, me->commutation_channel_v);
//	HAL_TIMEx_PWMN_Start(me->commutation_timer, me->commutation_channel_v);
	HAL_TIM_PWM_Start(me->commutation_timer, me->commutation_channel_w);
	HAL_TIMEx_PWMN_Start(me->commutation_timer, me->commutation_channel_w);

	// update state
	me->enabled = 1;
}

void TMC6300_Disable(TMC6300_t *me)
{
	// update state
	me->enabled = 0;

	// disable pwm channels
//	HAL_TIM_PWM_Stop(me->commutation_timer, me->commutation_channel_u);
//	HAL_TIMEx_PWMN_Stop(me->commutation_timer, me->commutation_channel_u);
//	HAL_TIM_PWM_Stop(me->commutation_timer, me->commutation_channel_v);
//	HAL_TIMEx_PWMN_Stop(me->commutation_timer, me->commutation_channel_v);
	HAL_TIM_PWM_Stop(me->commutation_timer, me->commutation_channel_w);
	HAL_TIMEx_PWMN_Stop(me->commutation_timer, me->commutation_channel_w);
	HAL_TIM_Base_Stop(me->commutation_timer);

	// update state
	HAL_GPIO_WritePin(me->standby_gpio, me->standby_pin, GPIO_PIN_RESET);
}

// pwm control
void TMC6300_SetPhaseAngle(TMC6300_t *me, uint32_t commutation_angle, uint16_t power)
{
	// convert to scalar indices
	int32_t angle = commutation_angle / 450;
	angle = angle < 0 ? 2880 - (-angle % 2880) : angle;

	// phases
	uint32_t sine_u = array[angle % 2880];
	uint32_t sine_v = array[(angle + 960) % 2880];
	uint32_t sine_w = array[(angle + 1920) % 2880];

	// pwm compare values
	me->commutation_compare_u = 200 * sine_u * power / 524288;
	me->commutation_compare_v = 200 * sine_v * power / 524288;
	me->commutation_compare_w = 200 * sine_w * power / 524288;

	// set compare values
//	me->commutation_timer->Instance->CCR1 = me->commutation_compare_u;
//	me->commutation_timer->Instance->CCR2 = me->commutation_compare_v;
	me->commutation_timer->Instance->CCR3 = power;//me->commutation_compare_w;
}

// control tick
void TMC6300_Tick(TMC6300_t *me, uint8_t ms1, uint8_t ms2, int8_t direction);

// diag
void TMC6300_DiagCallback(TMC6300_t *me, uint16_t GPIO_Pin);

