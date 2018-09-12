#include <stc12c5a.h>
#include <intrins.h>
#include "LCD1602.h"

void Timer0Init(void);
unsigned char morse_to_char();
void Delay10ms();
void UartInit();
void UART_send_string(unsigned char *p,unsigned char len);
void int_to_string(unsigned int val);

sbit key = P3^2;
/*
  1 is .
  0 is -
  填充规则如下:
                编码的初始值为1,
                每次按键对相应位置0或1
                向右环移
 */
code morse_code[] = {
    0x60, 0xE8, 0xA8, 0xD0, 0xC0, 0xB8, 0x90, 0xF8, 0xE0, //ABCDEFGHI
    0x18, 0x50, 0xD8, 0x20, 0xA0, 0x10, 0x98, 0x48, 0xB0, //JKLMNOPQR
    0xF0, 0x40, 0x70, 0x78, 0x30, 0x68, 0x28, 0x90,       //STUVWXYZ
    0x04, 0x0C, 0x1C, 0x3C, 0x7C, 0xFC, 0xF4, 0xE4, 0xC4, 0x84 //0123456789 
};

unsigned char str[5]="asdfg";       // 测试用字符串
unsigned int counter = 0;           // 定时器计数
unsigned int press_counter = 0;     // 按下按键的时间
unsigned int unpress_counter = 0;   // 未按下按键的时间

unsigned char encode_char=1;        
unsigned char decode_char=' ';

unsigned char new_word_flag = 1;    // 每个单词之间的空格标志位
unsigned char new_char_flag = 1;    // 每个字符之间的莫尔斯码的空格标志位

char chr;
char flag;

unsigned int key_speed=800;         // 手速定义, 一个. 按下的时间0~1500之间,单位百微妙

void main(){
    unsigned char press_flag = 0;
    init_LCD1602();
    Timer0Init();
    UartInit();
    EA=1;
    while(1){
        if(key==0){
            Delay10ms();
            if(key==0){
                TR0 = 0;  // 停止计时
                unpress_counter = counter;
                counter = 0;
                press_flag = 1;
                TR0 = 1;  // 启动计时
                while(!key);
                TR0 = 0;  // 停止计时
                press_counter = counter;
                counter = 0;
                TR0 = 1;  // 启动计时
            }
        }
        if(press_flag){
            new_word_flag = 1;
            new_char_flag = 1;
            // 按下了按键
            // 没有按下按键
            press_flag = 0;
            if(press_counter < 1500){
                // 认为输入是一个 .
                //key_speed = press_counter;
                encode_char = _cror_(encode_char,1);
                ++encode_char;
                write_len0_char(".");
            }else if(press_counter > 2*key_speed){
                // 认为输入是一个 -
                write_len0_char("_");
                encode_char = _cror_(encode_char,1);
            }
        }else{
            // 没有按下按键
            if(counter > 4*key_speed){
                if(new_char_flag){
                    unpress_counter = 0;
                    write_len0_char("/");
                    // 一个字符的结束
                    if(encode_char!=1){
                        encode_char = _cror_(encode_char,1);
                        decode_char = morse_to_char();
                        UART_send_string(&decode_char,1);
                        write_len1_char(decode_char);
                        encode_char = 1;
                    }
                    new_char_flag=0;
                }
                
            }
            if(counter > 12*key_speed){
                // 一个单词的结束
                if(new_word_flag){
                    if(encode_char!=1){
                        encode_char = _cror_(encode_char,1);
                        decode_char = morse_to_char();
                        UART_send_string(&decode_char,1);
                        write_len1_char(decode_char);
                        encode_char = 1;
                    }
                    write_len1_char('_');
                    new_word_flag=0;
                }
            }
        }
    }
}

unsigned char morse_to_char(){
    unsigned char i=0;
    for(i=0;i<36;i++){
        if(encode_char==morse_code[i]){
            break;
        }
    }
    if(i==36){
        return '#';
    }
    if(i<26){
        i += 'A';
    }else{
        i -= 26;
        i += '0';
    }
    return i;
}

void Timer0Init(void)		//100微秒@24.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x38;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;        //停止计时
    ET0 = 1;
}

void timer_serv() interrupt 1{
    TL0 = 0x38;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
    counter++;
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

void UartInit()		//9600bps@24.000MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xFB;		//独立波特率发生器时钟为Fosc/12,即12T
	BRT = 0xF3;		//设定独立波特率发生器重装值
	AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发生器
	EA=1;
	ES=1;
}

void UART_send_string(unsigned char *p,unsigned char len){
    unsigned char i=0;
	ES=0;
    
    for(i=0;i<len;i++){
        SBUF=p[i];
		while(!TI);
		TI=0;
    }
	ES=1;
}

void UART_server() interrupt 4{
	if(RI==1){
		RI=0;
		chr=SBUF;
		flag=1;
	}
}


void int_to_string(unsigned int val){
    char i=0;
    for(i=4;i>=0;--i){
        str[i] = '0'+val%10;
        val /= 10;
    }
}