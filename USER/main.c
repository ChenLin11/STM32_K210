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

extern u8 FLAG_PlayMusic;
void sendMessage(char* s);//�����͵���Ϣ
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

int main(void)
{
	u8 key;           //�����ֵ
	u8 t;
	u8 len;	
	u16 times=0;
	char s1[18] = {"STM32:order1"};//��Ӧ�ĸ�����
	char s2[18] = {"STM32:order2"};
	char s3[18] = {"STM32:order3"};
	char s4[18] = {"STM32:order4"};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	delay_init(168);  //��ʼ����ʱ����
	LED_Init();				//��ʼ��LED�˿� 
	BEEP_Init();      //��ʼ���������ӿ�
	KEY_Init();       //��ʼ�������ӿ�
	uart_init(115200); //��ʼ������
	LED0=0;	
	FLAG_PlayMusic = 0;//����������

	usmart_dev.init(84);		//��ʼ��USMART
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	W25QXX_Init();				//��ʼ��W25Q128
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(40,40);	//������������
	WM8978_SPKvol_Set(50);		//������������
		
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
		f_mount(fs[0],"0:",1); 	//����SD��  
  	f_mount(fs[1],"1:",1); 	//����SD��  
  	f_mount(fs[2],"2:",1); 	//����U��
	POINT_COLOR=RED;      
	while(font_init()) 			//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(60,50,200,16,"Explorer STM32F4������",16,0);				    	 
	Show_Str(60,70,200,16,"���ֲ�����ʵ��",16,0);				    	 
	Show_Str(60,90,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(60,110,200,16,"2014��5��24��",16,0);
	Show_Str(60,130,200,16,"KEY0:NEXT   KEY2:PREV",16,0); 
	Show_Str(60,150,200,16,"KEY_UP:PAUSE/PLAY",16,0);
	
	FLAG_PlayMusic = 1;  
	
	while(1)
	{ 
		
		if(FLAG_PlayMusic){//�����������
			audio_play();
			LED1 = 1;
		}
		else{
			delay_ms(1);
			t++;
			if(t==100)
			{
				LED0=!LED0;
				t=0;
			}
		}
		
		
	} 	
	
				  	
	while(0)
	{
		key=KEY_Scan(0);		//�õ���ֵ��ģʽ����֧����������
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//key_up��
					//BEEP=!BEEP;
					LED0 = 0;
					LED1 = 0;
//					sendMessage(s1);
					sendMessage("A");
					break;
				case KEY0_PRES:	//key0��
					LED0=!LED0;
					sendMessage(s2);
					break;
				case KEY1_PRES:	//key1��
					LED1=!LED1;
//					sendMessage(s3);
					sendMessage("C");
					break;
				case KEY2_PRES:	//key2��
					LED0=1;
					LED1=1;
					sendMessage(s4);
					break;
			}
		}else delay_ms(10); 
		
	}//while	
}
void sendMessage(char *s){//��K210������Ӧָ��
	int t;
	for(t=0;t<18;t++){
		USART_SendData(USART1, s[t]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
