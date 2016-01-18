/*! ----------------------------------------------------------------------------
 * @file	port.c
 * @brief	HW specific definitions and functions for portability
 *
 * @attention
 *
 * Copyright 2013 (c) DecaWave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 * @author DecaWave
 */

#include "compiler/compiler.h"
#include "platform/port.h"

#define rcc_init(x)					RCC_Configuration(x)
#define systick_init(x)				SysTick_Configuration(x)
#define rtc_init(x)					RTC_Configuration(x)
#define interrupt_init(x)			NVIC_Configuration(x)
#define usart_init(x)				USART_Configuration(x)
#define spi_init(x)					SPI_Configuration(x)
#define gpio_init(x)				GPIO_Configuration(x)
#define ethernet_init(x)			No_Configuration(x)
#define fs_init(x)					No_Configuration(x)
#define usb_init(x)					No_Configuration(x)
#define lcd_init(x)					No_Configuration(x)
#define touch_screen_init(x)		No_Configuration(x)

/* System tick 32 bit variable defined by the platform */
extern __IO unsigned long time32_incr;

int No_Configuration(void)
{
  return -1;
}

unsigned long portGetTickCnt(void)
{
  return time32_incr;
}

int SysTick_Configuration(void)
{
  if (SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC))
  {
    /* Capture error */
    while (1);
  }
  NVIC_SetPriority (SysTick_IRQn, 5);

  return 0;
}



int NVIC_DisableDECAIRQ(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Configure EXTI line */
  EXTI_InitStructure.EXTI_Line = DECAIRQ_EXTI;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MPW3 IRQ polarity is high by default
  EXTI_InitStructure.EXTI_LineCmd = DECAIRQ_EXTI_NOIRQ;
  EXTI_Init(&EXTI_InitStructure);

  return 0;
}



/**
 * @brief  Checks whether the specified EXTI line is enabled or not.
 * @param  EXTI_Line: specifies the EXTI line to check.
 *   This parameter can be:
 *     @arg EXTI_Linex: External interrupt line x where x(0..19)
 * @retval The "enable" state of EXTI_Line (SET or RESET).
 */
ITStatus EXTI_GetITEnStatus(uint32_t EXTI_Line)
{
  ITStatus bitstatus = RESET;
  uint32_t enablestatus = 0;
  /* Check the parameters */
  assert_param(IS_GET_EXTI_LINE(EXTI_Line));

  enablestatus =  EXTI->IMR & EXTI_Line;
  if (enablestatus != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

int USART_Configuration(void)
{
#if 0
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  // USARTx setup
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USARTx, &USART_InitStructure);

  // USARTx TX pin setup
  GPIO_InitStructure.GPIO_Pin = USARTx_TX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(USARTx_GPIO, &GPIO_InitStructure);

  // USARTx RX pin setup
  GPIO_InitStructure.GPIO_Pin = USARTx_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(USARTx_GPIO, &GPIO_InitStructure);

  // Enable USARTx
  USART_Cmd(USARTx, ENABLE);
#endif
  return 0;
}

void SPI_ChangeRate(uint16_t scalingfactor)
{
  uint16_t tmpreg = 0;

  /* Get the SPIx CR1 value */
  tmpreg = SPIx->CR1;

  /*clear the scaling bits*/
  tmpreg &= 0xFFC7;

  /*set the scaling bits*/
  tmpreg |= scalingfactor;

  /* Write to SPIx CR1 */
  SPIx->CR1 = tmpreg;
}

void SPI_ConfigFastRate(uint16_t scalingfactor)
{
  SPI_InitTypeDef SPI_InitStructure;

  SPI_I2S_DeInit(SPIx);

  // SPIx Mode setup
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	 //
  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; //
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //
  //SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = scalingfactor; //sets BR[2:0] bits - baudrate in SPI_CR1 reg bits 4-6
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;

  SPI_Init(SPIx, &SPI_InitStructure);

  // Enable SPIx
  SPI_Cmd(SPIx, ENABLE);
}

int SPI_Configuration(void)
{
  // Set MOSI and SCK as O/P
  DDRB = (1<<5)|(1<<3);
  //Enable SPI, Set as Master
  //Prescaler:Fosc/16, Enable Interrupts
  //The MOSI, SCK pins are as per Atmega328p
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
  return 0;
}




void reset_DW1000(void)
{
  // Enable GPIO used for DW1000 reset
  cbi(DDRB,0); //PB0 is the DW1000 rst pin
  //drive the RSTn pin low
  cbi(PORTB,0);
  //put the pin back to tri-state ... as input
  sbi(DDRB,0);

  Sleep(2);
}


void setup_DW1000RSTnIRQ(int enable)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  if(enable)
  {
    // Enable GPIO used as DECA IRQ for interrupt
    GPIO_InitStructure.GPIO_Pin = DECARSTIRQ;
    //GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_IPD;	//IRQ pin should be Pull Down to prevent unnecessary EXT IRQ while DW1000 goes to sleep mode
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DECARSTIRQ_GPIO, &GPIO_InitStructure);

    /* Connect EXTI Line to GPIO Pin */
    GPIO_EXTILineConfig(DECARSTIRQ_EXTI_PORT, DECARSTIRQ_EXTI_PIN);

    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = DECARSTIRQ_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MP IRQ polarity is high by default
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Set NVIC Grouping to 16 groups of interrupt without sub-grouping */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Enable and set EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = DECARSTIRQ_EXTI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
  }
  else
  {
    //put the pin back to tri-state ... as input
    GPIO_InitStructure.GPIO_Pin = DW1000_RSTn;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DW1000_RSTn_GPIO, &GPIO_InitStructure);

    /* Configure EXTI line */
    EXTI_InitStructure.EXTI_Line = DECARSTIRQ_EXTI;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//MP IRQ polarity is high by default
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
}




