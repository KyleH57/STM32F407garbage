/*
 * headboardCtrl.h
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#ifndef INC_HEADBOARDCTRL_H_
#define INC_HEADBOARDCTRL_H_

int set_headboard_solenoid_state(UART_HandleTypeDef *uart, uint8_t state);
int unlock_Z_axis(UART_HandleTypeDef *uart);
int lock_Z_axis(UART_HandleTypeDef *uart);

#endif /* INC_HEADBOARDCTRL_H_ */