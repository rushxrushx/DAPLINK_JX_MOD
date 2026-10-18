/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   All processing related to Virtual Com Port Demo
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
//#include "virtualComPort.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Request = 0;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    usbdev_init,
    usbdev_Reset,
    usbdev_Status_In,
    usbdev_Status_Out,
    usbdev_Data_Setup,
    usbdev_NoData_Setup,
    usbdev_Get_Interface_Setting,
    usbdev_GetDeviceDescriptor,
    usbdev_GetConfigDescriptor,
    usbdev_GetStringDescriptor,
    0,
    0x40 /*64 MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    usbdev_GetConfiguration,
    usbdev_SetConfiguration,
    usbdev_GetInterface,
    usbdev_SetInterface,
    usbdev_GetStatus,
    usbdev_ClearFeature,
    usbdev_SetEndPointFeature,
    usbdev_SetDeviceFeature,
    usbdev_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)DAPLink_DeviceDescriptor,
    DAPLink_DeviceDescriptorSize
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)DAPLink_ConfigDescriptor,
    DAPLink_ConfigDescriptorSize
  };

ONE_DESCRIPTOR String_Descriptor[4] =
  {
    {(uint8_t*)DAPLink_StringLangID, DAPLink_StringLangIDSize},
    {(uint8_t*)DAPLink_StringVendor, DAPLink_StringVendorSize},
    {(uint8_t*)DAPLink_StringProduct, DAPLink_StringProductSize},
    {(uint8_t*)DAPLink_StringSerial, DAPLink_StringSerialSize}
  };

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : usbdev_init.
* Description    : Virtual COM Port Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_init(void)
{

  /* Update the serial number string descriptor with the data from the unique
  ID*/
  //Get_SerialNum();

  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  /* configure the USART to the default settings */
  //USART_Config_Default();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : usbdev_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_Reset(void)
{
extern void endp_buffer_init();

  /* Set Virtual_Com_Port DEVICE as not configured */
  pInformation->Current_Configuration = 0;

  /* Current Feature initialization */
  //pInformation->Current_Feature = DAPLink_ConfigDescriptor[7];

  /* Set Virtual_Com_Port DEVICE with the default Interface*/
  //pInformation->Current_Interface = 0;

  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_STALL);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_ADDR);
  SetEPTxCount(ENDP1, 64);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPRxStatus(ENDP1, EP_TX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_BULK);
  SetEPRxAddr(ENDP2, ENDP2_ADDR);
  SetEPRxCount(ENDP2, 64);
  SetEPTxStatus(ENDP2, EP_TX_NAK);
  SetEPRxStatus(ENDP2, EP_RX_DIS);

  /* Initialize Endpoint 3 */


  /* Set this device to response on default address */
  SetDeviceAddress(0);
  
  endp_buffer_init();
  
  bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : usbdev_SetConfiguration.
* Description    : Update the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
* Function Name  : usbdev_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : usbdev_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_Status_In(void)
{
}

/*******************************************************************************
* Function Name  : usbdev_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usbdev_Status_Out(void)
{
}



/* 专门用于返回 WinUSB 描述符物理指针的函数 */
uint8_t *DAPLink_GetWinUSBCompatibleID_Pointer(uint16_t Length)
{
    // 无论是哪个阶段调用，直接返回数据首地址
    return (uint8_t *)&DAPLink_MSOSCompatibleIDDescriptor;
}

/*******************************************************************************
* Function Name  : usbdev_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT usbdev_Data_Setup(uint8_t RequestNo)
{

    /* 检查是否为微软特有的 WinUSB 请求 */
    if (RequestNo == USB_REQ_MS_VENDOR_CODE)
    {
        /* wIndex 等于 0x0004 代表请求 Compatible ID */
        if (pInformation->USBwIndex == 0x0004)
        {
            /* 设置返回数据和长度 */
            pInformation->Ctrl_Info.CopyData = DAPLink_GetWinUSBCompatibleID_Pointer;
            pInformation->Ctrl_Info.Usb_wLength = sizeof(DAPLink_MSOSCompatibleIDDescriptor);
            pInformation->Ctrl_Info.Usb_wOffset = 0;
            return USB_SUCCESS;
        }
    }
    
    /* 正常的标准类请求或者原有的DAPLink特殊请求流程... */
    return USB_UNSUPPORT;

}

/*******************************************************************************
* Function Name  : usbdev_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT usbdev_NoData_Setup(uint8_t RequestNo)
{


  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : usbdev_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *usbdev_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : usbdev_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *usbdev_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor);
}


/* 专门用于返回 OS 字符串物理指针的函数 */
uint8_t *DAPLink_GetMSOSString_Pointer(uint16_t Length)
{
    return (uint8_t *)&DAPLink_StringMSOS;
}

/*******************************************************************************
* Function Name  : usbdev_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *usbdev_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;
  
  // 在对应的初始化或获取逻辑中加入：
if (wValue0 == 0xEE) 
{
    // 如果 PC 索取 0xEE 字符串，返回微软 OS 专属描述符
    pInformation->Ctrl_Info.CopyData = DAPLink_GetMSOSString_Pointer;
    pInformation->Ctrl_Info.Usb_wLength = sizeof(DAPLink_StringMSOS);
    pInformation->Ctrl_Info.Usb_wOffset = 0;
    return USB_SUCCESS;
}
  
  if (wValue0 >= 4)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}

/*******************************************************************************
* Function Name  : usbdev_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT usbdev_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT;
  }
  else if (Interface > 1)
  {
    return USB_UNSUPPORT;
  }
  return USB_SUCCESS;
}



