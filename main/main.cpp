#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include "driver/ledc.h"

#include <stdlib.h>
#include <exception>

#include "HD44780.h"
#include "ST7735s.h"
#include "Canvas.h"
#include "Printer.h"

extern "C" void app_main(void) {
    HD44780 textDisplay(RS_Pin, E_Pin, D4_Pin, D5_Pin, D6_Pin, D7_Pin, V0_Pin);

    try {
        ST7735s graphicDisplay(SCL_Pin, SDA_Pin, DC_Pin, RES_Pin);
        Canvas canvas = *(graphicDisplay.getCanvas());
        Printer printer = *(graphicDisplay.getPrinter());
        graphicDisplay.init();
        graphicDisplay.clearScreen();
        graphicDisplay.setOrientation(180);
        canvas.fillRect(0, 128, 125, 156, Cyan);
        printer.setCursor(3, 3);
        printer.writeString("Hello, ESP32!", Yellow);
        printer.setLine(5);
        printer.setFontSize(Big);
        printer.writeString("Big Hello", Magenta);
        
    } catch (std::exception& e) {
        printf("exception\n");
        printf("%s\n", e.what());
    }
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        textDisplay.writeChar('!');
    }
}