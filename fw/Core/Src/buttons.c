/*
 * buttons.c
 *
 *  Created on: May 11, 2026
 *      Author: Tommy
 */

#include <main.h>
#include <tim.h>
#include <buttons.h>

volatile uint8_t role = TRANSMIT;
volatile uint8_t sendMode = CONTINUOUS_SEND_MODE;
volatile uint8_t currInterface = UART_3V3_INTERFACE;
volatile uint8_t running = 0;
volatile uint8_t resetRNG = 0;

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case START_Pin:
		htim6.Instance->CNT = 0;
		HAL_TIM_Base_Start(&htim6);
		break;
	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case START_Pin:
		HAL_TIM_Base_Stop(&htim6);
		if (htim6.Instance->CNT >= 10000) // if held for 5 sec
			resetRNG = 1; // set the reset RNG signal
		else
			running = !running; // toggle running
		break;
	case CONT_Pin:
		sendMode = (sendMode + 1) % 2;
		break;
	case INTERFACE_Pin:
		currInterface = (currInterface + 1) % 4;
		break;
	case MODE_Pin:
		role = (role + 1) % 3;
		break;
	}
}
