#include "ST7735s.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"

#include "EspException.h"
#include <stdexcept>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace std;

#define $(i) to_string(i)

const uint8_t ST7735s::FONT_CHARS[96][5] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 }, // (space)
	{ 0x00, 0x00, 0x5F, 0x00, 0x00 }, // !
	{ 0x00, 0x07, 0x00, 0x07, 0x00 }, // "
	{ 0x14, 0x7F, 0x14, 0x7F, 0x14 }, // #
	{ 0x24, 0x2A, 0x7F, 0x2A, 0x12 }, // $
	{ 0x23, 0x13, 0x08, 0x64, 0x62 }, // %
	{ 0x36, 0x49, 0x55, 0x22, 0x50 }, // &
	{ 0x00, 0x05, 0x03, 0x00, 0x00 }, // '
	{ 0x00, 0x1C, 0x22, 0x41, 0x00 }, // (
	{ 0x00, 0x41, 0x22, 0x1C, 0x00 }, // )
	{ 0x08, 0x2A, 0x1C, 0x2A, 0x08 }, // *
	{ 0x08, 0x08, 0x3E, 0x08, 0x08 }, // +
	{ 0x00, 0x50, 0x30, 0x00, 0x00 }, // ,
	{ 0x08, 0x08, 0x08, 0x08, 0x08 }, // -
	{ 0x00, 0x60, 0x60, 0x00, 0x00 }, // .
	{ 0x20, 0x10, 0x08, 0x04, 0x02 }, // /
	{ 0x3E, 0x51, 0x49, 0x45, 0x3E }, // 0
	{ 0x00, 0x42, 0x7F, 0x40, 0x00 }, // 1
	{ 0x42, 0x61, 0x51, 0x49, 0x46 }, // 2
	{ 0x21, 0x41, 0x45, 0x4B, 0x31 }, // 3
	{ 0x18, 0x14, 0x12, 0x7F, 0x10 }, // 4
	{ 0x27, 0x45, 0x45, 0x45, 0x39 }, // 5
	{ 0x3C, 0x4A, 0x49, 0x49, 0x30 }, // 6
	{ 0x01, 0x71, 0x09, 0x05, 0x03 }, // 7
	{ 0x36, 0x49, 0x49, 0x49, 0x36 }, // 8
	{ 0x06, 0x49, 0x49, 0x29, 0x1E }, // 9
	{ 0x00, 0x36, 0x36, 0x00, 0x00 }, // :
	{ 0x00, 0x56, 0x36, 0x00, 0x00 }, // ;
	{ 0x00, 0x08, 0x14, 0x22, 0x41 }, // <
	{ 0x14, 0x14, 0x14, 0x14, 0x14 }, // =
	{ 0x41, 0x22, 0x14, 0x08, 0x00 }, // >
	{ 0x02, 0x01, 0x51, 0x09, 0x06 }, // ?
	{ 0x32, 0x49, 0x79, 0x41, 0x3E }, // @
	{ 0x7E, 0x11, 0x11, 0x11, 0x7E }, // A
	{ 0x7F, 0x49, 0x49, 0x49, 0x36 }, // B
	{ 0x3E, 0x41, 0x41, 0x41, 0x22 }, // C
	{ 0x7F, 0x41, 0x41, 0x22, 0x1C }, // D
	{ 0x7F, 0x49, 0x49, 0x49, 0x41 }, // E
	{ 0x7F, 0x09, 0x09, 0x01, 0x01 }, // F
	{ 0x3E, 0x41, 0x41, 0x51, 0x32 }, // G
	{ 0x7F, 0x08, 0x08, 0x08, 0x7F }, // H
	{ 0x00, 0x41, 0x7F, 0x41, 0x00 }, // I
	{ 0x20, 0x40, 0x41, 0x3F, 0x01 }, // J
	{ 0x7F, 0x08, 0x14, 0x22, 0x41 }, // K
	{ 0x7F, 0x40, 0x40, 0x40, 0x40 }, // L
	{ 0x7F, 0x02, 0x04, 0x02, 0x7F }, // M
	{ 0x7F, 0x04, 0x08, 0x10, 0x7F }, // N
	{ 0x3E, 0x41, 0x41, 0x41, 0x3E }, // O
	{ 0x7F, 0x09, 0x09, 0x09, 0x06 }, // P
	{ 0x3E, 0x41, 0x51, 0x21, 0x5E }, // Q
	{ 0x7F, 0x09, 0x19, 0x29, 0x46 }, // R
	{ 0x46, 0x49, 0x49, 0x49, 0x31 }, // S
	{ 0x01, 0x01, 0x7F, 0x01, 0x01 }, // T
	{ 0x3F, 0x40, 0x40, 0x40, 0x3F }, // U
	{ 0x1F, 0x20, 0x40, 0x20, 0x1F }, // V
	{ 0x7F, 0x20, 0x18, 0x20, 0x7F }, // W
	{ 0x63, 0x14, 0x08, 0x14, 0x63 }, // X
	{ 0x03, 0x04, 0x78, 0x04, 0x03 }, // Y
	{ 0x61, 0x51, 0x49, 0x45, 0x43 }, // Z
	{ 0x00, 0x00, 0x7F, 0x41, 0x41 }, // [
	{ 0x02, 0x04, 0x08, 0x10, 0x20 }, // "\"
	{ 0x41, 0x41, 0x7F, 0x00, 0x00 }, // ]
	{ 0x04, 0x02, 0x01, 0x02, 0x04 }, // ^
	{ 0x40, 0x40, 0x40, 0x40, 0x40 }, // _
	{ 0x00, 0x01, 0x02, 0x04, 0x00 }, // `
	{ 0x20, 0x54, 0x54, 0x54, 0x78 }, // a
	{ 0x7F, 0x48, 0x44, 0x44, 0x38 }, // b
	{ 0x38, 0x44, 0x44, 0x44, 0x20 }, // c
	{ 0x38, 0x44, 0x44, 0x48, 0x7F }, // d
	{ 0x38, 0x54, 0x54, 0x54, 0x18 }, // e
	{ 0x08, 0x7E, 0x09, 0x01, 0x02 }, // f
	{ 0x08, 0x14, 0x54, 0x54, 0x3C }, // g
	{ 0x7F, 0x08, 0x04, 0x04, 0x78 }, // h
	{ 0x00, 0x44, 0x7D, 0x40, 0x00 }, // i
	{ 0x20, 0x40, 0x44, 0x3D, 0x00 }, // j
	{ 0x00, 0x7F, 0x10, 0x28, 0x44 }, // k
	{ 0x00, 0x41, 0x7F, 0x40, 0x00 }, // l
	{ 0x7C, 0x04, 0x18, 0x04, 0x78 }, // m
	{ 0x7C, 0x08, 0x04, 0x04, 0x78 }, // n
	{ 0x38, 0x44, 0x44, 0x44, 0x38 }, // o
	{ 0x7C, 0x14, 0x14, 0x14, 0x08 }, // p
	{ 0x08, 0x14, 0x14, 0x18, 0x7C }, // q
	{ 0x7C, 0x08, 0x04, 0x04, 0x08 }, // r
	{ 0x48, 0x54, 0x54, 0x54, 0x20 }, // s
	{ 0x04, 0x3F, 0x44, 0x40, 0x20 }, // t
	{ 0x3C, 0x40, 0x40, 0x20, 0x7C }, // u
	{ 0x1C, 0x20, 0x40, 0x20, 0x1C }, // v
	{ 0x3C, 0x40, 0x30, 0x40, 0x3C }, // w
	{ 0x44, 0x28, 0x10, 0x28, 0x44 }, // x
	{ 0x0C, 0x50, 0x50, 0x50, 0x3C }, // y
	{ 0x44, 0x64, 0x54, 0x4C, 0x44 }, // z
	{ 0x00, 0x08, 0x36, 0x41, 0x00 }, // {
	{ 0x00, 0x00, 0x7F, 0x00, 0x00 }, // |
	{ 0x00, 0x41, 0x36, 0x08, 0x00 }, // }
	{ 0x08, 0x08, 0x2A, 0x1C, 0x08 }, // ->
	{ 0x08, 0x1C, 0x2A, 0x08, 0x08 }, // <-
};

