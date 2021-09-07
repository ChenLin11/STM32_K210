#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
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
#include "exti.h"
#include "pwm.h"

extern u8 FLAG_PlayMusic;
extern u8 FLAG_Face_True;
extern u8 FLAG_Face_False;

void sendMessage(char* s);//待发送的信息
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

int main(void)
{
	u8 key;           //保存键值

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	LED_Init();				//初始化LED端口 
	BEEP_Init();      //初始化蜂鸣器接口
	KEY_Init();       //初始化按键接口
	uart_init(115200); //初始化串口1
	//uart3_init(115200); //初始化串口3
	FLAG_PlayMusic = 0;//不播放音乐
	FLAG_Face_True = 0;
	FLAG_Face_False = 0;

	usmart_dev.init(84);		//初始化USMART
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化 
	EXTIX_Init();       //初始化外部中断函数	
	TIM3_PWM_Init(199,7199);//1/(72/(psc+1))*(arr+1)	用于舵机的PWM初始化
	W25QXX_Init();				//初始化W25Q128
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(40,40);	//耳机音量设置
	WM8978_SPKvol_Set(50);		//喇叭音量设置
		
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
		f_mount(fs[0],"0:",1); 	//挂载SD卡  
  	f_mount(fs[1],"1:",1); 	//挂载SD卡  
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
 	
	
	FLAG_PlayMusic = 1;  
	
	while(1)
	{ 
		LCD_Fill(30,50,240,66,WHITE);//清除显示	 
		if(FLAG_PlayMusic){//如果播放音乐
			audio_play();
		}
		TIM_SetCompare1(TIM3,170);//170-195的范围
		TIM_SetCompare1(TIM3,180);//没+10,舵机转角+90°
		key=KEY_Scan(0);		//得到键值，模式：不支持连续按键
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//key_up键
					sendMessage("AA");//用于添加人脸信息
					break;
//				case KEY0_PRES:	//key0键
//					sendMessage("CC");//用于识别人脸信息
//					break;
				case KEY1_PRES:	//key1键
					sendMessage("DD");//用于删除人脸信息
					break;
//				case KEY2_PRES:	//key2键
//					sendMessage("RR");
//					break;
			}
		}else delay_ms(10); 
	} 	
	
				  	
}
void sendMessage(char *s){//向K210发送指令
	int t;
	for(t=0;t<2;t++){
		USART_SendData(USART1, s[t]);         //向串口发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
}
