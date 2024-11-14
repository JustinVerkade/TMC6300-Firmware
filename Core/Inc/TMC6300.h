/*
 * TMC6300.h
 *
 *  Created on: Nov 10, 2022
 *      Author: Justin
 */

#ifndef INC_TMC6300_H_
#define INC_TMC6300_H_

#include "main.h"
#include <stdint.h>

typedef struct TMC6300_t TMC6300_t;
struct TMC6300_t
{
	// sine commutation
	TIM_HandleTypeDef *commutation_timer;
	uint16_t commutation_channel_u;
	uint16_t commutation_channel_v;
	uint16_t commutation_channel_w;
	uint16_t commutation_compare_u;
	uint16_t commutation_compare_v;
	uint16_t commutation_compare_w;
	uint32_t commutation_arcseconds;

	// io control
	GPIO_TypeDef *standby_gpio;
	GPIO_TypeDef *diag_gpio;
	uint16_t standby_pin;
	uint16_t diag_pin;

	// driver state
	uint8_t enabled;
};

// init
void TMC6300_Init(TMC6300_t *me,
		TIM_HandleTypeDef *commutation_timer,
		uint16_t commutation_channel_u,
		uint16_t commutation_channel_v,
		uint16_t commutation_channel_w,
		GPIO_TypeDef *GPIOx_standby,
		uint16_t GPIOx_PIN_standby,
		GPIO_TypeDef *GPIOx_diag,
		uint16_t GPIOx_PIN_diag);

// change state
void TMC6300_Enable(TMC6300_t *me);
void TMC6300_Disable(TMC6300_t *me);

// pwm control
void TMC6300_SetPhaseAngle(TMC6300_t *me, uint32_t commutation_angle, uint16_t power);

// control tick
void TMC6300_Tick(TMC6300_t *me, uint8_t ms1, uint8_t ms2, int8_t direction);

// diag
void TMC6300_DiagCallback(TMC6300_t *me, uint16_t GPIO_Pin);

#endif /* INC_TMC6300_H_ */
