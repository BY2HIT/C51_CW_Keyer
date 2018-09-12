#include "LCD1602.h"

//写指令
void WriteCmd(unsigned char dat)
{
	RS=0; //写指令
	EN=0; //把EN置为低，以便于后面拉高
	P0=dat;//把要写的数据放到数据线上
	EN=1;//拉高EN线，让数据进入LCD
	delay(1);//维持高电平大于400ns
	EN=0;//拉低EN线，将数据线的状态锁存在LCD内
}
//写数据
void WriteDat(unsigned char dat)
{
	RS=1;
	EN=0;
	P0=dat;
	EN=1;
	delay(1);//维持高电平大于400ns
	EN=0;
}
// 设置光标位置
void Set_xy_LCM(unsigned char x, unsigned char y)
{
    unsigned char address;
    if( x == 0 )
       address = 0x80+y;
    else
       address = 0xc0+y;
    WriteCmd(address);
}
//写一行
void WriteString(unsigned char *p)
{
	while((*p)!='\0')//如果没有达到字符串结尾，则显示该字符
	{
		WriteDat(*p);//调用写字符函数向LCD写入一个字符
		p++;//指向字符串中下一个字符
	}
}
void init_LCD1602(){
	P0M0=0xFF;//设置推挽
	//初始化
	WriteCmd(0x38);
    WriteCmd(0x06);
    WriteCmd(0x0c);
	WriteCmd(0x01);   // 清屏
}
void delay(unsigned int t)
{
	unsigned char i;
	while(t--)
	{
		for	(i=0;i<255;i++);
	}
}

void write_len0_char(unsigned char *p){
    static unsigned char len0_index = 0;
    if(len0_index==17){
        Set_xy_LCM(0,0);
        WriteString("                ");
        len0_index=0;
    }
    Set_xy_LCM(0,len0_index);
    WriteString(p);
    ++len0_index;
}

void write_len1_char(unsigned char p){
    static unsigned char len1_index = 0;
    if(len1_index==17){
        Set_xy_LCM(1,0);
        WriteString("                ");
        len1_index=0;
    }
    Set_xy_LCM(1,len1_index);
    WriteDat(p);
    ++len1_index;
}