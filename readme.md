## stm32f103 DAPLINK v1(HID) v1.3  
# whats new  
1.update cmsis dap code to v1.3(2.12)  
2.mod some header file to compile in keil 4.74  
3.cut packet buffer 64 to 4 ,support stm32f103C6T6A devices without heap crash.  
(it has 10k RAM,old C6T6 is same with C8T6 ,hidden total 20k RAM)  
4.remove a HID-ISP update funtion and linker script introduced by JX  
(reduce code size and support C6T6A 16k-32k flash)   
5.fix LEDs.
6.code clean and other impovements.  
chinese:  
更新dap协议源码1.3并重新适配，修改部分h文件兼容keil4.7编译
协议缓存包数减至4，防止c6t6a小芯片爆内存  
删除技新原内置的一套isp升级机制和link脚本，并减少flash占用   
修正LED灯  
其他代码优化  
  
##requirments:  
STLINK v2.1 schematics.  
St comp stm32f103series mcu.as low as C6T6A(something Gd32 never works)  
12MHZ Xosc(can be mod by system_stm32f103.c)   
Keil mdk4.74 (ac6 never works with usblib)  
  
## OLD readme:    
本工程从“技新”开源的DAPLINK修改来。修改 by：rush  
1，解决了原工程缺文件无法编译问题。测试使用MDK474编译.  
由于某个文件RTL.h戳中了keil的G点，你需要注册机给keil注册下RTOS的功能！  
2，原工程居然想当然去修改了设备名字！！！！导致了很多版本MDK无法识别！  
3，原工程注释掉部分描述符，不知道是不是复合设备不兼容他们的win7，  
总之这导致了在win10下复合设备不识别(看到只有串口没有HID)  
4，原工程USBlib使用lib，但是提供了一份源码，为了完全开源我们改用源码编译，  
其中部分inline导致无法编译，已去除。 
5，那个RL USB库不兼容GD等国产单片机，请老实花钱购买正常STM32芯片。  
6，电路图就是常见的老古董STLINK2.0，还能刷JLINK OB的那种。  
我晶振是12M，用8M自行修改。    