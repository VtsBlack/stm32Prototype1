/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
#include <stdio.h>               /* prototype declarations for I/O functions  */

#include <stm32l1xx.h>
#include "LED.h"
#include "Serial.h"
#include "stm32l1xx_conf.h"

#include "main.h"
#include "modem_test.h"
#include "gps_test.h"

volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}


/*----------------------------------------------------------------------------
  Function that initializes Button pins
 *----------------------------------------------------------------------------*/
void BTN_Init(void) {

  RCC->AHBENR |=  (1UL <<  0);                  /* Enable GPIOA clock         */

  GPIOA->MODER    &= ~((3UL << 2*0)  );         /* PA.0 is input              */
  GPIOA->OSPEEDR  &= ~((3UL << 2*0)  );         /* PA.0 is Low Speed          */
  GPIOA->PUPDR    &= ~((3UL << 2*0)  );         /* PA.0 is no Pull up         */
}

/*----------------------------------------------------------------------------
  Function that read Button pins
 *----------------------------------------------------------------------------*/
uint32_t BTN_Get(void) {

 return (GPIOA->IDR & (1UL << 0));
}


/*----------------------------------------------------------------------------
  set HSI as SystemCoreClock (HSE is not populated on STM32L1-Discovery board)
 *----------------------------------------------------------------------------*/
void SystemCoreClockSetHSI(void) {

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                      /* Enable HSI                        */
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                   /* Wait for HSI Ready                */

  FLASH->ACR |= FLASH_ACR_ACC64;                            /* Enable 64-bit access              */
  FLASH->ACR |= FLASH_ACR_PRFTEN;                           /* Enable Prefetch Buffer            */
  FLASH->ACR |= FLASH_ACR_LATENCY;                          /* Flash 1 wait state                */

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;                        /* Enable the PWR APB1 Clock         */
  PWR->CR = PWR_CR_VOS_0;                                   /* Select the Voltage Range 1 (1.8V) */
  while((PWR->CSR & PWR_CSR_VOSF) != 0);                    /* Wait for Voltage Regulator Ready  */

  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;                /* HCLK = SYSCLK                     */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;               /* PCLK2 = HCLK                      */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;               /* PCLK1 = HCLK                      */

  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI; /* HSI is system clock               */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);   /* Wait for HSI used as system clock */
}

extern struct GPS_TEST_TYPE sr;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t num  = -1;
	uint8_t modem_init_mode = 0;

  SystemCoreClockSetHSI();
  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */

  LED_Init();
  //BTN_Init();
	SER_Init ();                   /* initialize the serial interface           */
	
	

  SysTick_Config(SystemCoreClock / 100);        /* SysTick 10 msec interrupts */
	
	#if (TEST_MODEM_EN == 1)
	init_modem_pins();
	modem_uart_init();
	#endif
	
	#if (TEST_GPS_EN == 1)
	init_gps_pins();
	init_gps_uart();
	#endif

  while(1) {                                    /* Loop forever               */
      /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...  */
      num++;
		if (num > 2) {
			num=0;
			#if (TEST_GPS_EN == 1)
			printf("gps rx cnt: %d\r\n", gp.receive_cnt);
			#endif
			
			#if (TEST_MODEM_EN == 1)
			if (modem_init_mode == 0) {
				
				send_to_modem("at\r");
			}
			else {
				send_to_modem("csq\r");
			}
			printf("modem rx cnt: %d\r\n", md.receive_cnt);
			if (sr.rx_index > 0) {
				sr.rx_index = 0;
				printf("echo: %s\r", sr.rx_buf);
				
				// uart 1 rx is the modem input
				if (strncmp(sr.rx_buf, "OK", 2) == 0) {
					modem_init_mode = 1;
				}
			}
			if (md.rx_index > 0) {
				
				
				printf("md<%d>: %s begin:%x,%x,%x\r", md.rx_index, md.rx_buf, md.rx_buf[0], md.rx_buf[1], md.rx_buf[2]);
				md.rx_index = 0;
				
				
			}
			#endif
			
		}


      LED_On (num);
      Delay(10);                                /* Delay 100ms                */
      LED_Off(num);
      Delay(40);                                /* Delay 400ms                */
			
			

  }

}

