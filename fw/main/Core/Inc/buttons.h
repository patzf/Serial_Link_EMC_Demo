/*
 * buttons.h
 *
 *  Created on: May 11, 2026
 *      Author: Tommy
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include <stdint.h>

#define UART_RS232_INTERFACE 0
#define UART_RS485_INTERFACE 1
#define UART_3V3_INTERFACE 2
#define CAN_INTERFACE 3

#define SINGLE_SEND_MODE 0
#define CONTINUOUS_SEND_MODE 1

#define TRANSMIT 0
#define RECEIVE 1
#define RECEIVE_TRANSMIT 2

extern volatile uint8_t role; // the current role either TRANSMIT, RECEIVE or RECEIVE_TRANSMIT
extern volatile uint8_t sendMode; // the current send mode single or continuous
extern volatile uint8_t currInterface; // the current interface to use
extern volatile uint8_t running; // gets set by EXTI buttons
extern volatile uint8_t resetRNG; // gets set to 1 if the RNG should be reset, should be manually reset to 0 if completed

#endif /* INC_BUTTONS_H_ */
