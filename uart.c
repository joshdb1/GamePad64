/* uart.c
*	author: Adam Fletcher
*/

#include "uart.h"

//initialize UART connections
void uartInit(void)
{
	//initialize PB6 and PB7 as TX and RX respectively
	//USART1_TX and USART1_RX
	//alternate function mode
	GPIOB->MODER &= ~((uint32_t)0xF << (2 * 6));
	GPIOB->MODER |= (0xA << (2 * 6));
	//pick appropriate alternate function (USART1)
	GPIOB->AFR[0] &= ~((uint32_t)0xFF << (4 * 6));
	GPIOB->AFR[0] |= (0x77 << (4 * 6));
	//high speed pins
	GPIOB->OSPEEDR |= (0xf << (2 * 6));
	//set PUPDR as pull up
	GPIOB->PUPDR &= ~((uint32_t)0xF << (2 * 6));
	GPIOB->PUPDR |= (0x5 << (2 * 6));
	//set OTYPER as push pull
	GPIOB->OTYPER |= (0x3 << 6);
	//enable clock to USART1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL);
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;

	USART_Init(USART1);
}

void USART_Init(USART_TypeDef *USARTx)
{
	USARTx->CR1 &= ~USART_CR1_UE;
	USARTx->CR1 &= ~USART_CR1_M;
	USARTx->CR2 &= ~USART_CR2_STOP;
	USARTx->CR1 &= ~USART_CR1_PCE;
	USARTx->CR1 &= ~USART_CR1_OVER8;
	USARTx->BRR = 0x1A1;
	USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	USARTx->CR1 |= USART_CR1_UE;
	while ((USARTx->ISR & USART_ISR_TEACK) == 0)
		;
	while ((USARTx->ISR & USART_ISR_REACK) == 0)
		;
}

//attempt connection to other microcontroller
uint8_t uartConnect(void)
{
	uint8_t connected = 0, attempt = 1;
	while (attempt != 0)
	{
		connected = uartReceive(SEC); //wait
		//if no message received yet, I'm first
		if (connected == 0)
		{
			uartSend(2);
			//wait for confirmation
			connected = uartReceive(MIN); //wait 1 minute
			//no confirmation, prompt try again?
			if (connected == 0)
			{
				uint8_t str[6];
				sprintf((char *)str, "TRY%d?", attempt);
				LCD_DisplayString(str);
				//get user input
				while ((GPIOA->IDR & 0x2F) == 0)
					;
				//if center, try again
				if (GPIOA->IDR & 0x1)
				{
					attempt++;
				}
				//if anything else, play single device mode
				else
				{
					attempt = 0;
				}
				LCD_Clear();
			}
			else if (connected == 2)
			{
				LCD_DisplayString((uint8_t *)"FAILED");
				//1 second delay
				for (volatile uint32_t i = 0; i < SEC; i++)
					;
				LCD_DisplayString((uint8_t *)"CNECT?");
				while ((GPIOA->IDR & 0x2F) == 0)
					;
				if (GPIOA->IDR & 0x1)
				{
					//check if connected to other uC
					connected = uartConnect();
				}
				attempt = 0;
				LCD_Clear();
			}
		}
		//else if message received, I'm second
		else
		{
			//send confirmation
			uartSend(1);
			attempt = 0;
		}
	}

	return connected;
}

//send data to other microcontroller
void uartSend(uint8_t message)
{
	//wait until tx is empty
	while (!(USART1->ISR & USART_ISR_TXE))
		;
	//write message to TDR
	USART1->TDR = message & 0xFF;
	//wait until transfer is complete
	while (/*!*/ (USART1->ISR & USART_ISR_TC))
		; //in the UART lab we had the ! shown in the comment, but I don't think it should be there
	//Clear the transfer complete flag
	USART1->ICR |= USART_ICR_TCCF;
}

//receive data from other microcontroller
uint8_t uartReceive(uint32_t delay)
{
	uint8_t message = 0;
	while (!(USART1->ISR & USART_ISR_RXNE) && (delay > 0))
	{
		delay--;
	}
	if (USART1->ISR & USART_ISR_RXNE)
	{
		message = (uint8_t)USART1->RDR;
	}
	return message;
}
