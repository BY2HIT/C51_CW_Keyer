#ifndef __KEY_H_
#define __KEY_H_
#include <STC12C5A.h>
#include "LCD1602.h"
sbit key = P1^3;
void init_interrupt_key();
void init_timer();
void Delay10ms();
#endif