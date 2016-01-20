#include <stdbool.h>
#include <stdint.h>
#include "ros_FreeRTOS.h"
extern "C"
{
  #include <stm32f10x.h>
  #include "FreeRTOS.h"
  #include "task.h"
}

#define tskSPIN_PRIORITY 1

static ros::NodeHandle *nh_;

// ros spin() like task
static void spinTask(void *pvParameters)
{
  portTickType ui32WakeTime;
  // Get the current tick count.
  ui32WakeTime = xTaskGetTickCount();

  while (1)
  {
    // rosserial callback handling
    nh_->spinOnce();

    // Toggle for spin heartbeat
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5))
      GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
    else
      GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);

    vTaskDelayUntil(&ui32WakeTime, 100);
  }
}

// spin task initialization
uint32_t spinInitTask(ros::NodeHandle *nh)
{
  nh_ = nh;

  // Init spin task
  if (xTaskCreate(spinTask, (signed portCHAR *)"spin", 100, NULL, tskIDLE_PRIORITY + tskSPIN_PRIORITY, NULL) != pdTRUE)
  {
    return 1;
  }
  return 0;
}
