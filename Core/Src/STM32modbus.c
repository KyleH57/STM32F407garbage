/*
 * modbus.c
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#include "stm32f4xx_hal.h"
#include "main.h"

enum Status
{
	//SUCCESS = 0,
	ERROR_INVALID_PARAMETER,
	ERROR_IO_ERROR,
	MODBUS_TIMEOUT,
	MESSAGE_CORRUPT,
	ERROR_UNKNOWN
};

uint16_t modBusCRC(uint8_t *message, uint16_t length)
{
	uint16_t crc = 0xFFFF;

	for (int i = 0; i < length; i++)
	{
		crc ^= message[i];

		for (int j = 0; j < 8; j++)
		{
			if (crc & 0x0001)
			{
				crc = (crc >> 1) ^ 0xA001;
			}
			else
			{
				crc = crc >> 1;
			}
		}
	}

	return crc;
}

int modBusTransmit(UART_HandleTypeDef *uart, uint8_t *data, uint32_t timeout)
{
	uint8_t uselessData[100];
	while(HAL_UART_Receive(uart, uselessData, 1, 1) != HAL_TIMEOUT)
	{
		//dump serial rx buffer of any leftover/unexpected data
	}


	//VFD runs at 19200 baud or 1.2KB/s --- 8 bytes takes ~5ms
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 1);
	HAL_Delay(1);
	//blocking transmit 15ms timeout
	HAL_UART_Transmit(uart, data, 8, 15);
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 0);

	return 0;
}


/*
 * test doc
 * uart*
 * deviceAddr
 */
int modBusWrSingle(UART_HandleTypeDef *uart, uint8_t deviceAddr,
		uint16_t registerAddr, uint16_t data, uint32_t timeout,
		uint8_t maxRetries)
{
	// Create a buffer to hold the Modbus message
	uint8_t message[8];

	// Set the Modbus function code for a single register write
	message[0] = deviceAddr;
	message[1] = 0x06;

	// Encode the register address and data in big-endian format
	message[2] = (registerAddr >> 8) & 0xFF;
	message[3] = registerAddr & 0xFF;
	message[4] = (data >> 8) & 0xFF;
	message[5] = data & 0xFF;

	// Calculate the 16-bit Modbus RTU CRC for the message
	uint16_t crc = modBusCRC(message, 6);

	// Append the CRC to the message
	//might be broken
	message[7] = (crc >> 8) & 0xFF;
	message[6] = crc & 0xFF;

//	message[0] = 0x01;
//	message[1] = 0x01;
//	message[2] = 0x01;
//	message[3] = 0x01;
//	message[4] = 0x01;
//	message[5] = 0x01;

	uint8_t rxMsg[8];

	int success = 0;
	int numTries = 0;
	do
	{
		numTries++;
		// Send the message over the UART
		modBusTransmit(uart, message, MODBUS_TIMEOUT);

		// Wait for the response message from the slave device
		if (HAL_UART_Receive(uart, rxMsg, 8, timeout) != HAL_OK)
		{
			return MODBUS_TIMEOUT;
		}
	} while (!success || numTries <= maxRetries);

	return 0;
}

typedef struct
{
	int status;
	uint16_t elements[10];
} ModBusRdResult;

//int modBusRd(UART_HandleTypeDef *uart, ModBusRdResult *dataStruct,
//		uint8_t deviceAddr, uint16_t startAddr, uint8_t numRegisters,
//		uint32_t timeout, uint8_t maxRetries)
//{
//	// Validate the input parameters
//	if (dataStruct == NULL)
//	{
//		// Invalid data struct pointer
//		return -1;
//	}
//	if (numRegisters > 10)
//	{
//		// Cannot read more than 10 registers
//		return -2;
//	}
//
//	// Try to read the registers up to the maximum number of retries
//	for (int i = 0; i < maxRetries; i++)
//	{
//		// Create a buffer to hold the Modbus message
//		uint8_t message[8];
//
//		// Set the Modbus function code for multiple registers read
//		message[0] = deviceAddr;
//		message[1] = 0x03;
//
//		// Encode the start address and number of registers in big-endian format
//		message[2] = (startAddr >> 8) & 0xFF;
//		message[3] = startAddr & 0xFF;
//		message[4] = (numRegisters >> 8) & 0xFF;
//		message[5] = numRegisters & 0xFF;
//
//		// Calculate the 16-bit Modbus RTU CRC for the message
//		uint16_t crc = modBusCRC(message, 6);
//
//		// Append the CRC to the message
//		message[6] = (crc >> 8) & 0xFF;
//		message[7] = crc & 0xFF;
//
//		// Send the message to the slave device
//		if (HAL_UART_Transmit(uart, message, sizeof(message), timeout)
//				!= HAL_OK)
//		{
//			// Failed to transmit the message
//			continue;
//		}
//
//		// Receive the response from the slave device
//		uint8_t response[256];
//		uint16_t responseLength = sizeof(response);
//		if (HAL_UART_Receive(uart, response, &responseLength, timeout)
//				!= HAL_OK)
//		{
//			// Failed to receive the response
//			continue;
//		}
//
//		// Check the response for errors
//		if (response[0] != deviceAddr)
//		{
//			// Invalid device address
//			continue;
//		}
//		if (response[1] != 0x03)
//		{
//			// Invalid function code
//			continue;
//		}
//		if (response[2] != numRegisters * 2)
//		{
//			// Invalid number of data bytes
//			continue;
//		}
//
//		crc = modBusCRC(response, responseLength - 2);
//
//		uint16_t expectedCRC = (response[responseLength - 2] << 8) | response[responseLength - 1];
//
//		if (crc == expectedCRC)
//		{
//			dataStruct->elements =
//			return SUCCESS;
//		}
//	}
//
//	return -1;
//}


