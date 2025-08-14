#include "serial-module.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <cstring>

static QueueHandle_t uart_queue;

SerialModule SerialLine; // Global instance

void onSerialData(const uint8_t* data, size_t len) {
    SerialLine.write(data, len);
}

SerialModule::SerialModule()
    : writeCallback(nullptr) {}

void SerialModule::begin() {
    // Configure UART
    uart_config_t uart_config = {};
    uart_config.baud_rate = SERIAL_BAUD_RATE;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_APB;

    uart_driver_install(SERIAL_UART_PORT, SERIAL_BUF_SIZE, SERIAL_BUF_SIZE, 10, &uart_queue, 0);
    uart_param_config(SERIAL_UART_PORT, &uart_config);
    uart_set_pin(SERIAL_UART_PORT, SERIAL_TX_GPIO, SERIAL_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Start UART event task
    xTaskCreate(uartEventTask, "uart_event_task", SERIAL_TASK_STACK, this, SERIAL_TASK_PRIO, nullptr);
}

void SerialModule::setWriteCallback(WriteCallback cb) {
    writeCallback = cb;
}

void SerialModule::write(const uint8_t* data, size_t len) {
    uart_write_bytes(SERIAL_UART_PORT, (const char*)data, len);
}

void SerialModule::write(const char* str) {
    write((const uint8_t*)str, strlen(str));
}

void SerialModule::uartEventTask(void* pvParameters) {
    SerialModule* self = static_cast<SerialModule*>(pvParameters);
    uart_event_t event;
    uint8_t data[SERIAL_BUF_SIZE];

    while (true) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                int len = uart_read_bytes(SERIAL_UART_PORT, data, event.size, pdMS_TO_TICKS(100));
                if (len > 0 && self->writeCallback) {
                    self->writeCallback(data, len);
                }
            }
        }
    }
}