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

void sendMessage(char* s);//�����͵���Ϣ
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}

int main(void)
{
	u8 key;           //�����ֵ

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	delay_init(168);  //��ʼ����ʱ����
	LED_Init();				//��ʼ��LED�˿� 
	
	uart_init(115200); //��ʼ������1
	KEY_Init();       //��ʼ�������ӿ�
	FLAG_PlayMusic = 0;//����������
	FLAG_Face_True = 0;
	FLAG_Face_False = 0;

	usmart_dev.init(84);		//��ʼ��USMART
 	LCD_Init();					//LCD��ʼ��  
	EXTIX_Init();       //��ʼ���ⲿ�жϺ���	

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
//	while(font_init()) 			//����ֿ�
//	{	    
//		LCD_ShowString(30,50,200,16,16,"Font Error!");
//		delay_ms(200);				  
//		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
//		delay_ms(200);				  
//	}  	 
//	POINT_COLOR=RED;  
	TIM3_PWM_Init(199,7199);//1/(72/(psc+1))*(arr+1)	���ڶ����PWM��ʼ��
//	TIM_SetCompare1(TIM3,180);//ÿ+10,���ת��+90��
//	delay_ms(1000);
	TIM_SetCompare1(TIM3,170);//Ĭ��Ϊ�ر�״̬
	delay_ms(1000);
	while(1)
	{ 
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	 
		if(FLAG_PlayMusic){//�����������
			audio_play();
		}
		
		key=KEY_Scan(0);		//�õ���ֵ��ģʽ����֧����������
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//key_up��
					sendMessage("AAA");//�������������Ϣ
					LED1 = !LED1;
					break;
				case KEY1_PRES:	//key1��
					sendMessage("DDD");//����ɾ��������Ϣ
					LED0 = !LED0;
					break;
				case KEY0_PRES:
					sendMessage("RRR");//�鿴��������ͼ��
					break;
				case KEY2_PRES:
					sendMessage("GGG");//ɾ����������ͼ��
					break;
			}
		}else delay_ms(10); 
	} 	
	
				  	
}
void sendMessage(char *s){//��K210����ָ��
	int t;
	for(t=0;t<3;t++){
		USART_SendData(USART1, s[t]);         //�򴮿ڷ�������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
