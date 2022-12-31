/*
 * H100Spindle.c
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#include "stm32f4xx_hal.h"

#include "STM32modbus.h"

enum H100Consts
{
	DEVICE_ADDRESS = 0x01,
	TIMEOUT = 1000,
	NUM_RETRIES = 3
};



int H100spindleFWD(UART_HandleTypeDef *uart)
{
	modBusWrSingle(uart, DEVICE_ADDRESS, 0x122, 0x01, TIMEOUT, NUM_RETRIES);

	//TODO see if 0x8000 works
	//modBusWrSingle(uart, DEVICE_ADDRESS, 0x8122, 0x01, TIMEOUT, NUM_RETRIES);

	return 0;
}

int H100spindleOFF(UART_HandleTypeDef *uart)
{
	modBusWrSingle(uart, DEVICE_ADDRESS, 0x122, 0x10, TIMEOUT, NUM_RETRIES);

	//modBusWrSingle(uart, DEVICE_ADDRESS, 0x8122, 0x10, TIMEOUT, NUM_RETRIES);

	return 0;
}

int H100SetFreq(UART_HandleTypeDef *uart)
{
	//TODO
	modBusWrSingle(uart, DEVICE_ADDRESS, 0x120, 0x9C40, TIMEOUT, NUM_RETRIES);

	//modBusWrSingle(uart, DEVICE_ADDRESS, 0x8120, 0x9C40, TIMEOUT, NUM_RETRIES);

	return 0;
}