ST7735s::ST7735s(gpio_num_t sclPin, gpio_num_t sdaPin, gpio_num_t dcPin, gpio_num_t resPin)
	:sclPin(sclPin), sdaPin(sdaPin), dcPin(dcPin), resPin(resPin) {
	cursorX = 0;
	cursorY = 0;
}

unsigned long ST7735s::intsqrt(unsigned long val) {
	unsigned long mulMask = 0x0008000;
	unsigned long retVal = 0;
	if (val > 0) {
		while (mulMask != 0) {
			retVal |= mulMask;
			if (retVal*retVal > val)
				retVal &= ~ mulMask;
			mulMask >>= 1;
		}
	}
	return retVal;
}

void ST7735s::sendCommand(Command command) {
	gpio_set_level(dcPin, 0);
	sendData((uint8_t)command);
	gpio_set_level(dcPin, 1);
}

void ST7735s::sendData(uint8_t data) {
	spi_transaction_t transaction;
	memset( &transaction, 0, sizeof( spi_transaction_t ) );
	transaction.length = 8;
	transaction.flags |= SPI_TRANS_USE_TXDATA;
	transaction.tx_data[0] = data;
	esp_err_t result = spi_device_transmit(spi, &transaction);
    if (result != ESP_OK)
		throw EspException("Fatal error: spi_device_transmit returned " + $(result) +
		": " + esp_err_to_name(result));
}

