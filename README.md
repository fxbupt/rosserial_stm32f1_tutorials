# rosserial_stm32f1_tutorials
Tutorials and examples for rosserial_stm32f1 package.

## Tutorials and examples:

### chatter
Small demo for the STM32F1038C.
It uses UART2 to communicate with rosserial.
Publishes on topic `/chatter` a string every few milliseconds.

### freertos
Small demo for the STM32F1038C using FreeRTOS.
It uses UART2 to communicate with rosserial.
Publishes on topic `/char_count` the number of chars input on topic `/str`.

Make sure there is only the FreeRTOSConfig.h of this application.
