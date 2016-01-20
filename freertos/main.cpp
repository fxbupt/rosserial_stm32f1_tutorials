/*********************************************************************
 *
 *  Copyright (c) 2016 Robosavvy Ltd.
 *  Author: Vitor Matos
 *
 *  rosserial_stm32f1 chatter tutorial
 *
 *  On this demo your STM32 MCU will publish a string over the
 * topic "/chatter".
 *
 *********************************************************************/

extern "C"
{
  #include <stm32f10x.h>
  #include "FreeRTOSConfig.h"
  #include "FreeRTOS.h"
  #include "task.h"
}
#include "ros_FreeRTOS.h"
#include "spin_task.h"
#include "publish_task.h"
#include "subscribe_task.h"

static void SetSysClockTo56(void);

// ROS nodehandle
ros::NodeHandle nh;

int main(void)
{  
  SetSysClockTo56();
  
  // ROS nodehandle initialization and topic registration
  nh.initNode();
  
  // Initialize debug LED
  GPIO_InitTypeDef GPIO_Config;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_Config.GPIO_Pin =  GPIO_Pin_5;
  GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_Config);
  GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
  
  // Start ROS spin task, responsible for handling callbacks and communications
  if (spinInitTask(&nh))
  {
    // Turn on LED on error
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
    while (1);
  }
  
  // Register and init publish task
  if (publishInitTask(&nh))
  {
    // Turn on LED on error
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
    while (1);
  }

  // Register and init subscribe task
  if (subscribeInitTask(&nh))
  {
    // Turn on LED on error
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
    while (1);
  }
  
  // Enter scheduler and loop forever
  vTaskStartScheduler();
  
  // In case the scheduler returns for some reason,.
  while (1)
  {
    // Turn on LED on error
    GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
  }
}


static void SetSysClockTo56(void)
{
  __IO uint32_t StartUpCounter = 0, HSIStatus = 0;
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/   
  /* Enable HSI */    
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
 
  /* Wait till HSI is ready and if Time out is reached exit */
  do
  {
    HSIStatus = RCC->CR & RCC_CR_HSIRDY;
    StartUpCounter++;  
  } while((HSIStatus == 0) && (StartUpCounter != ((uint16_t)0x5000)));

  if ((RCC->CR & RCC_CR_HSIRDY) != RESET)
  {
    HSIStatus = (uint32_t)0x01;
  }
  else
  {
    HSIStatus = (uint32_t)0x00;
  }  

  if (HSIStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    
 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
  
    /* PLL configuration: PLLCLK = HSI * 7 = 56 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL14);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  } 
}
