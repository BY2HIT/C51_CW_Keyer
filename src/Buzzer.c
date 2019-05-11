#include "Buzzer.h"
#include <stc12c5a.h>

sbit speaker = P4^6;


	
void T1INT() interrupt 3
{
	TH1 = BH; //��װ
  TL1 = BL;
  speaker=!speaker;          //�ߵ͵�ƽ�л�
}

void RingInit()
{
	P4SW|=0x40;		  //P4.6
	P4M0=0x40;		  //P4.6
	P4M1=0x00;
	speaker = 0;
	TMOD &= 0x0F;		//��ʱ��T1����Ϊ16λģʽ
	TMOD |= 0x10;		
	TH1 = BH;  //������ֵ����������Ƶ��
  TL1 = BL;
	TF1 = 0;
	ET1 = 1;
	EA = 1; 
	TR1 = 0;
}



