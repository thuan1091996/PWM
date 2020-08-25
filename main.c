#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123ge6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/timer.c"
#include "driverlib/pwm.h"
#include "driverlib/pwm.c"

#define BUTTON_BASE GPIO_PORTF_BASE
#define LED_PIN     GPIO_PIN_1
#define BUTTON_PIN  GPIO_PIN_4

uint32_t duty_real,duty=0;
uint32_t Period;
uint8_t mode=0;
void PWM_Init();
void PWM_update(void);
void GPIOF_ISREDGE();
void GPIO_Init(void);
void Operation(void);
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    Period=SysCtlClockGet()/10000;
    PWM_Init();
    GPIO_Init();
   while (1)
   {
       Operation();
   }
}

void PWM_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN|PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, Period); //Set the load value
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
}

void PWM_update(void)
{
    duty_real=duty*Period/100+1; //+1 because if duty=0 -> duty_real=1 but 100% wont work correctly
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,duty_real);
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
}

void GPIO_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlDelay(3);
    GPIOPinTypeGPIOInput(BUTTON_BASE, BUTTON_PIN);
    GPIOPadConfigSet(BUTTON_BASE, BUTTON_PIN, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(BUTTON_BASE, BUTTON_PIN, GPIO_FALLING_EDGE);
    GPIOIntRegister(BUTTON_BASE, GPIOF_ISREDGE);
    GPIOIntEnable(BUTTON_BASE, GPIO_INT_PIN_4); //CAREFUL ABOUT FLAGS
}


void GPIOF_ISREDGE()
{
    SysCtlDelay(SysCtlClockGet()/10);
    mode=(mode+1)%3;
    GPIOIntClear(BUTTON_BASE,GPIO_INT_PIN_4);
}

void Operation(void)
{
    if(mode==0)
    {
        PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);
        duty=0;
    }
    if(mode==1)
    {
        duty=20;
        PWM_update();
    }
    if(mode==2)
    {
        duty=90;
        PWM_update();
    }
}
