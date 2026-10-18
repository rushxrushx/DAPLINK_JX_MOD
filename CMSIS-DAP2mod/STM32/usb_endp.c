/**
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Endpoint routines
*/


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

uint8_t USB_Rx_Buffer[64];

//dap
#include "DAP_config.h"
#include "..\DAP.h"

//dap buffer
static volatile uint8_t  USB_RequestFlag;       // Request  Buffer Usage Flag
static volatile uint32_t USB_RequestIn;         // Request  Buffer In  Index
static volatile uint32_t USB_RequestOut;        // Request  Buffer Out Index

static volatile uint8_t  USB_ResponseIdle;      // Response Buffer Idle  Flag
static volatile uint8_t  USB_ResponseFlag;      // Response Buffer Usage Flag
static volatile uint32_t USB_ResponseIn;        // Response Buffer In  Index
static volatile uint32_t USB_ResponseOut;       // Response Buffer Out Index

static          uint8_t  USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
static          uint8_t  USB_Response[DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Response Buffer

void endp_buffer_init(void)
{
	USB_RequestFlag   = 0;
	USB_RequestIn     = 0;
	USB_RequestOut    = 0;
	USB_ResponseIdle  = 1;
	USB_ResponseFlag  = 0;
	USB_ResponseIn    = 0;
	USB_ResponseOut   = 0;
}


void EP2_IN_Callback(void)
{
//DAP: when data needs to be prepared for the host
	if ((USB_ResponseOut != USB_ResponseIn) || USB_ResponseFlag)
	{	
		UserToPMABufferCopy(USB_Response[USB_ResponseOut], GetEPTxAddr(EP2_IN & 0x7F), DAP_PACKET_SIZE);
		SetEPTxCount((EP2_IN & 0x7F), DAP_PACKET_SIZE);
		USB_ResponseOut++;
		if (USB_ResponseOut == DAP_PACKET_COUNT)
			USB_ResponseOut = 0;
		if (USB_ResponseOut == USB_ResponseIn)
			USB_ResponseFlag = 0;

	}
	else
		USB_ResponseIdle = 1;

}


void EP1_OUT_Callback(void)
{
//DAP: when data is received from the host  
    uint16_t len;
	
	len=GetEPRxCount(EP1_OUT & 0x7F);//Get the received data count
	if (len == 0) goto END1;
	
	//get 1-byte data to check
	PMAToUserBufferCopy(USB_Rx_Buffer, GetEPRxAddr(EP1_OUT & 0x7F), 1);
	
	if (USB_Rx_Buffer[0] == ID_DAP_TransferAbort)
	{
		//to DAP
		DAP_TransferAbort = 1;
		
		goto END1;
	}
	if (USB_RequestFlag && (USB_RequestIn == USB_RequestOut))
		goto END1;  // Discard packet when buffer is full

	// Store 64-byte into request packet buffer
	PMAToUserBufferCopy(USB_Request[USB_RequestIn], GetEPRxAddr(EP1_OUT & 0x7F), len);

	USB_RequestIn++;
	if (USB_RequestIn == DAP_PACKET_COUNT)
		USB_RequestIn = 0;
	if (USB_RequestIn == USB_RequestOut)
		USB_RequestFlag = 1;
  
  END1:
  
  /* Enable the receive of data on EP3 */
  SetEPRxValid(ENDP1);
}


/*******************************************************************************
* Function Name  : SOF_Callback / INTR_SOFINTR_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
//  static uint32_t FrameCount = 0;
//  
//  if(bDeviceState == CONFIGURED)
//  {
//    if (++FrameCount == VCOMPORT_IN_FRAME_INTERVAL)
//    {
//      /* Reset the frame counter */
//      FrameCount = 0;
//    }
//  }  
  
    static uint32_t FrameCount = 0;
  
  if(bDeviceState == CONFIGURED)
  {
  } 
}



// Process USB HID Data
uint8_t usbd_dap_process (void)
{
	uint32_t n;

	// Process pending requests
	if ((USB_RequestOut != USB_RequestIn) || USB_RequestFlag)
	{
		//to DAP
		DAP_ProcessCommand(USB_Request[USB_RequestOut], USB_Response[USB_ResponseIn]);//to DAP

		// Update request index and flag
		n = USB_RequestOut + 1;
		if (n == DAP_PACKET_COUNT)
			n = 0;
		USB_RequestOut = n;

		if (USB_RequestOut == USB_RequestIn)
			USB_RequestFlag = 0;

		if (USB_ResponseIdle)//need to manual trigger send
		{	
            UserToPMABufferCopy(USB_Response[USB_ResponseIn], GetEPTxAddr(EP2_IN & 0x7F), DAP_PACKET_SIZE);
            SetEPTxCount((EP2_IN & 0x7F), DAP_PACKET_SIZE);
            SetEPTxValid(ENDP2); // 触发发送。一旦发送完成，会进入 EP2_IN_Callback 中断
			// Request that data is send back to host
			USB_ResponseIdle = 0;
		}
		else
		{	// Update response index and flag
			n = USB_ResponseIn + 1;
			if (n == DAP_PACKET_COUNT)
				n = 0;
			USB_ResponseIn = n;

			if (USB_ResponseIn == USB_ResponseOut)
				USB_ResponseFlag = 1;
		}
		return 1;
	}
	return 0;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

