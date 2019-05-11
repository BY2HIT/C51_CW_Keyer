#ifndef __BUZZER_H_
#define __BUZZER_H_
#include <STC12C5A.h>

#define BH 0xFB
#define BL 0x90

void T1INT();
void RingInit();


#endif