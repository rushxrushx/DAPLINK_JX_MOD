
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define VIRTUAL_COM_PORT_DATA_SIZE              64
#define VIRTUAL_COM_PORT_INT_SIZE               8


#define STANDARD_ENDPOINT_DESC_SIZE             0x09

#define DAPLink_DeviceDescriptorSize 18
extern const uint8_t DAPLink_DeviceDescriptor[DAPLink_DeviceDescriptorSize];

#define DAPLink_ConfigDescriptorSize 32
extern const uint8_t DAPLink_ConfigDescriptor[DAPLink_ConfigDescriptorSize];

#define  DAPLink_StringLangIDSize 4
extern const uint8_t DAPLink_StringLangID[DAPLink_StringLangIDSize];

#define DAPLink_StringVendorSize 8
extern const uint8_t DAPLink_StringVendor[DAPLink_StringVendorSize];

#define DAPLink_StringProductSize 0x24
extern const uint8_t DAPLink_StringProduct[DAPLink_StringProductSize];

#define DAPLink_StringSerialSize 0x0a
extern const uint8_t DAPLink_StringSerial[DAPLink_StringSerialSize];


#define USB_REQ_MS_VENDOR_CODE   0x34   /* 自定义供应商请求代码，后续控制传输判定要用到 */

#define DAPLink_StringMSOSSize 0x12
extern const uint8_t DAPLink_StringMSOS[DAPLink_StringMSOSSize];

#define DAPLink_MSOSCompatibleIDDescriptorSize 0x28
extern const uint8_t DAPLink_MSOSCompatibleIDDescriptor[DAPLink_MSOSCompatibleIDDescriptorSize];


#endif /* __USB_DESC_H */

