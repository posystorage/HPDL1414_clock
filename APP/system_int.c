#include "system_int.h"
#include "HPDL1414.h"
#include "i2c.h"
#include "uart.h"
#include "18b20.h"

extern unsigned char time_dat[8];

extern bool effective_18b20;




void InitTimer4(void)
{
    TIM4_DeInit();
    //1ms
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
    TIM4_PrescalerConfig(TIM4_PRESCALER_128, TIM4_PSCRELOADMODE_IMMEDIATE);
    TIM4_ARRPreloadConfig(ENABLE);
    /* Clear TIM4 update flag */
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);

    /* Enable update interrupt */
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
    TIM4_Cmd(ENABLE);
    asm("rim");    //开总中断

}

void sys_int(void)
{ 
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);//2mhz-->16mhz        
    HPDL1414_gpio_init();
    InitTimer4();
    DS3231_Exti_Init();
    InitUart1();
        
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);//led
    GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT);//key1
    GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);//key2
    
      ITC->ISPR2 &= 0xf7;
      ITC->ISPR2 |= 0x04;
    __enable_interrupt(); 

    I2C_MASTERMODE_Init(FASTSPEED);
    DS3231_square_out_int();
     get_dat(time_dat) ;
     show_time_second(time_dat[3], time_dat[4], time_dat[5], 0);
      empty_display();    
      enableInterrupts();
      effective_18b20=DS18B20_Check();//检测18b20是否存在
}

