/*
 * STM32modbus.h
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#ifndef SRC_STM32MODBUS_H_
#define SRC_STM32MODBUS_H_

//enum Status
//{
//	SUCCESS = 0,
//	ERROR_INVALID_PARAMETER,
//	ERROR_IO_ERROR,
//	ERROR_TIMEOUT,
//	MESSAGE_CORRUPT,
//	ERROR_UNKNOWN
//};


typedef struct
{
	int status;
	uint16_t elements[10];
} ModBusRdResult;

uint16_t modBusCRC(uint8_t *message, uint16_t length);

int modBusWrSingle(UART_HandleTypeDef *uart, uint8_t deviceAddr,
		uint16_t registerAddr, uint16_t data, uint32_t timeout,
		uint8_t maxRetries);



#endif /* SRC_STM32MODBUS_H_ */
