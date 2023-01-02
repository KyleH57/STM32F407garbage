/*
 * H100Spindle.h
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#ifndef INC_H100SPINDLE_H_
#define INC_H100SPINDLE_H_

#include "stm32f4xx_hal.h"


//const uint8_t DEVICE_ADDRESS = 0x01;
//const uint32_t TIMEOUT = 1000;
//const uint8_t NUM_RETRIES = 3;


int H100spindleFWD(UART_HandleTypeDef *uart);
int H100spindleOFF(UART_HandleTypeDef *uart);

int H100SetRPM(UART_HandleTypeDef *uart, int RPM);


#endif /* INC_H100SPINDLE_H_ */
