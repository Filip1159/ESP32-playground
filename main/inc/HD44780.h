#pragma once

#include "driver/gpio.h"
#include "stdint.h"

class HD44780 {
private:
	void writeUpper(uint8_t command);
	void writeLower(uint8_t command);
	void command(uint8_t cmonmad);
	const gpio_num_t rsPin, ePin, d4Pin, d5Pin, d6Pin, d7Pin; 

public:
	HD44780(gpio_num_t rsPin, gpio_num_t enPin, gpio_num_t d4Pin, gpio_num_t d5Pin, gpio_num_t d6Pin, gpio_num_t d7Pin);
	void setCursor(uint8_t row, uint8_t column);
	void writeChar(char c);
	void writeString(const char* s);
	void clear();
};