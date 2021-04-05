/*
 * spindle.h
 *
 *  Created on: Apr 4, 2021
 *      Author: kyleh
 */

#ifndef INC_SPINDLE_H_
#define INC_SPINDLE_H_



void sendData8(uint8_t *data);
}

void clearSer();

void appendCRC8(uint8_t *array);

void checkEcho8();
//TODO
int readCurrent10X();

void spindleFWD();

void spindleOff();
void setFreq(uint16_t freq);


unsigned int crc_chk_value(uint8_t *data_value, uint8_t length);


#endif /* INC_SPINDLE_H_ */
