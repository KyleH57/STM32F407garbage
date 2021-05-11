/*
 * spindle.c
 *
 *  Created on: Apr 4, 2021
 *      Author: kyleh
 */

#include "spindle.h"
#include "stm32f4xx_hal.h"

#include "main.h"

uint8_t wrMsg[] =
{ 0x01, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 };

uint8_t rx485[] =
{ 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A' }; //12 bytes

void sendData8(UART_HandleTypeDef *huart)
{
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 1);
	HAL_Delay(5);
	//blocking transmit 10ms timeout
	HAL_UART_Transmit(huart, wrMsg, 8, 10);
	HAL_Delay(5);
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 0);

}

void clearSer()
{
	//TODO
}

void appendCRC8()
{
	uint16_t crc = crc_chk_value(wrMsg, 6);

	//change to 8 bit pointer so we can separate the data
	uint8_t *crcValPtr = (uint8_t*) &crc;

	//write first byte
	wrMsg[6] = *crcValPtr;
	crcValPtr++;
	//write second byte
	wrMsg[7] = *crcValPtr;
}

//TODO
//Check to see if we need to receive 10 bytes or not

//returns 1 if success, 0 if fail
int checkEcho8(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive(huart, rx485, 8, 50);
	for (int i = 0; i < 8; i++)
	{
		if (wrMsg[i] != rx485[i])
		{
			return 1;
		}
	}
	return 0;
}

//TODO add CDC check
int readCurrent10X(UART_HandleTypeDef *huart)
{

	wrMsg[0] = 0x01;
	wrMsg[1] = 0x03;
	wrMsg[2] = 0xD0;
	wrMsg[3] = 0x01;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x01;

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();

	sendData8(huart);

	HAL_UART_Receive(huart, rx485, 7, 50);

	int16_t cData = rx485[3] & 0xFF00;

	cData = rx485[4] & 0xFF;

	return cData;
}

////TODO add CRC check
//int readRPM(UART_HandleTypeDef *huart)
//{
//
//	wrMsg[0] = 0x01;
//	wrMsg[1] = 0x03;
//	wrMsg[2] = 0xD0;
//	wrMsg[3] = 0x03;
//	wrMsg[4] = 0x00;
//	wrMsg[5] = 0x01;
//
//	//overwritten by append CRC
//	wrMsg[6] = 0x00;
//	wrMsg[7] = 0x00;
//
//	appendCRC8();
//
//	sendData8(huart);
//
//	HAL_UART_Receive(huart, rx485, 7, 50);
//
//	int16_t rpmData = (rx485[3] << 8) | rx485[4];
//
//	int16_t *rpmData2 = (int16_t *)(rx485[3]);
//
//	return *rpmData2;
//
//	//return rpmData;
//}


uint16_t rdStatusValue(UART_HandleTypeDef *huart, uint8_t statusID)
{
	uint16_t data;

	wrMsg[0] = 0x01;
	wrMsg[1] = 0x03;
	wrMsg[2] = 0xD0;
	wrMsg[3] = statusID;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x01;

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();

	sendData8(huart);

	HAL_UART_Receive(huart, rx485, 7, 50);

	data = rx485[4];
	data <<= 8;
	data |= rx485[5];
	//*data = (uint16_t *)(rx485[3]);

	//return *data;


	return data;

}   //end of rdStatusValue()


#define VFD_PARAM_CODE_OUTPUT_FREQ   0x00
#define VFD_PARAM_CODE_OUTPUT_I   0x01
#define VFD_PARAM_CODE_RPM   0x03


uint16_t readRPM(UART_HandleTypeDef *huart)
{
	uint16_t data;

	data = rdStatusValue(huart, VFD_PARAM_CODE_RPM);

	return data;
}


uint16_t readI(UART_HandleTypeDef *huart)
{
	uint16_t data;

	data = rdStatusValue(huart, VFD_PARAM_CODE_OUTPUT_I);

	return data;
}



//sends message to turn on spindle, returns 1 if success, 0 if failed
int spindleFWD(UART_HandleTypeDef *huart)
{

	wrMsg[0] = 0x01;
	wrMsg[1] = 0x06;
	wrMsg[2] = 0x10;
	wrMsg[3] = 0x01;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x01;

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();

	sendData8(huart);

	return checkEcho8(huart);
}

int spindleOff(UART_HandleTypeDef *huart)
{

	wrMsg[0] = 0x01;
	wrMsg[1] = 0x06;
	wrMsg[2] = 0x10;
	wrMsg[3] = 0x01;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x03;

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();

	sendData8(huart);

	return checkEcho8(huart);
}

int setFreq(uint16_t freq, UART_HandleTypeDef *huart)
{
	wrMsg[0] = 0x01;
	wrMsg[1] = 0x06;
	wrMsg[2] = 0x10;
	wrMsg[3] = 0x02;
	wrMsg[4] = (uint8_t) (freq >> 8);
	wrMsg[5] = (uint8_t) (freq & 0xFF);

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();

	sendData8(huart);

	return checkEcho8(huart);
}

unsigned int crc_chk_value(uint8_t *data_value, uint8_t length)
{
	uint16_t crc_value = 0xFFFF;
	int i;
	while (length--)
	{
		crc_value ^= *data_value++;
		for (i = 0; i < 8; i++)
		{
			if (crc_value & 0x0001)
				crc_value = (crc_value >> 1) ^ 0xA001;
			else
				crc_value = crc_value >> 1;
		}
	}
	return (crc_value);
}

uint8_t* getCheck()
{
	return rx485;
}

uint8_t* getWr()
{
	return wrMsg;
}