void ST7735s::sendData16Bit(uint16_t data) {
	spi_transaction_t transaction;
    memset( &transaction, 0, sizeof( spi_transaction_t ) );
    transaction.length = 16;
	transaction.flags |= SPI_TRANS_USE_TXDATA;
    transaction.tx_data[0] = data >> 8;
	transaction.tx_data[1] = data & 0xFF;
    esp_err_t result = spi_device_transmit( spi, &transaction );
	if (result != ESP_OK)
		throw EspException("Fatal error: spi_device_transmit returned " + $(result) +
		": " + esp_err_to_name(result));
}

void ST7735s::fillColor(Color color, unsigned int count) {
	sendCommand(RamWrite);
	for (; count > 0; count--) {
		sendData16Bit((uint16_t)color);
	}
}

void ST7735s::init() {
	gpio_config_t config;
    memset(&config, 0, sizeof(gpio_config_t));
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (1ULL << resPin) | (1ULL << dcPin);
    esp_err_t result = gpio_config(&config);
	if (result != ESP_OK)
		throw EspException("Fatal error: gpio_config returned " + $(result) +
		": " + esp_err_to_name(result));

	memset( &buscfg, 0, sizeof ( buscfg ) );
    buscfg.miso_io_num = -1;
    buscfg.mosi_io_num = sdaPin;
    buscfg.sclk_io_num = sclPin;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
	memset( &devcfg, 0, sizeof ( devcfg ) );
    devcfg.clock_speed_hz = 10*1000*1000;
    devcfg.mode = 0;         //SPI mode 0
    devcfg.spics_io_num = -1;
    devcfg.queue_size = 7;   // able to queue 7 transactions at a time
	result = spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
	if (result != ESP_OK)
		throw EspException("Fatal error: spi_bus_initialize returned " + $(result) +
		": " + esp_err_to_name(result));
	result = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	if (result != ESP_OK)
		throw EspException("Fatal error: spi_device_transmit returned " + $(result) +
		": " + esp_err_to_name(result));

	gpio_set_level(resPin, 1);
	vTaskDelay(pdMS_TO_TICKS(1));
	gpio_set_level(resPin, 0);
	vTaskDelay(pdMS_TO_TICKS(1));
	gpio_set_level(resPin, 1);
	vTaskDelay(pdMS_TO_TICKS(150));

	sendCommand(SleepOut);
	vTaskDelay(pdMS_TO_TICKS(150));
	sendCommand(ColorMode);
	sendData(0x05); // select color mode 5 = 16bit pixels (RGB565)
	sendCommand(DisplayOn);
}

