#include "Buzzer.h"
#include <stc12c5a.h>

sbit speaker = P4^6;


	
void T1INT() interrupt 3
{
	TH1 = BH; //重装
  TL1 = BL;
  speaker=!speaker;          //高低电平切换
}

void RingInit()
{
	P4SW|=0x40;		  //P4.6
	P4M0=0x40;		  //P4.6
	P4M1=0x00;
	speaker = 0;
	TMOD &= 0x0F;		//定时器T1设置为16位模式
	TMOD |= 0x10;		
	TH1 = BH;  //计数初值，决定发声频率
  TL1 = BL;
	TF1 = 0;
	ET1 = 1;
	EA = 1; 
	TR1 = 0;
}



