
/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"



//dapv2 winusb bulk mode desc by google gemini

const uint8_t DAPLink_DeviceDescriptor[DAPLink_DeviceDescriptorSize] = {
    0x12,                       /* bLength (18字节) */
    0x01,                       /* bDescriptorType (Device) */
    0x00, 0x02,                 /* bcdUSB (设置为 USB 2.0，对 WinUSB 识别至关重要) */
    0x00,                       /* bDeviceClass (接口独立定义) */
    0x00,                       /* bDeviceSubClass */
    0x00,                       /* bDeviceProtocol */
    0x40,                       /* bMaxPacketSize0 (64 字节) */
    0x51, 0xC2,                 /* idVendor (0xC251) */
    0x01, 0xF0,                 /* idProduct (0xF001) */
    0x00, 0x01,                 /* bcdDevice (v1.00) */
    0x01,                       /* iManufacturer (字符串索引 1) */
    0x02,                       /* iProduct (字符串索引 2) */
    0x03,                       /* iSerialNumber (字符串索引 3) */
    0x01                        /* bNumConfigurations (1个配置) */
};

const uint8_t DAPLink_ConfigDescriptor[DAPLink_ConfigDescriptorSize] = {
    /* 1. 配置描述符 */
    0x09,                       /* bLength */
    0x02,                       /* bDescriptorType (Configuration) */
    0x20, 0x00,                 /* wTotalLength (32 字节) */
    0x01,                       /* bNumInterfaces (仅 1 个接口) */
    0x01,                       /* bConfigurationValue */
    0x00,                       /* iConfiguration String Index */
    0x80,                       /* bmAttributes (总线供电) */
    0x32,                       /* bMaxPower (100 mA) */

    /* 2. 接口描述符 (DAPLink Bulk 接口) */
    0x09,                       /* bLength */
    0x04,                       /* bDescriptorType (Interface) */
    0x00,                       /* bInterfaceNumber (接口 0) */
    0x00,                       /* bAlternateSetting */
    0x02,                       /* bNumEndpoints (2 个端点：Bulk IN + Bulk OUT) */
    0xFF,                       /* bInterfaceClass (0xFF = Vendor Specific, WinUSB 规范) */
    0x00,                       /* bInterfaceSubClass (自定义子类) */
    0x00,                       /* bInterfaceProtocol (自定义协议) */
    0x04,                       /* iInterface (可选指向 "CMSIS-DAP" 字符串描述符索引 4) */

    /* 3. 端点描述符 (EP1 Bulk OUT) */
    0x07,                       /* bLength */
    0x05,                       /* bDescriptorType (Endpoint) */
    0x01,                       /* bEndpointAddress (EP1 OUT) */
    0x02,                       /* bmAttributes (0x02 = Bulk 批量传输) */
    0x40, 0x00,                 /* wMaxPacketSize (全速下固定为 64 字节) */
    0x00,                       /* bInterval (Bulk 端点此值无效，填 0) */

    /* 4. 端点描述符 (EP2 Bulk IN) */
    0x07,                       /* bLength */
    0x05,                       /* bDescriptorType (Endpoint) */
    0x82,                       /* bEndpointAddress (EP2 IN) */
    0x02,                       /* bmAttributes (0x02 = Bulk 批量传输) */
    0x40, 0x00,                 /* wMaxPacketSize (全速下固定为 64 字节) */
    0x00                        /* bInterval (Bulk 端点此值无效，填 0) */
};

/* 语言 ID (0x0409 - 英文) */
const uint8_t DAPLink_StringLangID[DAPLink_StringLangIDSize] = { 0x04, 0x03, 0x09, 0x04 };

/* 厂家名称: "ARM" */
const uint8_t DAPLink_StringVendor[DAPLink_StringVendorSize] = { 0x08, 0x03, 'A', 0, 'R', 0, 'M', 0 };

/* 产品名称: "DAPLink CMSIS-DAP" */
const uint8_t DAPLink_StringProduct[DAPLink_StringProductSize] = {
    0x24, 0x03, 
    'D',0,'A',0,'P',0,'L',0,'i',0,'n',0,'k',0,' ',0,
    'C',0,'M',0,'S',0,'I',0,'S',0,'-',0,'D',0,'A',0,'P',0
};

/* 序列号 */
const uint8_t DAPLink_StringSerial[DAPLink_StringSerialSize] = {
    0x0A, 0x03, 
    '1',0,'0',0,'0',0,'0',0
};

/* 接口名称: "CMSIS-DAP" */
const uint8_t DAPLink_StringInterface[] = {
    0x14, 0x03,
    'C',0,'M',0,'S',0,'I',0,'S',0,'-',0,'D',0,'A',0,'P',0
};
const uint16_t DAPLink_StringInterfaceSize=0x14;

//增加：Microsoft OS 1.0 字符串描述符
//当 WinUSB 设备插入电脑时，Windows 会主动去获取索引为 0xEE 的字符串。我们必须在代码里提供这个。
//其核心标记是包含了字符 MSFT100，并且尾部带一个自定义的 Vendor Code（这里设为 0x34）


const uint8_t DAPLink_StringMSOS[DAPLink_StringMSOSSize] = {
    0x12,                       /* 长度：18字节 */
    0x03,                       /* 描述符类型：String */
    'M', 0, 'S', 0, 'F', 0, 'T', 0, '1', 0, '0', 0, '0', 0, /* "MSFT100" */
    USB_REQ_MS_VENDOR_CODE,     /* Vendor Code：0x34 */
    0x00                        /* 填充字节 */
};

//增加：Microsoft OS 兼容 ID 特征描述符 (Compatible ID)
//当 Windows 读到了上面的 0xEE 字符串并验证了 MSFT100 之后，会接着向单片机发送一个 Setup 控制传输包
//（bRequest 便是我们刚才指定的 0x34，wIndex 设为 0x0004），此时单片机必须返回这个扩展特征描述符，里面声明了系统将其视作 WINUSB
const uint8_t DAPLink_MSOSCompatibleIDDescriptor[DAPLink_MSOSCompatibleIDDescriptorSize] = {
    /* ---- Windows OS String Descriptor Header ---- */
    0x28, 0x00, 0x00, 0x00,     /* 总长度：40 字节 (0x00000028) */
    0x00, 0x01,                 /* 版本号：Version 1.0 (0x0100) */
    0x04, 0x00,                 /* 扩展特性索引 (wIndex)：0x0004 (Compatibility ID) */
    0x01,                       /* 包含的分组/接口节数量：1 个节 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 保留 7 字节 */

    /* ---- 接口 0 节 (Function Section) ---- */
    0x00,                       /* 本节对应的接口编号：Interface 0 */
    0x01,                       /* 保留，必须填 1 */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, /* 核心：Compatible ID 固定为 "WINUSB"，长8字节 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Sub-Compatible ID 空，长8字节 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00          /* 保留 6 字节 */
};
