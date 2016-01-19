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
}
#include <ros.h>
#include <std_msgs/String.h>

static void SetSysClockTo56(void);

// ROS nodehandle
ros::NodeHandle nh;

std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[13] = "Hello world!";

int main(void)
{  
  SetSysClockTo56();
  
  // ROS nodehandle initialization and topic registration
  nh.initNode();
  nh.advertise(chatter);

  // Initialize LED
  GPIO_InitTypeDef GPIO_Config;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_Config.GPIO_Pin =  GPIO_Pin_5;
  GPIO_Config.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_Config);
  GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
  
  while (1)
  {
    // Toggle LED
    if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5))
      GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
    else
      GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);

    // Publish message to be transmitted.
    str_msg.data = hello;
    chatter.publish(&str_msg);

    // Handle all communications and callbacks.
    nh.spinOnce();
    
    // Delay for a bit.
    nh.getHardware()->delay(100);
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
