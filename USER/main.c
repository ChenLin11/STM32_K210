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
#include "exti.h"
#include "pwm.h"

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
	LED0=1;	
	FLAG_PlayMusic = 0;//����������

	usmart_dev.init(84);		//��ʼ��USMART
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ�� 
	EXTIX_Init();       //��ʼ���ⲿ�жϺ���	
	TIM3_PWM_Init(199,7199);//1/(72/(psc+1))*(arr+1)	���ڶ����PWM��ʼ��
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
 	
	
	FLAG_PlayMusic = 1;  
	
	while(1)
	{ 
		
		if(FLAG_PlayMusic){//�����������
			audio_play();
		}
		TIM_SetCompare1(TIM3,170);//170-195�ķ�Χ
		TIM_SetCompare1(TIM3,180);//û+10,���ת��+90��
		key=KEY_Scan(0);		//�õ���ֵ��ģʽ����֧����������
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//key_up��
					sendMessage("A");//�������������Ϣ
					break;
				case KEY0_PRES:	//key0��
					sendMessage("C");//����ʶ��������Ϣ
					break;
				case KEY1_PRES:	//key1��
					sendMessage("D");//����ɾ��������Ϣ
					break;
//				case KEY2_PRES:	//key2��
//					sendMessage(s4);
//					break;
			}
		}else delay_ms(10); 
	} 	
	
				  	
}
void sendMessage(char *s){//��K210������Ӧָ��
	int t;
	for(t=0;t<2;t++){
		USART_SendData(USART1, s[t]);         //�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}