void ST7735s::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	// if (x0 > XSIZE || y0 > YSIZE || x1 > XSIZE || y1 > YSIZE || x0 > x1 || y0 > y1)
	// 	throw invalid_argument(
	// 		"Invalid addr window coordinates: (" + $(x0) + ", " + 
	// 		$(y0) + "), (" + $(x1) + ", " + $(y1) + ")");
	sendCommand(ColumnAddressSet); // set column range (x0,x1)
	sendData16Bit(x0+2);  // for some reason (propably bad hardware connections) display behaves as if it was 132x168
	sendData16Bit(x1+2);
	sendCommand(RowAddressSet); // set row range (y0,y1)
	sendData16Bit(y0+1);
	sendData16Bit(y1+1);
}

void ST7735s::verticalLine(uint8_t x, uint8_t yUp, uint8_t yDown, Color color) {
	if (yUp > yDown || yUp > YSIZE || yDown > YSIZE) throw invalid_argument(
		"Invalid vertical line coordinates: (" + $(yUp) + ", " + $(yDown) + ")"
	)
	setAddrWindow(x, yUp, x, yDown);
	fillColor(color, yDown - yUp + 1);
}

void ST7735s::line(int xLeft, int yUp, int xRight, int yDown, Color color) { // Bresenham algorithm
	int dx = fabs(xRight - xLeft), sx = xLeft < xRight ? 1 : -1;
	int dy = fabs(yDown - yUp), sy = yUp < yDown ? 1 : -1;
	int err = (dx > dy ? dx : -dy)/2, e2;
	while (true) {
		drawPixel(xLeft, yUp, color);
		if (xLeft == xRight && yUp == yDown) break;
		e2 = err;
		if (e2 >- dx) { err -= dy; xLeft += sx; }
		if (e2 < dy) { err += dx; yUp += sy; }
	}
}

void ST7735s::drawRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color) {
	if (xLeft > XSIZE || yUp > YSIZE || xRight > XSIZE || yDown > YSIZE || xLeft > xRight || yUp > yDown)
		throw invalid_argument(
			"Invalid rectangle coordinates: (" + $(xLeft) + ", " + 
			$(yUp) + "), (" + $(xRight) + ", " + $(yDown) + ")");
	horizontalLine(xLeft, xRight, yUp, color);
	horizontalLine(xLeft, xRight, yDown, color);
	verticalLine(xLeft, yUp, yDown, color);
	verticalLine(xRight, yUp, yDown, color);
}

void ST7735s::fillRect(uint8_t xLeft, uint8_t yUp, uint8_t xRight, uint8_t yDown, Color color) {
	// if (xLeft > XSIZE || yUp > YSIZE || xRight > XSIZE || yDown > YSIZE || xLeft > xRight || yUp > yDown)
	// 	throw invalid_argument(
	// 		"Invalid filled rectangle coordinates: (" + $(xLeft) + ", " + 
	// 		$(yUp) + "), (" + $(xRight) + ", " + $(yDown) + ")");
	uint8_t width = xRight - xLeft + 1;
	uint8_t height = yDown - yUp + 1;
	setAddrWindow(xLeft, yUp, xRight, yDown);
	fillColor(color, width * height);
}

void ST7735s::clearScreen() {
	fillRect(0, 0, 129, 169, White);
}

