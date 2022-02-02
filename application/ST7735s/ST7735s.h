#pragma once

#define XSIZE 128
#define YSIZE 160
#define XMAX XSIZE-1
#define YMAX YSIZE-1

#include "stdint.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "Color.h"
#include "graphics/Canvas.h"
#include "graphics/Printer.h"


enum Command {
	SoftwareReset = 0x01,
	SleepOut = 0x11,
	DisplayOff = 0x29,
	DisplayOn = 0x29,
	ColumnAddressSet = 0x2A,
	RowAddressSet = 0x2B,
	RamWrite = 0x2C,
	MADCTL = 0x36,
	ColorMode = 0x3A
};

class ST7735s {
private:
	uint8_t cursorX, cursorY;
	static const uint8_t FONT_CHARS[96][5];
	gpio_num_t sclPin, sdaPin, dcPin, resPin;
	spi_device_handle_t spi;
	spi_bus_config_t buscfg;
	spi_device_interface_config_t devcfg;

	Canvas* canvas;
	Printer* printer;

public:
	ST7735s(gpio_num_t sclPin, gpio_num_t sdaPin, gpio_num_t dcPin, gpio_num_t resPin);
	~ST7735s();

	Canvas* getCanvas();
	Printer* getPrinter();

	void sendCommand(Command command);
	void sendData(uint8_t data);
	void sendData16Bit(uint16_t data);
	void fillColor(Color color, unsigned int count);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

	void init();
	void clearScreen();
	void setOrientation(int degrees);
};