#define TIMEOUT_DELAY	10000

#include <stdio.h>

#include <RTL.h>
#include <rl_usb.h>
#include <stm32f10x.h>

#define  __NO_USB_LIB_C
#include "usb_config.c"

#include "DAP_config.h"
#include "..\DAP.h"

#include "usbd_user_cdc_acm.h"

uint8_t usbd_hid_process(void);

void LedConnectedOut(uint16_t bit);
void LedRunningOut(uint16_t bit);

void Delay_ms(uint32_t delay);

void NotifyOnStatusChange (void);
void BoardInit(void);


#if (USBD_CDC_ACM_ENABLE == 1)
	int32_t usb_rx_ch;
	int32_t usb_tx_ch;
#endif

uint32_t led_count;
uint32_t led_timeout;

/**
  * @brief	LED functions
  *
  */
void LedConnectedOn(void)		{	LED_CONNECTED_PORT->BRR = LED_CONNECTED;	}
void LedConnectedOff(void)		{	LED_CONNECTED_PORT->BSRR  = LED_CONNECTED;	}
void LedConnectedToggle(void)	{	LED_CONNECTED_PORT->ODR ^= LED_CONNECTED;	}

void LedRunningOn(void)			{	LED_RUNNING_PORT->BRR   = LED_RUNNING;		}
void LedRunningOff(void)		{	LED_RUNNING_PORT->BSRR    = LED_RUNNING;		}
void LedRunningToggle(void)		{	LED_RUNNING_PORT->ODR   ^= LED_RUNNING;		}

const GPIO_InitTypeDef INIT_PINS_LED = {
	(LED_CONNECTED | LED_RUNNING),
	GPIO_Speed_2MHz,
	GPIO_Mode_Out_PP
};

void LEDS_SETUP (void)
{
	RCC->APB2ENR |= LED_CONNECTED_RCC;
	LED_CONNECTED_PORT->BRR = (LED_CONNECTED | LED_RUNNING);
	GPIO_INIT(LED_CONNECTED_PORT, INIT_PINS_LED);
}

void LedConnectedOut(uint16_t bit)
{
	if (bit & 1)	LedConnectedOn();
	else			LedConnectedOff();
}
void LedRunningOut(uint16_t bit)
{
	if (bit & 1)	LedConnectedOn();
	else			LedConnectedOff();
}

// Delay for specified time
//    delay:  delay time in ms
void Delay_ms(uint32_t delay)
{
	delay *= (CPU_CLOCK / 1000 + (DELAY_SLOW_CYCLES - 1)) / (2 * DELAY_SLOW_CYCLES);
	PIN_DELAY_SLOW(delay);
}

extern uint32_t __Vectors;

void HardFault_Handler(void);
void NMI_Handler(void)			__attribute((alias("HardFault_Handler")));
void MemManage_Handler(void)	__attribute((alias("HardFault_Handler")));
void BusFault_Handler(void)		__attribute((alias("HardFault_Handler")));
void UsageFault_Handler(void)	__attribute((alias("HardFault_Handler")));
void SVC_Handler(void)			__attribute((alias("HardFault_Handler")));
void DebugMon_Handler(void)		__attribute((alias("HardFault_Handler")));
void PendSV_Handler(void)		__attribute((alias("HardFault_Handler")));

void HardFault_Handler(void)
{
	__disable_irq();
	__set_MSP(__Vectors);
	LEDS_SETUP();
	{
		register int count;
		for (count = 0; count < 5; count++)
		{
			LedRunningOn();
			Delay_ms(250);
			LedRunningOff();

			LedConnectedOn();
			Delay_ms(250);
			LedConnectedOff();

			Delay_ms(1000);
		}
	}
	NVIC_SystemReset();
}



#if (USBD_CDC_ACM_ENABLE == 1)
void send_char(char ch)
{
	if (ch == '\n')
		send_char('\r');
	while (USBD_CDC_ACM_PutChar(ch) != ch)
	{ }
}


int fputc(int ch, FILE *f)
{
	send_char(ch);
	return ch;
}
#endif






#if (USBD_CDC_ACM_ENABLE == 1)

