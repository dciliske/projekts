#include <N76E003.h>
#include <SFR_Macro.h>
#include <Function_Define.h>

void main()
{
    P1M2 = 0x04;
    P1M1 = 0xFB;
    SFRS = 1;
    P1SR = 0x04;
    SFRS = 0;
    PWMPL = 0x01;
    PWM0L = 0x01;
    PWMRUN = 1;
    PIOCON0 = 0x01;
	while(1)
    {
        P12 = 0;
        __asm__ ("nop");
        P12 = 1;
    }
}
