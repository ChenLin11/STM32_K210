#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"  
#include "lcd.h"
#include "usmart.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	 
#include "wm8978.h"	 
#include "audioplay.h"	
#include "usbh_usr.h" 

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

void sendMessage(char* s);//待发送的信息
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

u8 USH_User_App(void){//usb响应函数

	return 0;
}
int main(void)
{
	u8 key;           //保存键值
	u8 t;
	u8 len;	
	u16 times=0;  
	char s1[18] = {"STM32:order1"};//对应四个命令
	char s2[18] = {"STM32:order2"};
	char s3[18] = {"STM32:order3"};
	char s4[18] = {"STM32:order4"};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	LED_Init();				//初始化LED端口 
	BEEP_Init();      //初始化蜂鸣器接口
	KEY_Init();       //初始化按键接口
	uart_init(115200); //初始化串口
	LED0=0;	


	usmart_dev.init(84);		//初始化USMART
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	W25QXX_Init();				//初始化W25Q128
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(40,40);	//耳机音量设置
	WM8978_SPKvol_Set(50);		//喇叭音量设置
		
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	f_mount(fs[2],"2:",1); 	//挂载U盘
	POINT_COLOR=RED;      
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(60,50,200,16,"Explorer STM32F4开发板",16,0);				    	 
	Show_Str(60,70,200,16,"音乐播放器实验",16,0);				    	 
	Show_Str(60,90,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(60,110,200,16,"2014年5月24日",16,0);
	Show_Str(60,130,200,16,"KEY0:NEXT   KEY2:PREV",16,0); 
	Show_Str(60,150,200,16,"KEY_UP:PAUSE/PLAY",16,0);
	
	
	//初始化USB主机
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
	while(1)
	{ 
		//该函数必须循环调用,仅在U盘识别阶段,识别后,由USB中断处理其他操作(读写)
		USBH_Process(&USB_OTG_Core, &USB_Host);
		delay_ms(1);
		t++;
		if(t==100)
		{
			LED0=!LED0;
			t=0;
		}
		audio_play();
	} 	
	
				  	
	while(1)
	{
		key=KEY_Scan(0);		//得到键值，模式：不支持连续按键
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//key_up键
					//BEEP=!BEEP;
					LED0 = 0;
					LED1 = 0;
//					sendMessage(s1);
					sendMessage("A");
					break;
				case KEY0_PRES:	//key0键
					LED0=!LED0;
					sendMessage(s2);
					break;
				case KEY1_PRES:	//key1键
					LED1=!LED1;
//					sendMessage(s3);
					sendMessage("C");
					break;
				case KEY2_PRES:	//key2键
					LED0=1;
					LED1=1;
					sendMessage(s4);
					break;
			}
		}else delay_ms(10); 
		
	}//while	
}
void sendMessage(char *s){//向K210发送响应指令
	int t;
	for(t=0;t<18;t++){
		USART_SendData(USART1, s[t]);         //向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
