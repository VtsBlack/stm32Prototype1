#include <stdio.h>               /* prototype declarations for I/O functions  */

#include <stm32l1xx.h>
#include "LED.h"
#include "Serial.h"
#include "stm32l1xx_conf.h"

#include "main.h"
#include "modem_test.h"



struct MODEM_TEST_TYPE md = {0, {0}, 0};

void init_modem_pins(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	// power on pin
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	
	Delay(100);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_2);
	
	Delay(100);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_2);
	
}


void modem_uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef nvic;


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // tx and rx
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
	
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_WordLength  = USART_WordLength_8b;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStruct);
  
	USART_Cmd(USART2, ENABLE);
	
	nvic.NVIC_IRQChannel = USART2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
}


void USART2_IRQHandler(void)
{
	char byte=0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
		// clear by read, forward to usart 1
		byte = USART2->DR;
	  USART1->DR = byte;
		md.receive_cnt++;
		
		if (md.rx_index >= sizeof(md.rx_buf)) { md.rx_index = 0;}
		//---------------------------------------------------------
		md.rx_buf[md.rx_index] = byte;
		md.rx_index++;
	}
}