void ST7735s::circleQuadrant(int16_t xPos, int16_t yPos, uint8_t radius, Quad quad, Color color) {
	if ((quad == LowerRight && (yPos - radius > YSIZE || xPos - radius > XSIZE)) || 
		(quad == UpperRight && (yPos + radius < 0 || xPos - radius > XSIZE)) ||
		(quad == LowerLeft && (yPos - radius > YSIZE || xPos + radius < 0)) ||
		(quad == UpperLeft && (yPos + radius < 0 || xPos + radius < 0))
	) throw new invalid_argument(
		"Error: this " + quadToString(quad) + " circle quadrant won't be visible: " + $(xPos) + ", " + $(yPos) + ")");
	int end = (707*radius)/1000 + 1;
	for (uint8_t x = 0; x < end; x++) {
		uint8_t y = intsqrt(radius*radius - i*i);
		try {
			switch (quad) {
				case LowerRight:
					drawPixel(xPos+x, yPos+y, color);
					break;
				case UpperRight:
					drawPixel(xPos+x, yPos-y, color);
					break;
				case LowerLeft:
					drawPixel(xPos-x, yPos+y, color);
					break;
				case UpperLeft:
					drawPixel(xPos-x, yPos-y, color);
			}
		} catch (invalid_argument& e) {}
		try {
			switch (quad) {
				case LowerRight:
					drawPixel(xPos+y, yPos+x, color);
					break;
				case UpperRight:
					drawPixel(xPos+y, yPos-x, color);
					break;
				case LowerLeft:
					drawPixel(xPos-y, yPos+x, color);
					break;
				case UpperLeft:
					drawPixel(xPos-y, yPos-x, color);
			}
		} catch (invalid_argument& e) {}
	}
}

string ST7735s::quadToString(Quad quad) {
	switch(quad) {
		case LowerRight: return "lower right";
		case UpperRight: return "upper right";
		case LowerLeft: return "lower left";
		case UpperLeft: return "upper left";
	}
}

void ST7735s::drawCircle(uint8_t x, uint8_t y, uint8_t r, Color color) {
	if (yPos - radius > YSIZE || xPos - radius > XSIZE || yPos + radius < 0 || xPos + radius < 0)
		throw new illegal_argument(
			"Error: this circle won't be visible: (" + $(x) + "m " + $(y) + "), r = " + $(r)
		);
	circleQuadrant(x, y, r, LowerRight, color);
	circleQuadrant(x, y, r, UpperRight, color);
	circleQuadrant(x, y, r, LowerLeft, color);
	circleQuadrant(x, y, r, UpperLeft, color);
}

void ST7735s::drawRoundRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t r, Color color) {
	horizontalLine(x0+r, x1-r, y0, color); // top side
	horizontalLine(x0+r, x1-r, y1, color); // bottom side
	verticalLine(x0, y0+r, y1-r, color); // left side
	verticalLine(x1, y0+r, y1-r, color); // right side
	circleQuadrant(x0+r, y0+r, r, 8, color); // upper left corner
	circleQuadrant(x1-r, y0+r, r, 2, color); // upper right corner
	circleQuadrant(x0+r, y1-r, r, 4, color); // lower left corner
	circleQuadrant(x1-r, y1-r, r, 1, color); // lower right corner
}

void ST7735s::fillCircle(uint8_t xPos, uint8_t yPos, uint8_t radius, Color color) {
	long r2 = radius * radius;
	for (int x = 0; x <= radius; x++) {
		uint8_t y = intsqrt(r2-x*x);
		uint8_t y0 = yPos - y;
		uint8_t y1 = yPos + y;
		verticalLine(xPos+x, y0, y1, color);
		verticalLine(xPos-x, y0, y1, color);
	}
}

void ST7735s::drawEllipse(int x0, int y0, int width, int height, Color color) {
// two-part Bresenham method
// note: slight discontinuity between parts on some (narrow) ellipses.
	int a = width/2, b = height/2;
	int x = 0, y = b;
	long a2 = (long)a*a*2;
	long b2 = (long)b*b*2;
	long error = (long)a*a*b;
	long stopY = 0, stopX = a2*b;
	while (stopY <= stopX) {
		drawPixel(x0+x, y0+y, color);
		drawPixel(x0+x, y0-y, color);
		drawPixel(x0-x, y0+y, color);
		drawPixel(x0-x, y0-y, color);
		x++;
		error -= b2*(x-1);
		stopY += b2;
		if (error < 0) {
			error += a2*(y-1);
			y--;
			stopX -= a2;
		}
	}
	x = a; y = 0; error = b*b*a;
	stopY = a*b2; stopX = 0;
	while (stopY >= stopX) {
		drawPixel(x0+x, y0+y, color);
		drawPixel(x0+x, y0-y, color);
		drawPixel(x0-x, y0+y, color);
		drawPixel(x0-x, y0-y, color);
		y++;
		error -= a2*(y-1);
		stopX += a2;
		if (error < 0) {
			error += b2*(x-1);
			x--;
			stopY -= b2;
		}
	}
}

