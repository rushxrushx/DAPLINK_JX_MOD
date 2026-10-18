
#include <string.h>
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"

#include "DAP_config.h"

void Handle_USBAsynchXfer (void)
{

}




// USB HID Callback: when data needs to be prepared for the host
int usbd_hid_get_report (uint8_t rtype, uint8_t rid, uint8_t *buf, uint8_t req)
{
	switch (rtype)
	{
	case HID_REPORT_INPUT:
		switch (req)
		{
		case USBD_HID_REQ_EP_CTRL:
		case USBD_HID_REQ_PERIOD_UPDATE:
			break;
		case USBD_HID_REQ_EP_INT:
			if ((USB_ResponseOut != USB_ResponseIn) || USB_ResponseFlag)
			{
				memcpy(buf, USB_Response[USB_ResponseOut], DAP_PACKET_SIZE);
				USB_ResponseOut++;
				if (USB_ResponseOut == DAP_PACKET_COUNT)
					USB_ResponseOut = 0;
				if (USB_ResponseOut == USB_ResponseIn)
					USB_ResponseFlag = 0;
				return (DAP_PACKET_SIZE);
			}
			else
				USB_ResponseIdle = 1;
			break;
		}
		break;
	case HID_REPORT_FEATURE:
		break;
	}
	return (0);
}

// USB HID Callback: when data is received from the host
void dap_usb_receive( uint8_t *buf, uint16_t len)
{

		if (len == 0)
			break;
		if (buf[0] == ID_DAP_TransferAbort)
		{
			//to DAP
			DAP_TransferAbort = 1;
			
			break;
		}
		if (USB_RequestFlag && (USB_RequestIn == USB_RequestOut))
			break;  // Discard packet when buffer is full

		// Store data into request packet buffer
		memcpy(USB_Request[USB_RequestIn], buf, len);

		USB_RequestIn++;
		if (USB_RequestIn == DAP_PACKET_COUNT)
			USB_RequestIn = 0;
		if (USB_RequestIn == USB_RequestOut)
			USB_RequestFlag = 1;

}





