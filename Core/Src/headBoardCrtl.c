/*
 * headBoardCrtl.c
 *
 *  Created on: Dec 30, 2022
 *      Author: kyleh
 */

#include "stm32f4xx_hal.h"
#include "STM32modbus.h"
#include "headBoardCtrl.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

enum Stuff
{
	DEVICE_ADDRESS = 0x02,
	SOLENOID_ON = 0x01,
	SOLENOID_OFF = 0x00,
	NUM_RETRIES = 3,
	TIMEOUT = 200,
	Z_AXIS_SOLENOID_REGISTER_ADDR = 0x03,
	SPINDLE_DRAWBAR_REGISTER_ADDR = 0x00,
	COOLANT_REGISTER_ADDR = 0x02
};

int set_headboard_solenoid_state(UART_HandleTypeDef *uart, uint16_t registerAddr, uint8_t state)
{
	modBusWrSingle(uart, DEVICE_ADDRESS, registerAddr, state, TIMEOUT, NUM_RETRIES);
	return 0;
}

int unlock_Z_axis(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, Z_AXIS_SOLENOID_REGISTER_ADDR, SOLENOID_ON, TIMEOUT, NUM_RETRIES);
}

int lock_Z_axis(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, Z_AXIS_SOLENOID_REGISTER_ADDR, SOLENOID_OFF, TIMEOUT, NUM_RETRIES);
}

int clamp_tool(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, SPINDLE_DRAWBAR_REGISTER_ADDR, SOLENOID_OFF, TIMEOUT, NUM_RETRIES);
}

int release_tool(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, SPINDLE_DRAWBAR_REGISTER_ADDR, SOLENOID_ON, TIMEOUT, NUM_RETRIES);
}

int coolant_on(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, COOLANT_REGISTER_ADDR, SOLENOID_ON, TIMEOUT, NUM_RETRIES);
}

int coolant_off(UART_HandleTypeDef *uart)
{
	return modBusWrSingle(uart, DEVICE_ADDRESS, COOLANT_REGISTER_ADDR, SOLENOID_OFF, TIMEOUT, NUM_RETRIES);
}


