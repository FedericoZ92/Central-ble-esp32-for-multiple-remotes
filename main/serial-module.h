
#pragma once
#include "driver/uart.h"
#include "driver/gpio.h"
#include <functional>

// ===== User Configuration =====
#define SERIAL_TX_GPIO       GPIO_NUM_43  // Default TX for UART0 on ESP32-S3
#define SERIAL_RX_GPIO       GPIO_NUM_44  // Default RX for UART0 on ESP32-S3
#define SERIAL_UART_PORT     UART_NUM_0
#define SERIAL_BAUD_RATE     115200
#define SERIAL_BUF_SIZE      1024
#define SERIAL_TASK_STACK    2048
#define SERIAL_TASK_PRIO     10

extern void onSerialData(const uint8_t* data, size_t len);

// ===== Serial Class =====
class SerialModule {
public:
    using WriteCallback = std::function<void(const uint8_t* data, size_t len)>;

    SerialModule();
    void begin();
    void setWriteCallback(WriteCallback cb);
    void write(const uint8_t* data, size_t len);
    void write(const char* str);

private:
    static void uartEventTask(void* pvParameters);
    WriteCallback writeCallback;
};

// Extern global instance
extern SerialModule SerialLine;