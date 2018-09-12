#ifndef __LCD1602_H_
#define __LCD1602_H_
#include <STC12C5A.h>
//定义使能
sbit RS=P3^6;
sbit EN=P3^7;
//声明函数
void delay(unsigned int t);
void WriteCmd(unsigned char dat);
void WriteDat(unsigned char dat);
void Set_xy_LCM(unsigned char x, unsigned char y);
void WriteString(unsigned char *p);
void init_LCD1602();
void delay(unsigned int t);

void write_len0_char(unsigned char *p);
void write_len1_char(unsigned char p);
#endif
