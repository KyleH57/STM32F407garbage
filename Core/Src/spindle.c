/*
 * spindle.c
 *
 *  Created on: Apr 4, 2021
 *      Author: kyleh
 */

#include "spindle.h"
#include "stm32f4xx_hal.h"

#include "main.h"

uint8_t wrMsg[] = { 0x01, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 };

uint8_t check[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void sendData8(UART_HandleTypeDef *huart) {
	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 1);
	//blocking transmit 10ms timeout
	HAL_UART_Transmit(&huart3, wrMsg, 8, 10);

	HAL_GPIO_WritePin(RS485EN_GPIO_Port, RS485EN_Pin, 0);

}

void clearSer() {
	//TODO
}

void appendCRC8() {
	uint16_t crc = crc_chk_value(wrMsg, 6);

	//change to 8 bit pointer so we can separate the data
	uint8_t *crcValPtr = (uint8_t*) &crc;

	//write first byte
	wrMsg[6] = *crcValPtr;
	crcValPtr++;
	//write second byte
	wrMsg[7] = *crcValPtr;
}

void checkEcho8(UART_HandleTypeDef *huart) {
//  Serial.print("ESP32: BEGINING ECHO CHECK - ");
//  Serial1.readBytes(check, 8);
//  for (int i = 0; i < 8; i++)
//  {
//    Serial.print(check[i], HEX);
//    Serial.print(" ");
//    // if (check[i] != wrMsg[i])
//    // {
//    //   Serial.println("-ESP32: ECHO CHECK FAILED");
//    //   //cmdError = true;
//    //   return;
//    // }
//  }
//  Serial.println("-ESP32: ECHO CHECK COMPLETE");
}
//TODO
int readCurrent10X(UART_HandleTypeDef *huart) {
//  //should be const
//  uint8_t readMsg[] = {0x01, 0x03, 0xD0, 0x01, 0x00, 0x01, 0x00, 0x00};
//
//  appendCRC8(readMsg);
//
//  sendData8(readMsg);
//
//  uint8_t byteBuffer[7];
//  Serial1.readBytes(byteBuffer, 7);
//
//  uint16_t curData = byteBuffer[3];
//
//  curData <<= 8;
//
//  curData |= byteBuffer[4];
//
//  return curData;
}

void spindleFWD(UART_HandleTypeDef *huart) {
	wrMsg[0] = 0x01;
	wrMsg[1] = 0x03;

	wrMsg[2] = 0x10;
	wrMsg[3] = 0x01;
	wrMsg[4] = 0x00;
	wrMsg[5] = 0x01;

	appendCRC8(wrMsg);

	sendData8(wrMsg);
	Serial.println("ESP32: SENT M3\n");
	checkEcho8();
}

void spindleOff(UART_HandleTypeDef *huart) {
//  wrMsg[2] = 0x10;
//  wrMsg[3] = 0x01;
//  wrMsg[4] = 0x00;
//  wrMsg[5] = 0x03;
//
//  appendCRC8(wrMsg);
//
//  sendData8(wrMsg);
//
//  Serial.println("ESP32: SENT M5\n");
//  checkEcho8();
}

void setFreq(uint16_t freq, UART_HandleTypeDef *huart) {
//  wrMsg[2] = 0x10;
//  wrMsg[3] = 0x02;
//
//  wrMsg[4] = (uint8_t)(freq >> 8);
//  wrMsg[5] = (uint8_t)(freq & 0xFF);
//
//  appendCRC8(wrMsg);
//
//  sendData8(wrMsg);
//
//  Serial.println("ESP32: SENT S\n");
//  checkEcho8();
}

unsigned int crc_chk_value(uint8_t *data_value, uint8_t length) {
	uint16_t crc_value = 0xFFFF;
	int i;
	while (length--) {
		crc_value ^= *data_value++;
		for (i = 0; i < 8; i++) {
			if (crc_value & 0x0001)
				crc_value = (crc_value >> 1) ^ 0xA001;
			else
				crc_value = crc_value >> 1;
		}
	}
	return (crc_value);
}
