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
  ����������:
                ����ĳ�ʼֵΪ1,
                ÿ�ΰ�������Ӧλ��0��1
                ���һ���
 */
code morse_code[] = {
    0x60, 0xE8, 0xA8, 0xD0, 0xC0, 0xB8, 0x90, 0xF8, 0xE0, //ABCDEFGHI
    0x18, 0x50, 0xD8, 0x20, 0xA0, 0x10, 0x98, 0x48, 0xB0, //JKLMNOPQR
    0xF0, 0x40, 0x70, 0x78, 0x30, 0x68, 0x28, 0x90,       //STUVWXYZ
    0x04, 0x0C, 0x1C, 0x3C, 0x7C, 0xFC, 0xF4, 0xE4, 0xC4, 0x84 //0123456789 
};

unsigned char str[5]="asdfg";       // �������ַ���
unsigned int counter = 0;           // ��ʱ������
unsigned int press_counter = 0;     // ���°�����ʱ��
unsigned int unpress_counter = 0;   // δ���°�����ʱ��

unsigned char encode_char=1;        
unsigned char decode_char=' ';

unsigned char new_word_flag = 1;    // ÿ������֮��Ŀո��־λ
unsigned char new_char_flag = 1;    // ÿ���ַ�֮���Ī��˹��Ŀո��־λ

char chr;
char flag;

unsigned int key_speed=800;         // ���ٶ���, һ��. ���µ�ʱ��0~1500֮��,��λ��΢��

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
                TR0 = 0;  // ֹͣ��ʱ
                unpress_counter = counter;
                counter = 0;
                press_flag = 1;
                TR0 = 1;  // ������ʱ
                while(!key);
                TR0 = 0;  // ֹͣ��ʱ
                press_counter = counter;
                counter = 0;
                TR0 = 1;  // ������ʱ
            }
        }
        if(press_flag){
            new_word_flag = 1;
            new_char_flag = 1;
            // �����˰���
            // û�а��°���
            press_flag = 0;
            if(press_counter < 1500){
                // ��Ϊ������һ�� .
                //key_speed = press_counter;
                encode_char = _cror_(encode_char,1);
                ++encode_char;
                write_len0_char(".");
            }else if(press_counter > 2*key_speed){
                // ��Ϊ������һ�� -
                write_len0_char("_");
                encode_char = _cror_(encode_char,1);
            }
        }else{
            // û�а��°���
            if(counter > 4*key_speed){
                if(new_char_flag){
                    unpress_counter = 0;
                    write_len0_char("/");
                    // һ���ַ��Ľ���
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
                // һ�����ʵĽ���
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

void Timer0Init(void)		//100΢��@24.000MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0x38;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 0;        //ֹͣ��ʱ
    ET0 = 1;
}

void timer_serv() interrupt 1{
    TL0 = 0x38;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
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
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xFB;		//���������ʷ�����ʱ��ΪFosc/12,��12T
	BRT = 0xF3;		//�趨���������ʷ�������װֵ
	AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����
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