void ST7735s::fillEllipse(int xPos,int yPos,int width,int height, Color color) {
	int a = width/2, b = height/2; // get x & y radii
	int x1, x0 = a, y = 1, dx = 0;
	long a2 = a*a, b2 = b*b; // need longs: big numbers!
	long a2b2 = a2 * b2;
	horizontalLine(xPos-a, xPos+a, yPos, color); // draw centerline
	while (y<=b) { // draw horizontal lines...
		for (x1= x0-(dx-1); x1>0; x1--)
			if (b2*x1*x1 + a2*y*y <= a2b2)
				break;
		dx = x0 - x1;
		x0 = x1;
		horizontalLine(xPos-x0, xPos+x0, yPos+y, color);
		horizontalLine(xPos-x0, xPos+x0, yPos-y, color);
		y++;
	}
}
// ---------------------------------------------------------------------------
// TEXT ROUTINES
//
// Each ASCII character is 5x7, with one pixel space between characters
// So, character width = 6 pixels & character height = 8 pixels.
//
// In portrait mode:
// Display width = 128 pixels, so there are 21 chars/row (21x6 = 126).
// Display height = 160 pixels, so there are 20 rows (20x8 = 160).
// Total number of characters in portait mode = 21 x 20 = 420.
//
// In landscape mode:
// Display width is 160, so there are 26 chars/row (26x6 = 156).
// Display height is 128, so there are 16 rows (16x8 = 128).
// Total number of characters in landscape mode = 26x16 = 416.

#define PORTRAIT_MAX_CURSOR_X 20
#define PORTRAIT_MAX_CURSOR_Y 19

void ST7735s::gotoXY(uint8_t x, uint8_t y) { // position cursor on character x,y grid, where 0<x<20, 0<y<19.
	
	cursorX = x;
	cursorY = y;
}
void ST7735s::gotoLine(uint8_t y) { // position character cursor to start of line y, where 0<y<19.
	cursorX = 0;
	cursorY = y;
}

void ST7735s::advanceCursor() { // moves character cursor to next position, assuming portrait orientation
	cursorX++;
	if (cursorX > PORTRAIT_MAX_CURSOR_X) {
		cursorY++;
		cursorX = 1;
	}
	if (cursorY > PORTRAIT_MAX_CURSOR_Y)
		cursorY = 1;
}

void ST7735s::setOrientation(int degrees) {
	uint8_t arg;
	switch (degrees) {
		case 90: arg = 0x60; break;
		case 180: arg = 0xC0; break;
		case 270: arg = 0xA0; break;
		default: arg = 0x00;
	}
	sendCommand(MADCTL);
	sendData(arg);
}

void ST7735s::writeChar(char ch, Color color) {
	uint8_t mask = 0x01;
	setAddrWindow(cursorX*6, cursorY*8, cursorX*6+4, cursorY*8+6);
	sendCommand(RamWrite);
	for (uint8_t row = 0; row < 7; row++) {
		for (uint8_t col = 0; col < 5; col++) {
			if ((FONT_CHARS[ch-32][col] & mask) == 0) sendData16Bit((uint16_t)Black);
			else sendData16Bit((uint16_t)color);
		}
		mask <<= 1;
	}
	advanceCursor();
}

void ST7735s::writeString(const char* text, Color color) {
	while(*text)
		writeChar(*text++, color);
}

void ST7735s::writeInt(int i) {
	char str[8];
	itoa(i, str, 10);
	writeString(str, White);
}

void ST7735s::writeHex(int i) {
	char str[8];
	itoa(i, str, 16);
	writeString(str, White);
}