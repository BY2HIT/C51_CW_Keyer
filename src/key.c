#include "key.h"
unsigned char y=0;

void init_interrupt_key(){
//    EA=1;
//	EX0=1;
    P1M1=P1M1|0x08;
    P1M0=P1M0&0xf7;
    CMOD = 0x80;
    CCON = 0x00;
    CL = 0x00;
    CH=0x00;
    CCAPM0 = 0x31;
    EA=1;
    CR=1;
}

void Delay10ms()		//@24.000MHz
{
	unsigned char i, j, k;

	i = 1;
	j = 234;
	k = 113;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void interrupt_key() interrupt 7{
    EA=0;
    Delay10ms();
    if(key==0){
        Set_xy_LCM(0,y);
        WriteString("i");
        ++y;
    }
    CCF0 = 0;
    CCF1 = 0;
    EA=1;
}