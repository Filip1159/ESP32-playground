#include "inc/main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include "driver/ledc.h"

#include "inc/HD44780.h"

extern "C" void app_main(void) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    HD44780 textDisplay(RS_Pin, E_Pin, D4_Pin, D5_Pin, D6_Pin, D7_Pin, V0_Pin);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        textDisplay.writeChar('!');
        printf("a\n");
    }
}