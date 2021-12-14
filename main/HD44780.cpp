#include "inc/HD44780.h"
#include "inc/main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"

void HD44780::writeUpper(uint8_t command) {
	gpio_set_level(d4Pin, (command >> 4) & 1);
	gpio_set_level(d5Pin, (command >> 5) & 1);
	gpio_set_level(d6Pin, (command >> 6) & 1);
	gpio_set_level(d7Pin, (command >> 7) & 1);
}

void HD44780::writeLower(uint8_t command) {
	gpio_set_level(d4Pin, command & 1);
	gpio_set_level(d5Pin, (command >> 1) & 1);
	gpio_set_level(d6Pin, (command >> 2) & 1);
	gpio_set_level(d7Pin, (command >> 3) & 1);
}

HD44780::HD44780(gpio_num_t rsPin, gpio_num_t ePin, gpio_num_t d4Pin, gpio_num_t d5Pin, gpio_num_t d6Pin, gpio_num_t d7Pin) : rsPin(rsPin), ePin(ePin), d4Pin(d4Pin), d5Pin(d5Pin), d6Pin(d6Pin), d7Pin(d7Pin) {
	gpio_config_t config;
    memset(&config, 0, sizeof(gpio_config_t));
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (1ULL << rsPin) | (1ULL << ePin) | (1ULL << d4Pin) | (1ULL << d5Pin) | (1ULL << d6Pin) | (1ULL << d7Pin);
    gpio_config(&config);

	vTaskDelay(pdMS_TO_TICKS(20));  // always >15ms

	command(0x33);
	command(0x32);	// 4 bit initialization
	command(0x28);	// 2 line, 5*7 matrix in 4-bit mode
	command(0x0c);	// display on cursor off
	command(0x06);	// increment cursor
	command(0x01);	// Clear screen
}

void HD44780::command(uint8_t command) {
	gpio_set_level(rsPin, 0);
    ets_delay_us(100);
	gpio_set_level(ePin, 1);
    writeUpper(command);
    ets_delay_us(100);
	gpio_set_level(ePin, 0);

    gpio_set_level(rsPin, 0);
    ets_delay_us(100);
	gpio_set_level(ePin, 1);
    writeLower(command);
    ets_delay_us(100);
	gpio_set_level(ePin, 0);
}

void HD44780::setCursor(uint8_t row, uint8_t column) {
	if ((row == 0 || row == 1) && column <= 15) {
		command(column | (row == 0 ? 0x80 : 0xC0));
	}
}

void HD44780::writeChar(char c) {
	gpio_set_level(rsPin, 1);
    ets_delay_us(100);
	gpio_set_level(ePin, 1);
    writeUpper(c);
    ets_delay_us(100);
	gpio_set_level(ePin, 0);

    gpio_set_level(rsPin, 1);
    ets_delay_us(100);
	gpio_set_level(ePin, 1);
    writeLower(c);
    ets_delay_us(100);
	gpio_set_level(ePin, 0);
}

void HD44780::writeString(const char* s) {
	for (int i=0; s[i]; i++) {
		writeChar(s[i]);
	}
}

void HD44780::clear() {
	command(0x01);  // clear display
	vTaskDelay(pdMS_TO_TICKS(2));
	command(0x80);  // cursor at home position
}