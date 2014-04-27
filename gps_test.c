#include <stdio.h>               /* prototype declarations for I/O functions  */

#include <stm32l1xx.h>
#include "LED.h"
#include "Serial.h"
#include "stm32l1xx_conf.h"

#include "main.h"
#include "gps_test.h"

struct GPS_TEST_TYPE gp;

void init_gps_pins(void)
{
	#define GPS_EN_GPIO GPIOB
	#define GPS_EN_PIN GPIO_Pin_12
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	// power on pin
	GPIO_InitStruct.GPIO_Pin = GPS_EN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	
	
	GPIO_Init(GPS_EN_GPIO, &GPIO_InitStruct);
	
	
	GPIO_SetBits(GPS_EN_GPIO, GPS_EN_PIN);
}


void init_gps_uart(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef nvic;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
	
	GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_10 | GPIO_Pin_11); // tx and rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
	
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_WordLength  = USART_WordLength_8b;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART3, &USART_InitStruct);
  
	USART_Cmd(USART3, ENABLE);
	
	nvic.NVIC_IRQChannel = USART3_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}


void USART3_IRQHandler(void)
{
	char byte=0;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
		// clear by read, forward to usart 1
		byte = USART3->DR;
	  USART1->DR = byte;
		gp.receive_cnt++;
		
		if (gp.rx_index >= sizeof(gp.rx_buf)) { gp.rx_index = 0;}
		//---------------------------------------------------------
		gp.rx_buf[gp.rx_index] = byte;
		gp.rx_index++;
	}
}
