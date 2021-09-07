#include "audioplay.h"
#include "ff.h"
#include "malloc.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "key.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"  
#include "usbh_usr.h" 
#include "exti.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//���ֲ����� Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 
//���ֲ��ű�־
u8 FLAG_PlayMusic;  
extern u8 FLAG_Face_True;   //������֤�ɹ�
extern u8 FLAG_Face_False;   //������֤��ʧ��

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

u8 USH_User_App(void){//usb��Ӧ����
	u8 res;
 	DIR wavdir;	 		//Ŀ¼
	FILINFO wavfileinfo;//�ļ���Ϣ
	u8 *fn;   			//���ļ���
	u8 *pname;			//��·�����ļ���
	u16 totwavnum; 		//�����ļ�����
	u16 *wavindextbl;	//����������
	
	WM8978_ADDA_Cfg(1,0);	//����DAC
	WM8978_Input_Cfg(0,0,0);//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);	//����DAC��� 
	while(f_opendir(&wavdir,"2:/MUSIC"))//�������ļ���
 	{	    
		Show_Str(60,190,240,16,"MUSIC�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 									  
	totwavnum=audio_get_tnum("2:/MUSIC"); //�õ�����Ч�ļ���
 									   
  	wavfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	wavfileinfo.lfname=mymalloc(SRAMIN,wavfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,wavfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
 	wavindextbl=mymalloc(SRAMIN,2*totwavnum);				//����2*totwavnum���ֽڵ��ڴ�,���ڴ�������ļ�����
 	 	 
  res=f_opendir(&wavdir,(const TCHAR*)"2:/MUSIC"); 	//��Ŀ¼
	if(res==FR_OK)//�򿪳ɹ�
	{	
		if(FLAG_PlayMusic){//���Ҫ�������֣�˵��ʶ�����˸�����������ж�
			FLAG_PlayMusic = 0;
			LCD_Fill(60,190,240,190+16,WHITE);				//���֮ǰ����ʾ
			
			if(FLAG_Face_True){//��֤�ɹ�
				FLAG_Face_True = 0;
				fn = (u8*)"ring.wav";
				Show_Str(60,190,240-60,16,"ʶ��ɹ�",16,0);				//��ʾ�������� 
			}
			else if(FLAG_Face_False){//��֤ʶ��
				FLAG_Face_False = 0;
				fn = (u8*)"warning.wav";
				Show_Str(60,190,240-60,16,"ʶ��ʧ��",16,0);				//��ʾ�������� 
			}	
		}
		
		strcpy((char*)pname,"2:/MUSIC/");				//����·��(Ŀ¼)
		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���

		

		
		audio_play_song(pname); 			 		//���������Ƶ�ļ�
		audio_play_song(pname); 
		FLAG_PlayMusic = 0;
	} 											  
	myfree(SRAMIN,wavfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,wavindextbl);			//�ͷ��ڴ�	
	return 0;
}
//���ֲ��ſ�����
__audiodev audiodev;	

//��ʼ��Ƶ����
void audio_start(void)
{
	audiodev.status=3<<0;//��ʼ����+����ͣ
	I2S_Play_Start();
} 
//�ر���Ƶ����
void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 audio_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//ȡ����λ,�����ǲ��������ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}

 
//��������
void audio_play(void)
{
	
	//��ʼ��USB����
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
	
	while(FLAG_PlayMusic){
		//�ú�������ѭ������,����U��ʶ��׶�,ʶ���,��USB�жϴ�����������(��д)
		USBH_Process(&USB_OTG_Core, &USB_Host);
		delay_ms(1);
		
 	}
} 
//����ĳ����Ƶ�ļ�
u8 audio_play_song(u8* fname)
{
	u8 res;  
	res=f_typetell(fname); 
	switch(res)
	{
		case T_WAV:
			res=wav_play_song(fname);
			break;
		default://�����ļ�,�Զ���ת����һ��
			//res=KEY0_PRES;
			break;
	}
	return res;
}



























