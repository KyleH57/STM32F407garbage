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

uint8_t rx485[50] =
{ 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A' }; //12 bytes


uint16_t altRPM = 0;
uint16_t altI = 0;

uint16_t packetCRC;
uint16_t rxCRC;

void sendData8(UART_HandleTypeDef *huart)
{

	while(HAL_UART_Receive(huart, rx485, 1, 1) != HAL_TIMEOUT)
	{
		//dump serial rx buffer of any leftover/unexpected data
	}


	//VFD runs at 9600 baud or 1.2KB/s --- 8 bytes takes ~10ms
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 1);
	HAL_Delay(5);

	//blocking transmit 15ms timeout
	HAL_UART_Transmit(huart, wrMsg, 8, 15);
	//HAL_Delay(5);
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 0);

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

//returns 1 if fail, 0 if success
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

//statCode 1 = CRC check failed
int rdStatusValue(UART_HandleTypeDef *huart, uint8_t statusID, uint16_t *data)
{

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

	//TODO check for rx errors
	//TODO dump buffer
	HAL_UART_Receive(huart, rx485, 7, 200);

	uint16_t rxCRC = crc_chk_value(rx485, 5);

	//kinda sus  R shift LSB????
	uint16_t packetCRC = rx485[6];
	packetCRC <<= 8;
	packetCRC |= rx485[5];

	if (packetCRC != rxCRC)
	{
		return 1;
	}


	*data = rx485[3];
	*data <<= 8;
	*data |= rx485[4];


	return 0;

}   //end of rdStatusValue()


#define VFD_PARAM_CODE_OUTPUT_FREQ   0x00
#define VFD_PARAM_CODE_OUTPUT_I   0x01
#define VFD_PARAM_CODE_RPM   0x03


int readRPM(UART_HandleTypeDef *huart, uint16_t *data)
{

	int statCode = rdStatusValue(huart, VFD_PARAM_CODE_RPM, data);

	return statCode;
}


int readI(UART_HandleTypeDef *huart, uint16_t *data)
{

	int statCode = rdStatusValue(huart, VFD_PARAM_CODE_OUTPUT_I, data);

	return statCode;
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

int masterRd(UART_HandleTypeDef *huart, struct SpindleData *spindle0)
{

	wrMsg[0] = 0x01;
	wrMsg[1] = 0x03;
	wrMsg[2] = 0xD0;
	wrMsg[3] = 0x01;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x03;

	//overwritten by append CRC
	wrMsg[6] = 0x00;
	wrMsg[7] = 0x00;

	appendCRC8();


	sendData8(huart);

	//TODO check for rx errors
	if(HAL_UART_Receive(huart, rx485, 11, 200) == HAL_TIMEOUT)
	{
		return HAL_TIMEOUT;
	}

	rxCRC = crc_chk_value(rx485, 9);


	//lmao its midnight and i realized endinenness is fuc ked
	packetCRC = rx485[10];
	packetCRC <<= 8;
	packetCRC |= rx485[9];

	if (packetCRC != rxCRC)
	{
		//error code 4 - CRC failed
		return 4;
	}


//	spindle0->current = rx485[3];
//	spindle0->current <<= 8;
//	spindle0->current |= rx485[4];

	spindle0->current = (rx485[3] << 8) | rx485[4];

	//TODO delete whichever method doesnt work
	altI = (rx485[3] << 8) | rx485[4];

	spindle0->voltage = rx485[5];
	spindle0->voltage <<= 8;
	spindle0->voltage |= rx485[6];

	spindle0->rpm = rx485[7];
	spindle0->rpm <<= 8;
	spindle0->rpm |= rx485[8];

	//TODO delete whichever method doesnt work
	altRPM = (rx485[7] << 8) | rx485[8];
	return 0;

}


uint16_t altGetI()
{
	return altI;
}
uint16_t altGetRPM()
{
	return altRPM;
}

uint16_t getPacketCRC()
{
	return packetCRC;
}

uint16_t getRxCRC()
{
	return rxCRC;
}
