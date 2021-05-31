/*
 * spindle.h
 *
 *  Created on: Apr 4, 2021
 *      Author: kyleh
 */

#ifndef INC_SPINDLE_H_
#define INC_SPINDLE_H_

#include "stm32f4xx_hal.h"

typedef struct SpindleData
	{
		uint16_t current;
		uint16_t voltage;
		uint16_t rpm;
	};


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

int masterRd(UART_HandleTypeDef *huart, struct SpindleData *spindle0);

uint16_t altGetI();
uint16_t altGetRPM();

uint16_t getPacketCRC();
uint16_t getRxCRC();
#endif /* INC_SPINDLE_H_ */