#ifdef USART_SUPPORT

/**
 * @brief  Configures COM port.
 * @param  USART_InitStruct: pointer to a USART_InitTypeDef structure that
 *   contains the configuration information for the specified USART peripheral.
 * @retval None
 */
void usartinit(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* USARTx configured as follow:
     - BaudRate = 115200 baud
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
     */
  USART_InitStructure.USART_BaudRate = 115200 ;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  //For EVB1000 -> USART2_REMAP = 0

  /* Enable the USART2 Pins Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);


  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART2, &USART_InitStructure);

  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
}

void USART_putc(char c)
{
  //while(!(USART2->SR & 0x00000040));
  //USART_SendData(USART2,c);
  /* e.g. write a character to the USART */
  USART_SendData(USART2, c);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)	;
}

void USART_puts(const char *s)
{
  int i;
  for(i=0; s[i]!=0; i++)
  {
    USART_putc(s[i]);
  }
}

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
void printf2(const char *format, ...)
{
  va_list list;
  va_start(list, format);

  int len = vsnprintf(0, 0, format, list);
  char *s;

  s = (char *)malloc(len + 1);
  vsprintf(s, format, list);

  USART_puts(s);

  free(s);
  va_end(list);
  return;
}


#endif


int is_IRQ_enabled(void)
{
  return ((   NVIC->ISER[((uint32_t)(DECAIRQ_EXTI_IRQn) >> 5)]
        & (uint32_t)0x01 << (DECAIRQ_EXTI_IRQn & (uint8_t)0x1F)  ) ? 1 : 0) ;
}

int peripherals_init (void)
{
  rcc_init();
  gpio_init();
  //rtc_init();
  systick_init();
  interrupt_init();
  usart_init();
  //spi_init();
  ethernet_init();
  //fs_init();
  //usb_init();
  //lcd_init();
  //touch_screen_init();
#if (DMA_ENABLE == 1)
  dma_init();	//init DMA for SPI only. Connection of SPI to DMA in read/write functions
#endif

#ifdef USART_SUPPORT
  usartinit();
#endif
  return 0;
}

void spi_peripheral_init()
{
  spi_init();
}
