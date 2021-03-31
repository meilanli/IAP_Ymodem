IAP+Ymodem应用程序升级
2021-2-21 meilanli

## IAP实现：
    无需程序备份区，适用于小容量芯片。
	
    1.由APP接收升级指令，写入升级flag，软件复位
    2.启动，BL读取flag，进入Ymodem模式。如果没有升级标志，会进一步判断：系统不是由看门狗复位启动，则进入APP，否则说明APP有BUG导致跑死重启，那么还是会进入Ymodem模式。
    3.Ymodem收到文件信息包后，擦除APP应用区
    4.依次接收并写入新APP程序数据
    5.跳转到APP，由APP清除升级标志
    (由于BL不清除升级标志，所以，即便Ymodem接收过程中失败，重启后仍然进入升级模式,直到APP运行成功)

## IAP移植:
	app工程： 
		1.包含
			iap_app.c
			iap.h
		2.修改iap.h相关定义

	bootloader工程:
		1.包含源
			iap_bl.c
			iap.h
			ymodem.c/h
			ymodem_port.c/h
		2.修改ymodem_port.c/h,实现通信和保存接口
		3.修改iap.h相关定义
		4.iap_bl.c选择性修改（见注释）

	bl和app使用同一个iap.h，也就是说，改好一个，复制两份

## IAP使用说明：
	见Example

## Ymodem部分可以单独拿出来使用:
		a.包含ymodem下的文件。
		b.修改ymodem_port.c/h，实现通信和保存接口
		
##  更多说明 查看注释