/*
 * spindle.h
 *
 *  Created on: Apr 4, 2021
 *      Author: kyleh
 */

#ifndef INC_SPINDLE_H_
#define INC_SPINDLE_H_

#include "stm32f4xx_hal.h"

void sendData8(UART_HandleTypeDef *huart);


void appendCRC8();

int checkEcho8();
//TODO
int readCurrent10X(UART_HandleTypeDef *huart);

int readRPM(UART_HandleTypeDef *huart, uint16_t *data);

int readI(UART_HandleTypeDef *huart, uint16_t *data);

//int readFreq(UART_HandleTypeDef *huart);

int rdStatusValue(UART_HandleTypeDef *huart, uint8_t statusID, uint16_t *data);

int spindleFWD();

int spindleOff();

int setFreq(uint16_t freq, UART_HandleTypeDef *huart);

unsigned int crc_chk_value(uint8_t *data_value, uint8_t length);

uint8_t* getCheck();
uint8_t* getWr();
#endif /* INC_SPINDLE_H_ */