/* Check if status has changed and if so, send notify to USB Host on Int EP   */
void NotifyOnStatusChange (void)
{
	static int32_t old_notify = -1;
	int32_t status, notify = 0;

	status = UART_GetCommunicationErrorStatus();

	if (status & UART_OVERRUN_ERROR_Msk)
		notify |= CDC_SERIAL_STATE_OVERRUN;
	if (status & UART_PARITY_ERROR_Msk )
		notify |= CDC_SERIAL_STATE_OVERRUN;
	if (status & UART_FRAMING_ERROR_Msk)
		notify |= CDC_SERIAL_STATE_FRAMING;
	
	status	= UART_GetStatusLineState();	
	
	if (status & UART_STATUS_LINE_RI_Msk )
		notify |= CDC_SERIAL_STATE_RING;
	if (status & UART_STATUS_LINE_DSR_Msk)
		notify |= CDC_SERIAL_STATE_TX_CARRIER;
	if (status & UART_STATUS_LINE_DCD_Msk)
		notify |= CDC_SERIAL_STATE_RX_CARRIER;
	
	if (UART_GetBreak())
		notify |= CDC_SERIAL_STATE_BREAK;
	
	if (notify ^ old_notify)				// If notify changed
	{
		if (USBD_CDC_ACM_Notify (notify))   // Send new notification
			old_notify = notify;
	}
}

#endif

const GPIO_InitTypeDef INIT_PINS_A = {
	(	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
		GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
		GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
//		GPIO_Pin_11 | GPIO_Pin_12 |		// USB pins
//		GPIO_Pin_13 | GPIO_Pin_14 |		// SWD pins
		GPIO_Pin_15
	),
	(GPIOSpeed_TypeDef)0,
	GPIO_Mode_AIN
};
const GPIO_InitTypeDef INIT_PINS_B = {
	GPIO_Pin_All,
	(GPIOSpeed_TypeDef)0,
	GPIO_Mode_AIN
};
const GPIO_InitTypeDef INIT_PINS_C = {
	(GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15),
	(GPIOSpeed_TypeDef)0,
	GPIO_Mode_AIN
};

const GPIO_InitTypeDef GPIOPINS_B12 = {
	GPIO_Pin_12 ,
	(GPIOSpeed_TypeDef)0,
	GPIO_Mode_IPU
};

void BoardInit(void)
{
	// Enable GPIOA-GPIOC
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);

	// Enable SWJ only
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_INIT(GPIOA, INIT_PINS_A);
	GPIO_INIT(GPIOB, INIT_PINS_B);
	GPIO_INIT(GPIOC, INIT_PINS_C);
	GPIO_INIT(GPIOB, GPIOPINS_B12);//PB12 pullup of swDAT

	LEDS_SETUP();
}

void USBD_Error_Event(void)
{
	LedConnectedOn();
	LedRunningOn();

	usbd_connect(__FALSE);
	usbd_reset_core();

	HardFault_Handler();
}



/**
  * @brief	Main
  *
  */
int main(void)
{
	BoardInit();
	SystemCoreClockUpdate();
	
	//enable DWT core timer for new daplink use
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
	//RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5); // 72/1.5=48MHz
	//RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);// for 48mhz cpu

	LedConnectedOn();
	LedRunningOff();
	
	//to DAP
	DAP_Setup();
	
	LedConnectedOff();

	Delay_ms(1);

	// USB Device Initialization and connect
	usbd_init();
	usbd_connect(__TRUE);

	led_count = 0;
	while (!usbd_configured())	// Wait for USB Device to configure
	{
		if (led_count++ == 0)
			LedConnectedOn();
		else if (led_count == 5)
			LedConnectedOff();
		else if (led_count == 20)
			led_count = 0;
		Delay_ms(10);
	}
	LedConnectedOn();
	Delay_ms(100);				// Wait for 100ms

	led_count = 0;
	led_timeout = TIMEOUT_DELAY;
	
#if (USBD_CDC_ACM_ENABLE == 1)
	usb_rx_ch = -1;
	usb_tx_ch = -1;
#endif

	while (1)
	{
		if (!usbd_hid_process())	// No packet processing
		{
			if (led_timeout == 0)
			{
				LedRunningOff();
			}
			else 
			led_timeout--;
		}
		else
		{
			LedRunningOn();
			led_timeout = TIMEOUT_DELAY;
			
		}

#if (USBD_CDC_ACM_ENABLE == 1)

		NotifyOnStatusChange();

		// USB -> UART
		if (usb_rx_ch == -1)
			usb_rx_ch = USBD_CDC_ACM_GetChar();

		if (usb_rx_ch != -1)
		{
			if (UART_PutChar (usb_rx_ch) == usb_rx_ch)
				usb_rx_ch = -1;
		}

		// UART -> USB
		if (usb_tx_ch == -1)
			usb_tx_ch = UART_GetChar();

		if (usb_tx_ch != -1)
		{
			if (USBD_CDC_ACM_PutChar(usb_tx_ch) == usb_tx_ch)
				usb_tx_ch = -1;
		}
#endif
	}
}
