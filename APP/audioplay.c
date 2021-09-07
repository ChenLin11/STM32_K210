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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//音乐播放器 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 
//音乐播放标志
u8 FLAG_PlayMusic;  
extern u8 FLAG_Face_True;   //人脸认证成功
extern u8 FLAG_Face_False;   //人脸认证成失败

USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

u8 USH_User_App(void){//usb响应函数
	u8 res;
 	DIR wavdir;	 		//目录
	FILINFO wavfileinfo;//文件信息
	u8 *fn;   			//长文件名
	u8 *pname;			//带路径的文件名
	u16 totwavnum; 		//音乐文件总数
	u16 *wavindextbl;	//音乐索引表
	
	WM8978_ADDA_Cfg(1,0);	//开启DAC
	WM8978_Input_Cfg(0,0,0);//关闭输入通道
	WM8978_Output_Cfg(1,0);	//开启DAC输出 
	while(f_opendir(&wavdir,"2:/MUSIC"))//打开音乐文件夹
 	{	    
		Show_Str(60,190,240,16,"MUSIC文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(60,190,240,206,WHITE);//清除显示	     
		delay_ms(200);				  
	} 									  
	totwavnum=audio_get_tnum("2:/MUSIC"); //得到总有效文件数
 									   
  	wavfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	wavfileinfo.lfname=mymalloc(SRAMIN,wavfileinfo.lfsize);	//为长文件缓存区分配内存
 	pname=mymalloc(SRAMIN,wavfileinfo.lfsize);				//为带路径的文件名分配内存
 	wavindextbl=mymalloc(SRAMIN,2*totwavnum);				//申请2*totwavnum个字节的内存,用于存放音乐文件索引
 	 	 
  res=f_opendir(&wavdir,(const TCHAR*)"2:/MUSIC"); 	//打开目录
	if(res==FR_OK)//打开成功
	{	
		if(FLAG_PlayMusic){//如果要播放音乐，说明识别有了个结果，发送中断
			FLAG_PlayMusic = 0;
			LCD_Fill(60,190,240,190+16,WHITE);				//清除之前的显示
			
			if(FLAG_Face_True){//认证成功
				FLAG_Face_True = 0;
				fn = (u8*)"ring.wav";
				Show_Str(60,190,240-60,16,"识别成功",16,0);				//显示歌曲名字 
			}
			else if(FLAG_Face_False){//认证识别
				FLAG_Face_False = 0;
				fn = (u8*)"warning.wav";
				Show_Str(60,190,240-60,16,"识别失败",16,0);				//显示歌曲名字 
			}	
		}
		
		strcpy((char*)pname,"2:/MUSIC/");				//复制路径(目录)
		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面

		

		
		audio_play_song(pname); 			 		//播放这个音频文件
		audio_play_song(pname); 
		FLAG_PlayMusic = 0;
	} 											  
	myfree(SRAMIN,wavfileinfo.lfname);	//释放内存			    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,wavindextbl);			//释放内存	
	return 0;
}
//音乐播放控制器
__audiodev audiodev;	

//开始音频播放
void audio_start(void)
{
	audiodev.status=3<<0;//开始播放+非暂停
	I2S_Play_Start();
} 
//关闭音频播放
void audio_stop(void)
{
	audiodev.status=0;
	I2S_Play_Stop();
}  
//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 audio_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO tfileinfo;	//临时文件信息		
	u8 *fn; 			 			   			     
    res=f_opendir(&tdir,(const TCHAR*)path); //打开目录
  	tfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//为长文件缓存区分配内存
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X40)//取高四位,看看是不是音乐文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	} 
	myfree(SRAMIN,tfileinfo.lfname);
	return rval;
}

 
//播放音乐
void audio_play(void)
{
	
	//初始化USB主机
  USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);  
	
	while(FLAG_PlayMusic){
		//该函数必须循环调用,仅在U盘识别阶段,识别后,由USB中断处理其他操作(读写)
		USBH_Process(&USB_OTG_Core, &USB_Host);
		delay_ms(1);
		
 	}
} 
//播放某个音频文件
u8 audio_play_song(u8* fname)
{
	u8 res;  
	res=f_typetell(fname); 
	switch(res)
	{
		case T_WAV:
			res=wav_play_song(fname);
			break;
		default://其他文件,自动跳转到下一曲
			//res=KEY0_PRES;
			break;
	}
	return res;
}



























