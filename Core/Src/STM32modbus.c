/*
 * modbus.c
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#include "stm32f4xx_hal.h"
#include "main.h"

//enum Status
//{
//	//SUCCESS = 0,
//	ERROR_INVALID_PARAMETER,
//	ERROR_IO_ERROR,
//	MODBUS_TIMEOUT,
//	MESSAGE_CORRUPT,
//	ERROR_UNKNOWN
//};

#define MODBUS_TX_TIMEOUT 50
#define MODBUS_RX_TIMEOUT 300

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

/* Table of CRC values for high-order byte */
static unsigned char table_crc_hi[] =
{ 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01,
		0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
		0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00,
		0xC1, 0x81, 0x40 };

/* Table of CRC values for low-order byte */
static unsigned char table_crc_lo[] =
{ 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05,
		0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A,
		0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B,
		0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14,
		0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11,
		0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36,
		0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF,
		0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28,
		0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D,
		0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22,
		0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63,
		0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C,
		0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69,
		0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE,
		0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77,
		0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50,
		0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55,
		0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A,
		0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B,
		0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 0x44,
		0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41,
		0x81, 0x80, 0x40 };

static uint8_t modbusCRC[2] =
{ 0 };

static void crc16(unsigned char *buffer, unsigned short buffer_length)
{
	unsigned char crc_hi = 0xFF; /* high CRC byte initialized */
	unsigned char crc_lo = 0xFF; /* low CRC byte initialized */
	unsigned int i; /* will index into CRC lookup */

	/* pass through message buffer */
	while (buffer_length--)
	{
		i = crc_hi ^ *buffer++; /* calculate the CRC  */
		crc_hi = crc_lo ^ table_crc_hi[i];
		crc_lo = table_crc_lo[i];
	}
//
//	modbusCRC[1] = crc_hi;
//	modbusCRC[0] = crc_lo;

	modbusCRC[0] = crc_hi;
	modbusCRC[1] = crc_lo;
}

int modBusTransmit(UART_HandleTypeDef *uart, uint8_t *data, uint32_t timeout)
{
	uint8_t uselessData[100];
	while (HAL_UART_Receive(uart, uselessData, 1, 1) != HAL_TIMEOUT)
	{
		//dump serial rx buffer of any leftover/unexpected data
	}

	//VFD runs at 19200 baud or 1.2KB/s --- 8 bytes takes ~5ms
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 1);
	HAL_Delay(1);
	//blocking transmit 15ms timeout
	HAL_UART_Transmit(uart, data, 8, 15);
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 0);
	HAL_Delay(1);
	return 0;
}

/*
 * returns 0 if no errors

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

	uint16_t crc = 0;

	// Calculate the 16-bit Modbus RTU CRC for the message
	//crc = modBusCRC(message, 6);

	// Append the CRC to the message
	//might be broken
	//message[7] = (crc >> 8) & 0xFF;
	//message[6] = crc & 0xFF;

	// Calculate the 16-bit Modbus RTU CRC for the message
	crc16(message, 6);

	// Append the CRC to the message
	//might be broken
	//message[7] = (crc >> 8) & 0xFF;
	//message[6] = crc & 0xFF;

	//Lut version
	message[6] = modbusCRC[0];
	message[7] = modbusCRC[1];

	uint8_t rxMsg[8];

//	int success = 0;
//	int numTries = 0;
//	do
//	{
//		numTries++;
//		// Send the message over the UART
//		modBusTransmit(uart, message, MODBUS_TIMEOUT);
//
//		//TODO
//		//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
//		HAL_Delay(1000);
//
//		// Wait for the response message from the slave device
//		if (HAL_UART_Receive(uart, rxMsg, 8, timeout) != HAL_OK)
//		{
//			return MODBUS_TIMEOUT;
//		}
//	} while (!success || numTries <= maxRetries);

	int messageCorrupt = 0;

	for (int i = 0; i < maxRetries; ++i)
	{
		modBusTransmit(uart, message, MODBUS_TX_TIMEOUT);
		HAL_UART_Receive(uart, rxMsg, 8, MODBUS_RX_TIMEOUT);

		for (int j = 0; j < 8; ++j)
		{
			if (message[j] != rxMsg[j])
			{
				//Error!
				messageCorrupt = 1;
				break;
			}
		}

		if (!messageCorrupt) {
			return 0;
		}

	}

	return 1;
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

