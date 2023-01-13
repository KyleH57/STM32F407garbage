/*
 * H100Spindle.c
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#include "stm32f4xx_hal.h"

#include "STM32modbus.h"
#include "H100Spindle.h"

enum H100Consts
{
	DEVICE_ADDRESS = 0x01, TIMEOUT = 1000, NUM_RETRIES = 3
};

int H100spindleFWD(UART_HandleTypeDef *uart)
{
	//modBusWrSingle(uart, DEVICE_ADDRESS, 0x122, 0x01, TIMEOUT, NUM_RETRIES);

	//TODO see if 0x8000 works
	modBusWrSingle(uart, DEVICE_ADDRESS, 0x8122, 0x01, TIMEOUT, NUM_RETRIES);

	return 0;
}

int H100spindleOFF(UART_HandleTypeDef *uart)
{
	//modBusWrSingle(uart, DEVICE_ADDRESS, 0x122, 0x10, TIMEOUT, NUM_RETRIES);

	modBusWrSingle(uart, DEVICE_ADDRESS, 0x8122, 0x10, TIMEOUT, NUM_RETRIES);

	return 0;
}

int H100SetRPM(UART_HandleTypeDef *uart, int RPM)
{

	/*
	 * The data field controls the % of max frequency
	 * For example, 0x9C40 = 40,000 = 40% = 160Hz for a 400Hz motor and 320Hz for a 800Hz motor
	 */
	float percentRPM = RPM / 24000.0; //max RPM = 24000
	uint16_t RPM_data = (int)(percentRPM * 10000); //convert to H100 format

	//uint8_t testBuff[5];
	//testBuff[0] = RPM_data >> 8;..
	//CDC_Transmit_FS(testBuff, 2);

	modBusWrSingle(uart, DEVICE_ADDRESS, 0x8121, RPM_data, TIMEOUT, NUM_RETRIES);

	return 0;
}
