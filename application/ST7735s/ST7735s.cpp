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

ST7735s::ST7735s(gpio_num_t sclPin, gpio_num_t sdaPin, gpio_num_t dcPin, gpio_num_t resPin)
	:sclPin(sclPin), sdaPin(sdaPin), dcPin(dcPin), resPin(resPin) {
	cursorX = 0;
	cursorY = 0;
	canvas = new Canvas(this);
	printer = new Printer(this);
}

ST7735s::~ST7735s() {
	delete canvas;
	delete printer;
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

void ST7735s::setAddrWindow(uint16_t xLeft, uint16_t yUp, uint16_t xRight, uint16_t yDown) {
	sendCommand(ColumnAddressSet);
	sendData16Bit(xLeft + 2);  // for some reason (propably bad hardware connections) display behaves as if it was 132x168
	sendData16Bit(xRight + 2);
	sendCommand(RowAddressSet);
	sendData16Bit(yUp + 1);
	sendData16Bit(yDown + 1);
}

void ST7735s::clearScreen() {
	setAddrWindow(0, 0, 129, 169);
	fillColor(Black, 129*169);
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

Printer* ST7735s::getPrinter() {
	return printer;
}

Canvas* ST7735s::getCanvas() {
	return canvas;
}